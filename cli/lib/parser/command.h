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

#include <string>
#include <vector>

namespace NCli {

/**
 * This represents a single input command without pipes.
 *
 * It contains the command, sequence of its args and sequence of local variable assignments.
 */
class TCommand final {
public:
    /**
     * Creates the command by parsing its command line.
     */
    explicit TCommand(std::vector<std::string> cmdline);

    /**
     * The command is supposed to be stored in std::vector, so it is copy-constructible and -assignable and
     * move-constructible and -assignable.
     */
    ~TCommand() = default;
    TCommand(const TCommand&) = default;
    TCommand& operator=(const TCommand&) = default;
    TCommand(TCommand&&) noexcept = default;
    TCommand& operator=(TCommand&&) noexcept = default;

    /**
     * Returns the command (the first token after assignments).
     */
    const std::string& Command() const;

    /**
     * Returns the command arguments (every token after assignments, including the command).
     */
    const std::vector<std::string>& Args() const;

    /**
     * Returns the sequence of local variable assignments before the actual command.
     */
    const std::vector<TAssignment>& Assignments()const;

private:
    std::vector<TAssignment> Assignments_;
    std::vector<std::string> Cmdline_;
};

} // namespace NCli