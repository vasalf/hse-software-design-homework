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

#include <algorithm>
#include <cstring>

namespace NCli {

TEnvironment LoadGlobalEnvironment(const char** envp) {
    TEnvironment ret;

    for (const char** varval = envp; *varval != nullptr; varval++) {
        const char* end = *varval + std::strlen(*varval);
        const char* eq = std::find(*varval, end, '=');

        std::string name;
        name.reserve(eq - *varval);
        std::copy(*varval, eq, std::back_inserter(name));

        std::string value;
        value.reserve(end - eq - 1);
        std::copy(eq + 1, end, std::back_inserter(value));

        ret[name] = value;
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

} // namespace NCli