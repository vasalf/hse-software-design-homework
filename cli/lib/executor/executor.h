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

class TCommandNotFoundException final : public std::runtime_error {
public:
    explicit TCommandNotFoundException(const std::string& command)
            : std::runtime_error(command + ": command not found")
    {}

    ~TCommandNotFoundException() override = default;
};

class IExecutor {
public:
    virtual ~IExecutor() = default;

    virtual void Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& out) = 0;
};

using TExecutorPtr = std::shared_ptr<IExecutor>;

class TExecutorFactory {
public:
    static TExecutorPtr MakeExecutor(const std::string& command, TEnvironment& globalEnvironment);
};

void UpdateCmdEnvironment(TCmdEnvironment& env, const TCommand& command);

} // namespace NCli