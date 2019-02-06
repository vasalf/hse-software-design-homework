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
#include <optional>

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

std::optional<std::string> ResolveFilename(TEnvironment& env, std::string filename) {
    namespace fs = std::filesystem;
    fs::path probe = fs::path(env["PWD"]) / filename;
    if (fs::exists(fs::path(env["PWD"]) / filename)) {
        return probe;
    }
    return {};
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
            auto file = ResolveFilename(Environment_, cmd.Args()[1]);
            if (file.has_value()) {
                std::ifstream fin(file.value());
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

TPwdExecutor::TPwdExecutor(TEnvironment& environment)
    : Environment_(environment)
{}

void TPwdExecutor::Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) {
    os << Environment_["PWD"] << std::endl;
}

TWcExecutor::TWcExecutor(TEnvironment& environment)
    : Environment_(environment)
{}

void TWcExecutor::Execute(const TCommand& cmd, IIStreamWrapper& in, std::ostream& os) {
    if (cmd.Args().size() > 2) {
        throw std::runtime_error("wc: Too many arguments");
    }

    bool readFromStdin = cmd.Args().size() == 1 || cmd.Args()[1] == "-";

    std::optional<std::string> resolvedFile = {};
    if (!readFromStdin) {
        resolvedFile = ResolveFilename(Environment_, cmd.Args()[1]);
    }
    if (!readFromStdin && !resolvedFile.has_value()) {
        throw std::runtime_error("wc: " + cmd.Args()[1] + ": No such file or directory");
    }
    std::unique_ptr<std::istream> fs;
    if (!readFromStdin) {
        fs = std::make_unique<std::ifstream>(resolvedFile.value());
    }

    std::istream& readFrom = readFromStdin ? in.WrappedIStream() : *fs;

    long lines = 0;
    {
        std::string s;
        while (std::getline(readFrom, s)) {
            lines++;
        }
        readFrom.clear();
        readFrom.seekg(0, std::ios::beg);
    }

    long words = 0;
    {
        std::string s;
        while (readFrom >> s) {
            words++;
        }
        readFrom.clear();
        readFrom.seekg(0, std::ios::beg);
    }

    long bytes = 0;
    {
        readFrom.seekg(0, std::ios::end);
        bytes = readFrom.tellg();
        readFrom.seekg(0, std::ios::beg);
    }

    os << "\t" << lines << "\t" << words << "\t" << bytes << std::endl;
}

} // namespace NPrivate
} // namespace NCli