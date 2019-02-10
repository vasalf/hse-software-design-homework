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
        : TDetachedExecutorBase(globalEnv)
{}

int TExternalExecutor::ExecuteChild(const TCommand& command, TCmdEnvironment& env) {
    char* argv[command.Args().size() + 1];
    std::transform(command.Args().begin(), command.Args().end(), argv,
                   [](const std::string& s) { return const_cast<char*>(s.c_str()); }
    );
    argv[command.Args().size()] = nullptr;

    auto environment = env.ToEnvP();

    char* envp[environment.size() + 1];
    std::transform(environment.begin(), environment.end(), envp,
                   [](const std::string& s) { return const_cast<char*>(s.c_str()); }
    );
    envp[environment.size()] = nullptr;

    if (execve(CmdPath_.c_str(), argv, envp) == -1) {
        ThrowSystemError();
    }

    return 0; // Unreachable
}

void TExternalExecutor::PreExec(TCmdEnvironment& cmdEnv, const TCommand& command) {
    CmdPath_ = FindPath(cmdEnv, command.Command());
}

} // namespace NPrivate
} // namespace NCli