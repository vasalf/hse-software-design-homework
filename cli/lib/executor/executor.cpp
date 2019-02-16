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

#include "executor.h"

#include <executor/private/external_executor.h>
#include <executor/private/builtin_executors.h>

namespace NCli {

TExecutorPtr TExecutorFactory::MakeExecutor(const std::string& command, TEnvironment& globalEnvironment) {
    if (command.empty()) {
        return std::make_shared<NPrivate::TAssignmentExecutor>(globalEnvironment);
    } else if (command == "exit") {
        return std::make_shared<NPrivate::TExitExecutor>(globalEnvironment);
    } else if (command == "echo") {
        return std::make_shared<NPrivate::TEchoExecutor>(globalEnvironment);
    } else if (command == "cat") {
        return std::make_shared<NPrivate::TCatExecutor>(globalEnvironment);
    } else if (command == "pwd") {
        return std::make_shared<NPrivate::TPwdExecutor>(globalEnvironment);
    } else if (command == "wc") {
        return std::make_shared<NPrivate::TWcExecutor>(globalEnvironment);
    } else if (command == "grep") {
        return std::make_shared<NPrivate::TGrepExecutor>(globalEnvironment);
    } else if (command == "cd") {
        return std::make_shared<NPrivate::TGrepExecutor>(globalEnvironment);
    } else if (command == "ls") {
        return std::make_shared<NPrivate::TGrepExecutor>(globalEnvironment);
    } else {
        return std::make_shared<NPrivate::TExternalExecutor>(globalEnvironment);
    }
}

void UpdateCmdEnvironment(TCmdEnvironment& env, const TCommand& command) {
    for (const auto& assignment : command.Assignments()) {
        env.SetLocalValue(assignment.Name, assignment.Value);
    }
}

} // namespace NCli