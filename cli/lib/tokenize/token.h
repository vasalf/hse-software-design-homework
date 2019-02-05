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

#include <common/escaped_char.h>

#include <string>
#include <vector>

namespace NCli {

class TToken {
public:
    TToken() = default;

    ~TToken() = default;
    TToken(const TToken&) = default;
    TToken& operator=(const TToken&) = default;
    TToken(TToken&&) noexcept = default;
    TToken& operator=(TToken&&) noexcept = default;

    std::string ToString() const;

    TEscapedChar& operator[](std::size_t i);
    const TEscapedChar& operator[](std::size_t i) const;

    void PushBack(const TEscapedChar& character);
    void PushBack(TEscapedChar&& character);

private:
    std::vector<TEscapedChar> Token_;
};

} // namespace NCli
