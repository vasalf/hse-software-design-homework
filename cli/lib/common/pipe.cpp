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

#include "pipe.h"

#include <stdexcept>

#include <unistd.h>

namespace NCli {

TPipe::TPipe() {
    Direction_ = EDirection::UNSPECIFIED;
    pipe(FD_);
}

TPipe::~TPipe() {
    if (Status_[0] == EPipeEndStatus::OPEN) {
        close(FD_[0]);
    }
    if (Status_[1] == EPipeEndStatus::OPEN) {
        close(FD_[1]);
    }
}

void TPipe::RegisterDirection(TPipe::EDirection direction) {
    if (Direction_ != EDirection::UNSPECIFIED) {
        throw std::logic_error("invalid pipe state");
    }

    Direction_ = direction;
    if (direction == EDirection::IN) {
        close(WriteEndDescriptor());
        Status_[1] = EPipeEndStatus::CLOSED;
    } else if (direction == EDirection::OUT) {
        close(ReadEndDescriptor());
        Status_[0] = EPipeEndStatus::CLOSED;
    }
}

int TPipe::ReadEndDescriptor() const {
    return FD_[0];
}

int TPipe::WriteEndDescriptor() const {
    return FD_[1];
}

void TPipe::CloseWriteEnd() {
    if (Status_[1] == EPipeEndStatus::CLOSED) {
        throw std::logic_error("invalid pipe end status");
    }

    close(WriteEndDescriptor());
    Status_[1] = EPipeEndStatus::CLOSED;
}

} // namespace NCli
