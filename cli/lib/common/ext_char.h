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

#include <vector>

namespace NCli {

/**
 * Denotes whether the input character was escaped by a backslash.
 *
 * Many internal processes, such as tokenization, assumes same escaped and unescaped characters to be unequal. For
 * example, input string "do\ a\ thing" is assumed to be a single token.
 */
enum class ECharEscapeStatus {
    ESCAPED,
    UNESCAPED
};

/**
 * Denotes whether some part of a character should be ignored by internal processes.
 */
enum class ECharIgnoranceStatus {
    /**
     * No part of a character should be ignored.
     */
    NOTHING,

    /**
     * This indicates that a character was surrounded by single quotes, so some special characters, such as '$' or '\',
     * have no special meaning.
     */
    IGNORE_VARIABLES,

    /**
     * This indicates a "fake delimeter" special character, which is ignored by almost every internal process except for
     * variable expanding.
     */
    JUST_IGNORE
};

/**
 * Represents a character and some flags parsed from input string.
 *
 * Actual flags are the "escape status" and the "ignorance status".
 *
 * Instances of TExtChar are supposed to be stored in a std::vector, so the class is destructible, copy-constructible
 * and -assignable and move-constructible and -assignable.
 */
class TExtChar final {
public:
    /**
     * Constructs an extended character.
     *
     * @param c The input character.
     * @param escaped The escape status.
     * @param ignorance The ignorance status.
     */
    explicit TExtChar(char c,
                      ECharEscapeStatus escaped = ECharEscapeStatus::UNESCAPED,
                      ECharIgnoranceStatus ignorance = ECharIgnoranceStatus::NOTHING);

    ~TExtChar() = default;
    TExtChar(const TExtChar&) = default;
    TExtChar& operator=(const TExtChar&) = default;
    TExtChar(TExtChar&&) noexcept = default;
    TExtChar& operator=(TExtChar&&) noexcept = default;

    /**
     * This transforms an extended character to its view as a simple character.
     *
     * Actually, returns the stored input character.
     */
    char ToChar() const;

    /**
     * Returns the escape status of a character. See {@link NCli::EEscapeStatus} for more information.
     */
    ECharEscapeStatus EscapeStatus() const;

    /**
     *  Returns the ignorance status of a character. See {@link NCli::EIgnoranceStatus} for more information.
     */
    ECharIgnoranceStatus IgnoranceStatus() const;

    /**
     * Constructs a special "fake delimiter" character ignored by every internal process except for variable expanding.
     *
     * Fake delimiter characters serve as a barrier for reading variable names. They are placed instead of special
     * characters, quotes, for example.
     */
    static TExtChar FakeDelim();

private:
    char Char_;
    ECharEscapeStatus EscapeStatus_;
    ECharIgnoranceStatus IgnoranceStatus_;
};

} // namespace NCli