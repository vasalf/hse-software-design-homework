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

#include <environment/environment.h>
#include <parser/parse.h>

namespace NCli {

class TExecutor final {
public:
    TExecutor(TEnvironment& environment);

    ~TExecutor() = default;
    TExecutor(const TExecutor&) = delete;
    TExecutor& operator=(const TExecutor&) = delete;
    TExecutor(TExecutor&&) noexcept = delete;
    TExecutor& operator=(TExecutor&&) noexcept = delete;

    void Execute(const TFullCommand& command, std::istream& in, std::ostream& out);

private:
    TEnvironment& Environment_;
};

} // namespace NCli