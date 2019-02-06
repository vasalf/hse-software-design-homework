/**
 * Copyright 2019 Vasily Alferov
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "external_executor.h"

#include <common/pipe.h>
#include <tokenize/tokenize_dfa.h>

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <system_error>

#include <sys/wait.h>
#include <unistd.h>

namespace NCli {
namespace NPrivate {
namespace {

void ThrowSystemError() {
    throw std::system_error(0, std::system_category());
}

void DoExecute(const std::string& command,
               const std::vector<std::string>& args,
               const std::vector<std::string>& environment,
               IIStreamWrapper& in,
               std::ostream& out) {
    TPipe childStdin;
    TPipe childStdout;

    pid_t pid = fork();
    if (pid == 0) {
        // child

        childStdin.RegisterDirection(TPipe::EDirection::IN);
        if (in.Dup(childStdin.ReadEndDescriptor(), STDIN_FILENO) == -1) {
            ThrowSystemError();
        }

        childStdout.RegisterDirection(TPipe::EDirection::OUT);
        if (dup2(childStdout.WriteEndDescriptor(), STDOUT_FILENO) == -1) {
            ThrowSystemError();
        }

        char* argv[args.size() + 1];
        std::transform(args.begin(), args.end(), argv,
                       [](const std::string& s) { return const_cast<char*>(s.c_str()); }
        );
        argv[args.size()] = nullptr;

        char* envp[environment.size() + 1];
        std::transform(environment.begin(), environment.end(), envp,
                       [](const std::string& s) { return const_cast<char*>(s.c_str()); }
        );
        envp[environment.size()] = nullptr;

        if (execve(command.c_str(), argv, envp) == -1) {
            ThrowSystemError();
        }
    } else {
        // parent

        childStdin.RegisterDirection(TPipe::EDirection::OUT);
        childStdout.RegisterDirection(TPipe::EDirection::IN);

        in.CopyContentToFile(childStdin.WriteEndDescriptor());
        childStdin.CloseWriteEnd();

        waitpid(pid, nullptr, 0);

        char buf[128];
        std::memset(buf, 0, sizeof(buf));
        ssize_t status;
        do {
            out << buf;
            status = read(childStdout.ReadEndDescriptor(), buf, sizeof(buf) - 1);
            buf[status] = 0;
        } while (status != 0);
    }
}

std::string FindPath(TCmdEnvironment& cmdEnv, const std::string& command) {
    namespace fs = std::filesystem;

    if (fs::exists(fs::path(command))) {
        return command;
    }

    TTokenizeDFA pathParser;
    TTokenizeDFA::TState* zero = pathParser.ZeroState();
    TTokenizeDFA::TState* escape = pathParser.MakeState();
    zero->SetCallback(
            [escape](char c, TTokenizeDFA::TExecCallback& cb) {
                if (c == '\\') {
                    cb.PushState(escape);
                } else if (c == ':') {
                    if (cb.TokenStarted()) {
                        cb.EndToken();
                    }
                    cb.StartToken();
                } else {
                    cb.PushCharacter(TExtChar(c));
                }
            }
    );
    escape->SetCallback(
            [](char c, TTokenizeDFA::TExecCallback& cb) {
                cb.PushCharacter(TExtChar(c, ECharEscapeStatus::ESCAPED));
                cb.PopState();
            }
    );

    std::string path = cmdEnv.GetValue("PATH");
    pathParser.Update(":" + path + ":");

    for (const TToken& searchPath : pathParser.ParsedTokens()) {
        fs::path check = fs::path(searchPath.ToString()) / fs::path(command);
        if (fs::exists(check)) {
            return check;
        }
    }

    throw TCommandNotFoundException(command);
}

} // namespace <anonymous>

TExternalExecutor::TExternalExecutor(TEnvironment& globalEnv)
        : GlobalEnv_(globalEnv)
{}

void TExternalExecutor::Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& out) {
    TCmdEnvironment cmdEnvironment(GlobalEnv_);
    UpdateCmdEnvironment(cmdEnvironment, command);
    DoExecute(FindPath(cmdEnvironment, command.Command()), command.Args(), cmdEnvironment.ToEnvP(), in, out);
}

} // namespace NPrivate
} // namespace NCli