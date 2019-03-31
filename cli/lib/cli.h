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

#include <iostream>
#include <common/istream_wrapper.h>

namespace NCli {

/**
 * Runs the CLI, reading input from {@arg is}, writing output to {@arg os} and writing errors to {@arg err} with initial
 * environment variables {@arg envp}.
 */
void RunMain(IIStreamWrapper& is, std::ostream& os, std::ostream& err, char* envp[]);

} // namespace NCli