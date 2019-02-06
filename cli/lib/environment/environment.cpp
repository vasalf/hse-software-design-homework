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

#include "environment.h"

#include <common/char_utils.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

namespace NCli {

TEnvironment LoadGlobalEnvironment(const char** envp) {
    TEnvironment ret;

    for (const char** varval = envp; *varval != nullptr; varval++) {
        auto assignment = ParseEnvVarAssignment(std::string(*varval));
        if (assignment.has_value()) {
            std::cerr << assignment.value().Name << "=" << assignment.value().Value << std::endl;
            ret[assignment.value().Name] = assignment.value().Value;
        }
    }

    return ret;
}

namespace {

enum class EAssignmentParserState {
    NAME,
    VARIABLE
};

} // namespace <anonymous>

std::optional<TAssignment> ParseEnvVarAssignment(const std::string& s) {
    EAssignmentParserState curState = EAssignmentParserState::NAME;
    TAssignment ret{};
    for (char c: s) {
        if (curState == EAssignmentParserState::NAME) {
            if (c == '=') {
                curState = EAssignmentParserState::VARIABLE;
            } else if (!IsVariableNameLetter(c)) {
                return {};
            } else {
                ret.Name.push_back(c);
            }
        } else {
            ret.Value.push_back(c);
        }
    }
    if (curState == EAssignmentParserState::NAME) {
        return {};
    }
    return ret;
}

TCmdEnvironment::TCmdEnvironment(TEnvironment& globalEnvironment)
    : GlobalEnvironment_(globalEnvironment)
{}

const std::string& TCmdEnvironment::GetValue(const std::string& name) const {
    if (LocalEnvironment_.count(name)) {
        return LocalEnvironment_.at(name);
    }
    if (GlobalEnvironment_.count(name)) {
        return GlobalEnvironment_[name];
    }
    return EmptyString_;
}

void TCmdEnvironment::SetLocalValue(const std::string& name, const std::string& value) {
    LocalEnvironment_[name] = value;
}

std::vector<std::string> TCmdEnvironment::ToEnvP() const {
    std::vector<std::string> ret;
    for (auto p : GlobalEnvironment_) {
        if (!LocalEnvironment_.count(p.first)) {
            char buf[p.first.size() + 1 + p.second.size() + 1];
            std::sprintf(buf, "%s=%s", p.first.c_str(), p.second.c_str());
            ret.push_back(buf);
        }
    }
    for (auto p : LocalEnvironment_) {
        char buf[p.first.size() + 1 + p.second.size() + 1];
        std::sprintf(buf, "%s=%s", p.first.c_str(), p.second.c_str());
        ret.push_back(buf);
    }
    return ret;
}

} // namespace NCli