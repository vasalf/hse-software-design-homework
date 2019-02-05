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

#include "var_expander.h"

#include <common/char_utils.h>

#include <cctype>

namespace NCli {
namespace {

bool IsVariableIndicator(const TExtChar& c) {
    return c.ToChar() == '$'
           && c.EscapeStatus() == ECharEscapeStatus::UNESCAPED
           && c.IgnoranceStatus() != ECharIgnoranceStatus::IGNORE_VARIABLES;
}

bool IsVariableNameSymbol(const TExtChar& c) {
    return IsVariableNameLetter(c.ToChar())
           && c.EscapeStatus() == ECharEscapeStatus::UNESCAPED
           && c.IgnoranceStatus() == ECharIgnoranceStatus::NOTHING;
}

enum class EState {
    NOTHING,
    VARIABLE
};

TToken DoExpand(TEnvironment& env, const TToken& token) {
    TToken res;
    std::string var;
    EState state = EState::NOTHING;
    for (std::size_t i = 0; i != token.Size(); i++) {
        const auto& c = token[i];
        if (state == EState::NOTHING) {
            if (IsVariableIndicator(c)) {
                var.clear();
                state = EState::VARIABLE;
            } else {
                res.PushBack(c);
            }
        } else {
            if (IsVariableNameSymbol(c)) {
                var.push_back(c.ToChar());
            } else {
                for (char x: env[var]) {
                    res.PushBack(TExtChar(x));
                }
                state = EState::NOTHING;
                res.PushBack(c);
            }
        }
    }
    if (state == EState::VARIABLE) {
        for (char x: env[var]) {
            res.PushBack(TExtChar(x));
        }
    }
    return res;
}

} // namespace <anonymous>

TVarExpander::TVarExpander(TEnvironment& environment)
    : Environment_(environment)
{}

std::vector<TToken> TVarExpander::Expand(const std::vector<TToken>& tokens) {
    std::vector<TToken> result;
    for (const auto& token : tokens) {
        result.push_back(DoExpand(Environment_, token));
    }
    return result;
}

} // namespace NCli
