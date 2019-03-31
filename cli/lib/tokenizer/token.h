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

#include <common/ext_char.h>

#include <string>
#include <vector>

namespace NCli {

/**
 * Represents a sequence of input characters understood by CLI as a single word. Stores a sequence of extended chars.
 */
class TToken {
public:
    /**
     * Constructs a token.
     */
    TToken() = default;

    /**
     * The token is supposed to be stored in std::vector, so it is copy-constructible and -assignable and
     * move-constructible and -assignable.
     */
    ~TToken() = default;
    TToken(const TToken&) = default;
    TToken& operator=(const TToken&) = default;
    TToken(TToken&&) noexcept = default;
    TToken& operator=(TToken&&) noexcept = default;

    /**
     * Constructs a string representation of a token.
     *
     * This takes account of ignorance and escape statuses of stored extended chars.
     */
    std::string ToString() const;

    /**
     * Returns a reference to extended char at given position.
     */
    TExtChar& operator[](std::size_t i);

    /**
     * Returns a const reference to extended char at given position.
     */
    const TExtChar& operator[](std::size_t i) const;

    /**
     * Appends a character to the token.
     */
    void PushBack(const TExtChar& character);

    /**
     * Returns number of stored extended characters, including fake delimiters.
     */
    std::size_t Size() const;

private:
    std::vector<TExtChar> Token_;
};

} // namespace NCli
