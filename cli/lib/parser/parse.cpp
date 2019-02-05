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

#include "parse.h"

namespace NCli {
namespace {

bool IsPipe(TExtChar c) {
    return c.ToChar() == '|'
           && c.EscapeStatus() == ECharEscapeStatus::UNESCAPED
           && c.IgnoranceStatus() == ECharIgnoranceStatus::NOTHING;
}

}

TFullCommand Parse(const std::vector<TToken>& tokens) {
    std::vector<std::string> nextCommand;
    TFullCommand ret;
    for (const auto& token: tokens) {
        if (token.Size() == 1 && IsPipe(token[0])) {
            ret.push_back(TCommand(nextCommand));
            nextCommand.clear();
        } else {
            nextCommand.push_back(token.ToString());
        }
    }
    if (!nextCommand.empty()) {
        ret.push_back(TCommand(nextCommand));
    }
    return ret;
}

} // namespace NCli