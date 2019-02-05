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

#include "token.h"

namespace NCli {

std::string TToken::ToString() const {
    std::string ret;
    ret.reserve(Token_.size());
    for (std::size_t i = 0; i != Token_.size(); i++) {
        ret.push_back(Token_[i].ToChar());
    }
    return ret;
}

TEscapedChar& TToken::operator[](std::size_t i) {
    return Token_[i];
}

const TEscapedChar& TToken::operator[](std::size_t i) const {
    return Token_[i];
}

void TToken::PushBack(const TEscapedChar& character) {
    Token_.push_back(character);
}

void TToken::PushBack(TEscapedChar&& character) {
    Token_.push_back(std::forward<TEscapedChar>(character));
}

} // namespace NCli