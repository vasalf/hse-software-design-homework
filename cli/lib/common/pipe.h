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

/**
 * This is a C++ wrapper for a POSIX pipe.
 *
 * The pipe is supposed to be used in this way:
 *
 * NCli::TPipe pipe;
 * if (fork() == 0) {
 *     // child
 *     pipe.RegisterDirection(NCli::TPipe::EDirection::IN);
 *     // read from the read end
 * } else {
 *     // parent
 *     pipe.RegisterDirection(NCli::TPipe::EDirection::OUT);
 *     // write to the write end
 * }
 *
 * Alternatively, the RegisterDirection calls might be swapped.
 *
 * Note that the pipe is uni-directional. One must use two TPipes to make an bidirectional connection between the
 * processes.
 *
 * @see pipe(7)
 */
class TPipe final {
public:
    /**
     * Opens a new pipe.
     *
     * @see pipe(2)
     */
    TPipe();

    /**
     * Closes the opened descriptors.
     */
    ~TPipe();

    /**
     * This class is not copy-constructible nor copy-assignable. We can't create a full copy of a pipe in such way that
     * it would be available as a new pipe in another process.
     */
    TPipe(const TPipe&) = delete;
    TPipe& operator=(const TPipe&) = delete;

    /**
     * This class is move-constructible and move-assignable. Calling any of this delegates the responsibility to close
     * the file descriptors to a new object (in case of move constructor) or to the left operand (in case of move
     * assignment operator).
     */
    TPipe(TPipe&&) noexcept = default;
    TPipe& operator=(TPipe&&) noexcept = default;

    /**
     * Represents the direction in which the pipe is used in the current process.
     */
    enum class EDirection {
        /**
         * An uninitialized pipe.
         */
        UNSPECIFIED,

        /**
         * This process reads from the pipe.
         */
        IN,

        /**
         * This process writes to the pipe.
         */
        OUT
    };

    /**
     * Registeres the direction of the pipe.
     *
     * This also closes the unused file descriptor, as it is recommended by the manual page {@see pipe(2)}.
     *
     * This method **must** be called **once in each process** with different arguments. Attempt to call it twice from
     * the same process will cause a std::logic_error.
     */
    void RegisterDirection(EDirection direction);

    /**
     * Returns the descriptor of the read end of the pipe.
     *
     * Attempt to call it in case of unregistered or registered write direction will cause a std::logic_error.
     * Note that it is NCli::TPipe responsibility to close the file descriptors. Attempt to close it manually will
     * result in double-closing of descriptor, which is an error with errno in case the descriptor was not reused or
     * closing an unrelated file in case it was.
     */
    int ReadEndDescriptor() const;

    /**
     * Returns the descriptor of the write end of the pipe.
     *
     * Attempt to call if in case of unregistered or registered read direction will cause a std::logic_error.
     * Note that it is NCli::TPipe responsibility to close the file descriptors. Attempt to close it manually will
     * result in double-closing of descriptor, which is an error with errno in case the descriptor was not reused or
     * closing an unrelated file in case it was.
     */
    int WriteEndDescriptor() const;

    /**
     * Closes the write end of the pipe.
     *
     * As the result, the read end of the pipe will reach EOF.
     */
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
