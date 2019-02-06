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

namespace NCli {

class TPipe final {
public:
    TPipe();

    ~TPipe();

    TPipe(const TPipe&) = delete;
    TPipe& operator=(const TPipe&) = delete;
    TPipe(TPipe&&) noexcept = default;
    TPipe& operator=(TPipe&&) noexcept = default;

    enum class EDirection {
        UNSPECIFIED,
        IN,
        OUT
    };

    void RegisterDirection(EDirection direction);
    int ReadEndDescriptor() const;
    int WriteEndDescriptor() const;
    void CloseWriteEnd();

private:
    enum class EPipeEndStatus {
        OPEN,
        CLOSED
    };

    int FD_[2]{};
    EPipeEndStatus Status_[2] = {EPipeEndStatus::OPEN, EPipeEndStatus::OPEN};
    EDirection Direction_;
};

} // namespace NCli
