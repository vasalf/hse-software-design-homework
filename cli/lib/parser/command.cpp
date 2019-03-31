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

#include "command.h"

#include <environment/environment.h>

namespace NCli {

TCommand::TCommand(std::vector<std::string> cmdline) {
    if (!cmdline.empty()) {
        auto it = cmdline.begin();
        auto assignment = ParseEnvVarAssignment(*it);
        while (it != cmdline.end() && assignment.has_value()) {
            Assignments_.push_back(assignment.value());
            it++;
            if (it != cmdline.end()) {
                assignment = ParseEnvVarAssignment(*it);
            }
        }
        std::copy(it, cmdline.end(), std::back_inserter(Cmdline_));
    }
}

const std::string& TCommand::Command() const {
    if (!Cmdline_.empty()) {
        return Cmdline_[0];
    } else {
        static std::string emptyString;
        return emptyString;
    }
}

const std::vector<std::string>& TCommand::Args() const {
    return Cmdline_;
}

const std::vector<TAssignment>& TCommand::Assignments() const {
    return Assignments_;
}


} // namespace NCli