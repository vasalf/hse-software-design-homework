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

class TAssignmentExecutor final : public IExecutor {
public:
    TAssignmentExecutor(TEnvironment& env);

    ~TAssignmentExecutor() override = default;
    TAssignmentExecutor(const TAssignmentExecutor&) = delete;
    TAssignmentExecutor& operator=(const TAssignmentExecutor&) = delete;
    TAssignmentExecutor(TAssignmentExecutor&&) = delete;
    TAssignmentExecutor& operator=(TAssignmentExecutor&&) = delete;

    void Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& os);

private:
    TEnvironment& Environment_;
};

} // namespace NPrivate
} // namespace NCli