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

#include "tokenize_dfa.h"

#include <stdexcept>
#include <stack>

namespace NCli {
namespace {

class TInvalidDFAStateException : std::runtime_error {
public:
    explicit TInvalidDFAStateException(const std::string& component)
        : std::runtime_error("Invalid DFA " + component + " state")
    {}
};

} // namespace <anonymous>

class TTokenizeDFA::TImpl final {
public:
    TImpl() {
        ZeroState_ = MakeState();
        StateStack_.push(ZeroState_);
        TokenizerState_ = ETokenizerState::WAITING;
        DelegateState_ = EDelegateState::STOP;
    }

    ~TImpl() {
        for (auto p: OwnedStates_) {
            delete p;
        }
    }

    TImpl(const TImpl&) = delete;
    TImpl& operator=(const TImpl&) = delete;
    TImpl(TImpl&&) noexcept = default;
    TImpl& operator=(TImpl&&) noexcept = default;

    TState* ZeroState() const {
        return ZeroState_;
    }

    TState* MakeState() {
        OwnedStates_.push_back(new TState());
        return OwnedStates_.back();
    }

    TState* CurrentState() const {
        if (StateStack_.empty()) {
            throw TInvalidDFAStateException("stack");
        }

        return StateStack_.top();
    }

    void Update(char c) {
        DelegateState_ = EDelegateState::DELEGATE;
        while (DelegateState_ == EDelegateState::DELEGATE) {
            DelegateState_ = EDelegateState::STOP;
            TState& currentState = *CurrentState();
            TExecCallback callback(this);
            currentState(c, callback);
        }
    }

    std::vector<TToken> ParsedTokens() const {
        return CurTokens_;
    }

    void PushState(TState* to) {
        StateStack_.push(to);
    }

    void PopState() {
        if (StateStack_.empty()) {
            throw TInvalidDFAStateException("stack");
        }

        StateStack_.pop();
    }

    void PushStateAndDelegate(TState* to) {
        PushState(to);
        DelegateState_ = EDelegateState::DELEGATE;
    }

    void PopStateAndDelegate() {
        PopState();
        DelegateState_ = EDelegateState::DELEGATE;
    }

    void PushCharacter(TExtChar character) {
        if (TokenizerState_ == ETokenizerState::WAITING) {
            throw TInvalidDFAStateException("token");
        }

        NextToken_.PushBack(character);
    }

    void StartToken() {
        if (TokenizerState_ == ETokenizerState::WRITING) {
            throw TInvalidDFAStateException("token");
        }

        TokenizerState_ = ETokenizerState::WRITING;
    }

    void EndToken() {
        if (TokenizerState_ == ETokenizerState::WAITING) {
            throw TInvalidDFAStateException("token");
        }

        CurTokens_.push_back(NextToken_);
        NextToken_ = TToken();
        TokenizerState_ = ETokenizerState::WAITING;
    }

private:
    enum class ETokenizerState {
        WRITING,
        WAITING
    };

    enum class EDelegateState {
        DELEGATE,
        STOP
    };

private:
    std::vector<TState*> OwnedStates_;
    std::stack<TState*> StateStack_;

    TState* ZeroState_;

    ETokenizerState TokenizerState_;
    TToken NextToken_;
    std::vector<TToken> CurTokens_;

    EDelegateState DelegateState_;
};

TTokenizeDFA::TTokenizeDFA()
    : Impl_(std::make_unique<TImpl>())
{}

TTokenizeDFA::TState* TTokenizeDFA::MakeState() {
    return Impl_->MakeState();
}

void TTokenizeDFA::Update(char c) {
    Impl_->Update(c);
}

void TTokenizeDFA::Update(std::string s) {
    for (char c: s) {
        Update(c);
    }
}

std::vector<TToken> TTokenizeDFA::ParsedTokens() const {
    return Impl_->ParsedTokens();
}

TTokenizeDFA::TState* TTokenizeDFA::ZeroState() const {
    return Impl_->ZeroState();
}

TTokenizeDFA::TState* TTokenizeDFA::CurrentState() const {
    return Impl_->CurrentState();
}

TTokenizeDFA::~TTokenizeDFA() = default;

void TTokenizeDFA::TState::SetCallback(TTokenizeDFA::TState::TStateCallback&& callback) {
    Callback_ = callback;
}

void TTokenizeDFA::TState::operator()(char c, TTokenizeDFA::TExecCallback& callback) const {
    if (!Callback_.has_value()) {
        throw std::runtime_error("Uninitialized state used in DFA");
    }

    Callback_.value()(c, callback);
}

void TTokenizeDFA::TExecCallback::PushState(TTokenizeDFA::TState* to) {
    DFAImpl_->PushState(to);
}

void TTokenizeDFA::TExecCallback::PopState() {
    DFAImpl_->PopState();
}

void TTokenizeDFA::TExecCallback::PushStateAndDelegate(TTokenizeDFA::TState* to) {
    DFAImpl_->PushStateAndDelegate(to);
}

void TTokenizeDFA::TExecCallback::PopStateAndDelegate() {
    DFAImpl_->PopStateAndDelegate();
}

void TTokenizeDFA::TExecCallback::PushCharacter(TExtChar character) {
    DFAImpl_->PushCharacter(character);
}

void TTokenizeDFA::TExecCallback::StartToken() {
    DFAImpl_->StartToken();
}

void TTokenizeDFA::TExecCallback::EndToken() {
    DFAImpl_->EndToken();
}

TTokenizeDFA::TExecCallback::TExecCallback(TTokenizeDFA::TImpl* dfa)
    : DFAImpl_(dfa)
{}

} // namespace NCli
