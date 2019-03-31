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

#pragma once

#include <common/istream_wrapper.h>
#include <environment/environment.h>
#include <parser/parse.h>

#include <memory>
#include <string>

namespace NCli {

/**
 * This exception is thrown when an internal command is not a built-in and cannot be found in $PATH.
 */
class TCommandNotFoundException final : public std::runtime_error {
public:
    explicit TCommandNotFoundException(const std::string& command)
            : std::runtime_error(command + ": command not found")
    {}

    ~TCommandNotFoundException() override = default;
};

/**
 * This is an interface for executing a single command (NCli::TCommand).
 *
 * All built-in and external commands are derived from this class.
 */
class IExecutor {
public:
    virtual ~IExecutor() = default;

    /**
     * Execute the command {@arg command}, taking input from {@arg in} and printing output to {@arg out}.
     */
    virtual void Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& out) = 0;
};

using TExecutorPtr = std::shared_ptr<IExecutor>;

/**
 * This is a class with only one static method — the factory method for executors.
 */
class TExecutorFactory {
public:
    /**
     * Creates an executor. It is the only place in which private executors may be created. It is the place where CLI
     * decides whether a command is built-in or external.
     */
    static TExecutorPtr MakeExecutor(const std::string& command, TEnvironment& globalEnvironment);
};

/**
 * Updates the command environment {@arg env} with local variable assignments from {@arg command}.
 */
void UpdateCmdEnvironment(TCmdEnvironment& env, const TCommand& command);

} // namespace NCli