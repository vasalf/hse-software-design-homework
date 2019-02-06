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

#include <executor/executor.h>


namespace NCli {
namespace NPrivate {

class TExternalExecutor final : public IExecutor {
public:
    explicit TExternalExecutor(TEnvironment& globalEnv);

    ~TExternalExecutor() override = default;
    TExternalExecutor(const TExternalExecutor&) = delete;
    TExternalExecutor& operator=(const TExternalExecutor&) = delete;
    TExternalExecutor(TExternalExecutor&&) = delete;
    TExternalExecutor& operator=(TExternalExecutor&&) = delete;

    void Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& out) override;

private:
    TEnvironment& GlobalEnv_;
};

} // namespace NPrivate
} // namespace NCli