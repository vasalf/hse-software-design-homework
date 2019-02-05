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
#include <tokenize/token.h>

#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace NCli {

class TTokenizeDFA final {
private:
    class TImpl;

public:
    class TExecCallback;

    class TState final {
    public:
        using TStateCallback = std::function<void (char, TExecCallback&)>;

        ~TState() = default;
        TState(const TState&) = delete;
        TState& operator=(const TState&) = delete;
        TState(TState&&) noexcept = default;
        TState& operator=(TState&&) noexcept = default;

        void SetCallback(TStateCallback&& callback);

    private:
        TState() = default;

        void operator()(char c, TExecCallback& callback) const;

        std::optional<TStateCallback> Callback_;

        friend class TTokenizeDFA::TImpl;
    };

    class TExecCallback final {
    public:
        ~TExecCallback() = default;
        TExecCallback(const TExecCallback&) = delete;
        TExecCallback& operator=(const TExecCallback&) = delete;
        TExecCallback(TExecCallback&&) noexcept = default;
        TExecCallback& operator=(TExecCallback&&) noexcept = default;

        void PushState(TState* to);
        void PopState();
        void PushStateAndDelegate(TState* to);
        void PopStateAndDelegate();
        void PushCharacter(TEscapedChar character);
        void StartToken();
        void EndToken();

    private:
        explicit TExecCallback(TTokenizeDFA::TImpl*);
        TTokenizeDFA::TImpl* DFAImpl_;

        friend class TTokenizeDFA::TImpl;
    };

    TTokenizeDFA();
    ~TTokenizeDFA();

    TTokenizeDFA(const TTokenizeDFA&) = delete;
    TTokenizeDFA& operator=(const TTokenizeDFA&) = delete;
    TTokenizeDFA(TTokenizeDFA&&) noexcept = default;
    TTokenizeDFA& operator=(TTokenizeDFA&&) noexcept = default;

    TState* ZeroState() const;
    TState* MakeState();
    TState* CurrentState() const;
    void Update(char c);
    void Update(std::string s);
    std::vector<TToken> ParsedTokens() const;

private:
    std::unique_ptr<TImpl> Impl_;
};

} // namespace NCli
