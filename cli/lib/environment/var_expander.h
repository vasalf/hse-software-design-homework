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
#include <tokenizer/token.h>

namespace NCli {

/**
 * Substitutes the environmental variable references by its values in the command.
 *
 * The values are placed in the tokens as extended chars with default escape and ignorance statuses.
 */
class TVarExpander {
public:
    /**
     * Constructs the expander. Takes a reference to the environment.
     */
    explicit TVarExpander(TEnvironment& environment);

    /**
     * This class holds a reference to the global environment so it is not copy-constructible nor copy-assignable nor
     * move-constructible nor move-assignable.
     */
    ~TVarExpander() = default;
    TVarExpander(const TVarExpander&) = delete;
    TVarExpander& operator=(const TVarExpander&) = delete;
    TVarExpander(TVarExpander&&) noexcept = delete;
    TVarExpander& operator=(TVarExpander&&) noexcept = delete;

    /**
     * Performs the actual substitution.
     */
    std::vector<TToken> Expand(const std::vector<TToken>& tokens);
private:
    TEnvironment& Environment_;
};

} // namespace NCli