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

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace NCli {

/**
 * This data structure represents an assignment token from user input.
 *
 * Assignment tokens are tokens of kind "VARIABLE=value".
 */
struct TAssignment {
    /**
     * Variable name.
     */
    std::string Name;

    /**
     * Assigned value.
     */
    std::string Value;
};

/**
 * Tries to parse the token as an assignment.
 *
 * @return Empty in case the token does not represent an assignment, parsed assignment otherwise.
 */
std::optional<TAssignment> ParseEnvVarAssignment(const std::string& s);

/**
 * This type represents the environment, global or local.
 *
 * The environment is understood as a dictionary from variable name to its value.
 */
using TEnvironment = std::map<std::string, std::string>;

/**
 * Parses the global environment (usually the third optional argument to main function) and returns its representation
 * as NCli::TEnvironment.
 *
 * It skips any tokens that cannot be interpreted as assignments.
 */
TEnvironment LoadGlobalEnvironment(const char **envp);

/**
 * Represents an enviroment specific to a particular command. Stores a pair of reference to global environment and a
 * local environment.
 */
class TCmdEnvironment final {
public:
    /**
     * Constructs a command environment from global environment
     */
    explicit TCmdEnvironment(TEnvironment& globalEnvironment);

    /**
     * This class stores a reference, so it is not copy-constructible nor copy-assignable nor move-constructible nor
     * move-assignable.
     */
    ~TCmdEnvironment() = default;
    TCmdEnvironment(const TCmdEnvironment&) = delete;
    TCmdEnvironment& operator=(const TCmdEnvironment&) = delete;
    TCmdEnvironment(TCmdEnvironment&&) noexcept = delete;
    TCmdEnvironment& operator=(TCmdEnvironment&&) noexcept = delete;

    /**
     * Returns the const reference to variable value.
     *
     * Looks up its value in local environments, then, if no such variable found, in global environment.
     */
    const std::string& GetValue(const std::string& name) const;

    /**
     * Sets the variable value in the local environment. Does not affect the global environment.
     */
    void SetLocalValue(const std::string& name, const std::string& value);

    /**
     * Represents the environments in the same format as execve(2) takes. Does not put nullptr at the end.
     */
    std::vector<std::string> ToEnvP() const;

private:
    TEnvironment& GlobalEnvironment_;
    TEnvironment LocalEnvironment_;
    std::string EmptyString_;
};

} // namespace NCli
