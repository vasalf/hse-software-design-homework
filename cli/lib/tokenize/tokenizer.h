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

#include <tokenize/token.h>

#include <memory>
#include <string>
#include <vector>

namespace NCli {

class TTokenizer final {
public:
    enum class EState {
        WAITING,
        DONE
    };

    TTokenizer();
    ~TTokenizer();

    TTokenizer(const TTokenizer&) = delete;
    TTokenizer& operator=(const TTokenizer&) = delete;
    TTokenizer(TTokenizer&&) noexcept = default;
    TTokenizer& operator=(TTokenizer&&) noexcept = default;

    void Update(std::string s);
    EState State() const;
    std::vector<TToken> ParsedTokens() const;

private:
    class TImpl;
    std::unique_ptr<TImpl> Impl_;
};

} // namespace NCli
