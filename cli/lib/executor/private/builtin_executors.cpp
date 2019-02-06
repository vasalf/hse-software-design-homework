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

#include <iomanip>

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

} // namespace NPrivate
} // namespace NCli