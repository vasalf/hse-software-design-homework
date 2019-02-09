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

/**
 * Splits the input command into sequence of {@link NCli::TToken}.
 */
class TTokenizer final {
public:
    /**
     * Represents whether the tokenizer is waiting for more symbols or the command is complete.
     */
    enum class EState {
        WAITING,
        DONE
    };

    /**
     * Creates the tokenizer.
     */
    TTokenizer();
    ~TTokenizer();

    /**
     * Those restrictions are dictated by the {@link NCli::TTokenizeDFA}, which is stored inside.
     */
    TTokenizer(const TTokenizer&) = delete;
    TTokenizer& operator=(const TTokenizer&) = delete;
    TTokenizer(TTokenizer&&) noexcept = default;
    TTokenizer& operator=(TTokenizer&&) noexcept = default;

    /**
     * Updates the tokenizer state with a sequence of input symbols.
     */
    void Update(std::string s);

    /**
     * Returns the tokenizer state.
     *
     * @see NCli::TTokenizer::EState
     *
     * Note that the tokenizer expects a space or newline character at the end of input.
     */
    EState State() const;

    /**
     * Returns a sequence of parsed tokens.
     *
     * @see NCli::TTokenizeDFA::ParsedTokens
     */
    std::vector<TToken> ParsedTokens() const;

private:
    class TImpl;
    std::unique_ptr<TImpl> Impl_;
};

} // namespace NCli
