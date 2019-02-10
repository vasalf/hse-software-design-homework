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

#include "detached_executor_base.h"

#include <cstring>
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

} // namespace <anonymous>

TDetachedExecutorBase::TDetachedExecutorBase(TEnvironment& globalEnvironment)
    : GlobalEnvironment_(globalEnvironment)
{ }

void TDetachedExecutorBase::Execute(const NCli::TCommand& command, NCli::IIStreamWrapper& in, std::ostream& out) {
    TCmdEnvironment cmdEnv(GlobalEnvironment_);
    UpdateCmdEnvironment(cmdEnv, command);

    PreExec(cmdEnv, command);

    TPipe childStdin;
    TPipe childStdout;

    pid_t pid = fork();
    if (pid < 0) {
        ThrowSystemError();
    } else if (pid == 0) {
        // child

        childStdin.RegisterDirection(TPipe::EDirection::IN);
        if (in.Dup(childStdin.ReadEndDescriptor(), STDIN_FILENO) == -1) {
            ThrowSystemError();
        }

        childStdout.RegisterDirection(TPipe::EDirection::OUT);
        if (dup2(childStdout.WriteEndDescriptor(), STDOUT_FILENO) == -1) {
            ThrowSystemError();
        }

        exit(ExecuteChild(command, cmdEnv));
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
            status = read(childStdout.ReadEndDescriptor(), buf, sizeof(buf));
            buf[status] = 0;
        } while (status != 0);
    }
}

void TDetachedExecutorBase::PreExec(TCmdEnvironment& cmdEnv, const TCommand& command) { }

} // namespace NPrivate
} // namespace NCli
