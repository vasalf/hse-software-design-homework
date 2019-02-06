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

#include "istream_wrapper.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <unistd.h>

namespace NCli {

TIStreamWrapperBase::TIStreamWrapperBase(std::istream& is)
    : WrappedIStream_(is)
{}

std::istream& TIStreamWrapperBase::WrappedIStream() {
    return WrappedIStream_;
}

TPipeIStreamWrapper::TPipeIStreamWrapper(std::istream& is)
    : TIStreamWrapperBase(is)
{}

int TPipeIStreamWrapper::Dup(int fd1, int fd2) {
    return dup2(fd1, fd2);
}

void TPipeIStreamWrapper::CopyContentToFile(int fileDescriptor) {
    std::string input;
    std::copy(std::istreambuf_iterator<char>(WrappedIStream()),
              std::istreambuf_iterator<char>(),
              std::back_inserter(input));
    write(fileDescriptor, input.c_str(), input.size());
}

TStdinIStreamWrapper::TStdinIStreamWrapper(std::istream& is)
    : TIStreamWrapperBase(is)
{}

int TStdinIStreamWrapper::Dup(int, int) {
    return 0;
}

void TStdinIStreamWrapper::CopyContentToFile(int) {}

}
