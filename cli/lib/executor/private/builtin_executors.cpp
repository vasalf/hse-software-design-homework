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

#include "builtin_executors.h"

#include <common/exit_exception.h>
#include <common/pipe.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <unistd.h>
#include <sys/wait.h>

namespace NCli {
namespace NPrivate {

TAssignmentExecutor::TAssignmentExecutor(TEnvironment& env)
    : Environment_(env)
{}

void TAssignmentExecutor::Execute(const TCommand& command, IIStreamWrapper&, std::ostream&) {
    for (const auto& assignment : command.Assignments()) {
        Environment_[assignment.Name] = assignment.Value;
    }
}

TExitExecutor::TExitExecutor(TEnvironment&) {}

void TExitExecutor::Execute(const TCommand&, IIStreamWrapper&, std::ostream&) {
    throw TExitException();
}

TEchoExecutor::TEchoExecutor(TEnvironment&) {}

void TEchoExecutor::Execute(const TCommand& cmd, IIStreamWrapper&, std::ostream& os) {
    for (std::size_t i = 1; i < cmd.Args().size() - 1; i++) {
        os << cmd.Args()[i] << " ";
    }
    if (cmd.Args().size() > 1) {
        os << cmd.Args().back();
    }
    os << std::endl;
}

namespace {

void CopyPipeContentsToOStream(const TPipe& pipe, std::ostream& os) {
    char buf[128];
    std::memset(buf, 0, sizeof(buf));
    ssize_t status;
    do {
        os << buf;
        status = read(pipe.ReadEndDescriptor(), buf, sizeof(buf) - 1);
        buf[status] = 0;
    } while (status != 0);
}

} // namespace <anonymous>

TCatExecutor::TCatExecutor(TEnvironment& environment)
    : Environment_(environment)
{}

void TCatExecutor::Execute(const TCommand& cmd, IIStreamWrapper& in, std::ostream& os) {
    // In case of reading from stdin (`cat` or `cat -`) std::cin may become damaged after
    // receiving EOF, so we need to read the file in separate process.

    TPipe childStdin;
    TPipe childStdout;

    pid_t pid = fork();
    if (pid == 0) {
        // child

        childStdin.RegisterDirection(TPipe::EDirection::IN);
        if (in.Dup(childStdin.ReadEndDescriptor(), STDIN_FILENO) == -1) {
            exit(1);
        }
        childStdout.RegisterDirection(TPipe::EDirection::OUT);
        if (dup2(childStdout.WriteEndDescriptor(), STDOUT_FILENO) == -1) {
            exit(1);
        }

        if (cmd.Args().size() > 2) {
            std::cerr << "cat: Too many arguments" << std::endl;
            exit(1);
        }

        if (cmd.Args().size() == 1 || cmd.Args()[1] == "-") {
            std::copy(std::istreambuf_iterator<char>(std::cin),
                      std::istreambuf_iterator<char>(),
                      std::ostreambuf_iterator<char>(std::cout));
        } else {
            namespace fs = std::filesystem;
            fs::path file = fs::path(Environment_["PWD"]) / cmd.Args()[1];
            if (fs::exists(file)) {
                std::ifstream fin(file);
                std::copy(std::istreambuf_iterator<char>(fin),
                          std::istreambuf_iterator<char>(),
                          std::ostreambuf_iterator<char>(std::cout));
            } else {
                std::cerr << "cat: " << cmd.Args()[1] << ": No such file or directory" << std::endl;
                exit(1);
            }
        }

        exit(0);
    } else {
        // parent

        childStdin.RegisterDirection(TPipe::EDirection::OUT);
        in.CopyContentToFile(childStdin.WriteEndDescriptor());
        childStdin.CloseWriteEnd();
        childStdout.RegisterDirection(TPipe::EDirection::IN);

        waitpid(pid, nullptr, 0);

        CopyPipeContentsToOStream(childStdout, os);
    }
}

} // namespace NPrivate
} // namespace NCli