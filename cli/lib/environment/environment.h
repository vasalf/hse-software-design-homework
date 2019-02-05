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

#include <map>
#include <string>

namespace NCli {

using TEnvironment = std::map<std::string, std::string>;

TEnvironment LoadGlobalEnvironment(const char **envp);

class TCmdEnvironment {
public:
    explicit TCmdEnvironment(TEnvironment& globalEnvironment);

    ~TCmdEnvironment() = default;
    TCmdEnvironment(const TCmdEnvironment&) = delete;
    TCmdEnvironment& operator=(const TCmdEnvironment&) = delete;
    TCmdEnvironment(TCmdEnvironment&&) noexcept = delete;
    TCmdEnvironment& operator=(TCmdEnvironment&&) noexcept = delete;

    const std::string& GetValue(const std::string& name) const;
    void SetLocalValue(const std::string& name, const std::string& value);

private:
    TEnvironment& GlobalEnvironment_;
    TEnvironment LocalEnvironment_;
    std::string EmptyString_;
};

} // namespace NCli
