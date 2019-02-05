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

enum class ECharEscapeStatus {
    ESCAPED,
    UNESCAPED
};

class TEscapedChar final {
public:
    explicit TEscapedChar(char c, ECharEscapeStatus escaped = ECharEscapeStatus::UNESCAPED);

    ~TEscapedChar() = default;
    TEscapedChar(const TEscapedChar&) = default;
    TEscapedChar& operator=(const TEscapedChar&) = default;
    TEscapedChar(TEscapedChar&&) noexcept = default;
    TEscapedChar& operator=(TEscapedChar&&) noexcept = default;

    char ToChar() const;
    ECharEscapeStatus EscapeStatus() const;

private:
    char Char_;
    ECharEscapeStatus EscapeStatus_;
};

} // namespace NCli