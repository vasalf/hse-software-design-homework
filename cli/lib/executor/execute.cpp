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

#include "execute.h"

#include <executor/executor.h>

#include <memory>
#include <sstream>
#include <vector>

namespace NCli {
namespace {

TExecutorPtr SelectExecutor(const TCommand& command, TEnvironment& environment) {
    return TExecutorFactory::MakeExecutor(command.Command(), environment);
}

} // namespace <anonymous>

void Execute(const TFullCommand& fullCommand, TEnvironment& environment, std::istream& in, std::ostream& out) {
    if (fullCommand.empty()) {
        return;
    }

    std::vector<std::stringstream> intermediateStreams(fullCommand.size() - 1);
    std::vector<std::istream*> istreams(fullCommand.size());
    std::vector<std::ostream*> ostreams(fullCommand.size());

    istreams[0] = &in;
    for (std::size_t i = 1; i != fullCommand.size(); i++) {
        istreams[i] = &intermediateStreams[i - 1];
    }
    ostreams.back() = &out;
    for (std::size_t i = 0; i + 1 != fullCommand.size(); i++) {
        ostreams[i] = &intermediateStreams[i];
    }

    for (std::size_t i = 0; i != fullCommand.size(); i++) {
        const TCommand& command = fullCommand[i];
        SelectExecutor(command, environment)->Execute(command, *istreams[i], *ostreams[i]);
    }
}

} // namespace NCli
