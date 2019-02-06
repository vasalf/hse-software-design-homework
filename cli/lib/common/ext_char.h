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

enum class ECharIgnoranceStatus {
    NOTHING,
    IGNORE_VARIABLES,
    JUST_IGNORE
};

class TExtChar final {
public:
    explicit TExtChar(char c,
                      ECharEscapeStatus escaped = ECharEscapeStatus::UNESCAPED,
                      ECharIgnoranceStatus ignorance = ECharIgnoranceStatus::NOTHING);

    ~TExtChar() = default;
    TExtChar(const TExtChar&) = default;
    TExtChar& operator=(const TExtChar&) = default;
    TExtChar(TExtChar&&) noexcept = default;
    TExtChar& operator=(TExtChar&&) noexcept = default;

    char ToChar() const;
    ECharEscapeStatus EscapeStatus() const;
    ECharIgnoranceStatus IgnoranceStatus() const;

    static TExtChar FakeDelim();

private:
    char Char_;
    ECharEscapeStatus EscapeStatus_;
    ECharIgnoranceStatus IgnoranceStatus_;
};

} // namespace NCli