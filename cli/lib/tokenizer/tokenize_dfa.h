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
#include <tokenizer/token.h>

#include <functional>
#include <memory>
#include <optional>
#include <vector>

namespace NCli {

/**
 * This DFA-like class is used for splitting a string into tokens.
 *
 * Actually, that's not a true DFA. A **stack** of states is stored in the process of automation execution. Given the
 * next character, the automation gives it to the state from the top of the stack, which decides what to do with the
 * character. There are four legal actions to manipulate the stack. You may
 * 1) Push a new state on the stack.
 * 2) Push a new state and delegate the given character analysis to it (so its implementation will be called
 *    immediately.
 * 3) Pop the current state from the stack.
 * 4) Pop the current state from the stack and exit.
 *
 * This allows to simply analyze a formal language with many kind of quotes with different meanings. For example, it is
 * easy enough to handle characters escaped with backslash. Also, this allows to join strings in bash-style with
 * virtually no developing expenses.
 *
 * The goal of this class is to efficiently tokenize a string into sequence of {NCli::TToken}, so there are additional
 * methods in {@link NCli::TTokenizeDFA::TExecCallback} which are designed to manipulate the current token sequence.
 *
 * You may see an example of its usage in {@link NCli::TTokenizer}.
 */
class TTokenizeDFA final {
private:
    /**
     * An unusual design solution is to use pimpl idiom not only to hide implementation, but also to have two separate
     * interfaces: one for DFA users ({@link NCli::TTokenizeDFA}), another one for DFA state developers
     * ({@link NCli::TTokenizeDFA::TExecCallback}). Here TImpl represents the DFA and its current state, with state
     * stack and all rules stored in.
     *
     * Actually, both classes are calling methods from TImpl. But the state developers need not to call the methods
     * related to adding new symbols and updating the state. The DFA users need not to call the local state-updating
     * methods as well.
     *
     * Now {@link NCli::TTokenizeDFA} owns the {@link NCli::TTokenizeDFA::TImpl} while
     * {@link NCli::TTokenizeDFA::TExecCallback} only stores a weak pointer to it.
     */
    class TImpl;

public:
    class TExecCallback;

    /**
     * This represents a state of DFA (the one that is stored in the stack).
     */
    class TState final {
    public:
        /**
         * This is the type for state implementations.
         *
         * Functions of this type are called when an automation is asked to updates its state with a character. The
         * first argument is the character with which the automation is updated and the second is an interface for
         * the automation internal functions.
         */
        using TStateCallback = std::function<void (char, TExecCallback&)>;

        /**
         * The state should be constructed only inside the automation, so it is not copy-constructible nor
         * copy-assignable but move-constructible and move-assignable.
         */
        ~TState() = default;
        TState(const TState&) = delete;
        TState& operator=(const TState&) = delete;
        TState(TState&&) noexcept = default;
        TState& operator=(TState&&) noexcept = default;

        /**
         * Sets the callback to be called for analysis.
         *
         * @see NCli::TTokenizeDFA::TState::TStateCallback.
         */
        void SetCallback(TStateCallback&& callback);

    private:
        TState() = default;

        void operator()(char c, TExecCallback& callback) const;

        std::optional<TStateCallback> Callback_;

        friend class TTokenizeDFA::TImpl;
    };

    /**
     * This is the interface to DFA implementation for DFA state developers.
     */
    class TExecCallback final {
    public:
        ~TExecCallback() = default;

        /**
         * The callback should be only constructed in DFA implementation, so it is not copy-constructible nor
         * copy assignable but move-constructible and move-assignable.
         */
        TExecCallback(const TExecCallback&) = delete;
        TExecCallback& operator=(const TExecCallback&) = delete;
        TExecCallback(TExecCallback&&) noexcept = default;
        TExecCallback& operator=(TExecCallback&&) noexcept = default;

        /**
         * @see NCli::TTokenizeDFA
         */
        void PushState(TState* to);

        /**
         * @see NCli::TTokenizeDFA
         */
        void PopState();

        /**
         * @see NCli::TTokenizeDFA
         */
        void PushStateAndDelegate(TState* to);

        /**
         * @see NCli::TTokenizeDFA
         */
        void PopStateAndDelegate();

        /**
         * Inserts a new character to the current token. Adding a character when the token is not started will cause
         * std::runtime_error.
         */
        void PushCharacter(TExtChar character);

        /**
         * Starts a new token. Starting a new token when the old is not ended will cause std::runtime_error.
         */
        void StartToken();

        /**
         * Ends the token. Ending a token when it is not started will cause std::runtime_error.
         */
        void EndToken();

        /**
         * Returns whether the token is started or not.
         */
        bool TokenStarted();

    private:
        explicit TExecCallback(TTokenizeDFA::TImpl*);
        TTokenizeDFA::TImpl* DFAImpl_;

        friend class TTokenizeDFA::TImpl;
    };

    /**
     * Constructs the DFA with the only state — zero state.
     */
    TTokenizeDFA();
    ~TTokenizeDFA();

    /**
     * The DFA is not copy-constructible nor copy-assignable. The reason is that it seems that it is currently
     * impossible to construct new states with the same implementations, as the inner implementation stores the pointers
     * to the old states.
     */
    TTokenizeDFA(const TTokenizeDFA&) = delete;
    TTokenizeDFA& operator=(const TTokenizeDFA&) = delete;

    /**
     * The DFA is move-constructible and move-assignable.
     */
    TTokenizeDFA(TTokenizeDFA&&) noexcept = default;
    TTokenizeDFA& operator=(TTokenizeDFA&&) noexcept = default;

    /**
     * Returns the pointer to the zero state.
     */
    TState* ZeroState() const;

    /**
     * Creates a new state. Returns a pointer to it.
     *
     * The DFA totally takes responsibility to delete it when DFA destructor is called. The states are owned by DFA.
     */
    TState* MakeState();

    /**
     * Returns pointer to the state which is currently on the top of the implementation stack.
     *
     * Note that pointers can be legally compared on being equal within C++ standard/
     */
    TState* CurrentState() const;

    /**
     * Updates the DFA state with a single character.
     */
    void Update(char c);

    /**
     * Updates the DFA state with a sequence of characters.
     */
    void Update(std::string s);

    /**
     * Returns the sequence of currently parsed tokens.
     */
    std::vector<TToken> ParsedTokens() const;

private:
    std::unique_ptr<TImpl> Impl_;
};

} // namespace NCli
