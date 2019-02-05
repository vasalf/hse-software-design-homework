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

#include "tokenizer.h"

#include <tokenize/tokenize_dfa.h>

#include <cctype>
#include <utility>


namespace NCli {

class TTokenizer::TImpl {
    using TDFAState = TTokenizeDFA::TState;
    using TDFACallback = TTokenizeDFA::TExecCallback;

public:
    TImpl() {
        Zero_ = DFA_.ZeroState();
        Token_ = DFA_.MakeState();
        SingleQuote_ = DFA_.MakeState();
        DoubleQuote_ = DFA_.MakeState();
        Escape_ = DFA_.MakeState();
        Pipe_ = DFA_.MakeState();

        Zero_->SetCallback(
                [this](char c, TDFACallback& cb) {
                    if (c == '|') {
                        cb.PushStateAndDelegate(Pipe_);
                    } else if (!std::isspace(c)) {
                        cb.StartToken();
                        cb.PushStateAndDelegate(Token_);
                    }
                }
        );
        Token_->SetCallback(
                [this](char c, TDFACallback& cb) {
                    if (c == '\\') {
                        cb.PushState(Escape_);
                    } else if (c == '\'') {
                        cb.PushCharacter(TExtChar::FakeDelim());
                        cb.PushState(SingleQuote_);
                    } else if (c == '"') {
                        cb.PushCharacter(TExtChar::FakeDelim());
                        cb.PushState(DoubleQuote_);
                    } else if (std::isspace(c)) {
                        cb.EndToken();
                        cb.PopStateAndDelegate();
                    } else if (c == '|') {
                        cb.EndToken();
                        cb.PopState();
                        cb.PushStateAndDelegate(Pipe_);
                    } else {
                        cb.PushCharacter(TExtChar(c));
                    }
                }
        );
        SingleQuote_->SetCallback(
                [this](char c, TDFACallback& cb) {
                    if (c == '\'') {
                        cb.PushCharacter(TExtChar::FakeDelim());
                        cb.PopState();
                    } else {
                        cb.PushCharacter(TExtChar(c,
                                                  ECharEscapeStatus::ESCAPED,
                                                  ECharIgnoranceStatus::IGNORE_VARIABLES));
                    }
                }
        );
        DoubleQuote_->SetCallback(
                [this](char c, TDFACallback& cb) {
                    if (c == '\\') {
                        cb.PushState(Escape_);
                    } else if (c == '"') {
                        cb.PushCharacter(TExtChar::FakeDelim());
                        cb.PopState();
                    } else {
                        cb.PushCharacter(TExtChar(c));
                    }
                }
        );
        Escape_->SetCallback(
                [this](char c, TDFACallback& cb) {
                    if (c != '\n') {
                        cb.PushCharacter(TExtChar(c, ECharEscapeStatus::ESCAPED));
                    }
                    cb.PopState();
                }
        );
        Pipe_->SetCallback(
                [this](char c, TDFACallback& cb) {
                    cb.StartToken();
                    cb.PushCharacter(TExtChar(c));
                    cb.EndToken();
                    cb.PopState();
                }
        );
    }

    ~TImpl() = default;
    TImpl(const TImpl&) = delete;
    TImpl& operator=(const TImpl&) = delete;
    TImpl(TImpl&&) noexcept = default;
    TImpl& operator=(TImpl&&) noexcept = default;

    void Update(std::string s) {
        DFA_.Update(std::move(s));
    }

    TTokenizer::EState State() const {
        if (DFA_.CurrentState() == Zero_) {
            return TTokenizer::EState::DONE;
        } else {
            return TTokenizer::EState::WAITING;
        }
    }

    std::vector<TToken> ParsedTokens() const {
        return DFA_.ParsedTokens();
    }

private:
    TTokenizeDFA DFA_;
    TDFAState* Zero_;
    TDFAState* Token_;
    TDFAState* SingleQuote_;
    TDFAState* DoubleQuote_;
    TDFAState* Escape_;
    TDFAState* Pipe_;
};

TTokenizer::TTokenizer()
    : Impl_(std::make_unique<TImpl>())
{}

void TTokenizer::Update(std::string s) {
    Impl_->Update(std::move(s));
}

TTokenizer::EState TTokenizer::State() const {
    return Impl_->State();
}

std::vector<TToken> TTokenizer::ParsedTokens() const {
    return Impl_->ParsedTokens();
}

TTokenizer::~TTokenizer() = default;

} // namespace NCli
