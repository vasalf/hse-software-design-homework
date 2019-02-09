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

#include <iosfwd>

namespace NCli {

/**
 * An interface for input stream wrappers.
 *
 * Those wrappers are necessary for executing external commands. The problem is that there must be different behaviour
 * for stdin streams and for internally stored streams when an internal command is reading input from stdin. When stdin
 * should be simply redirected, an input from another command must be sent through a pipe. This behaviour is implemented
 * in the children of this class.
 */
class IIStreamWrapper {
public:
    virtual ~IIStreamWrapper() = default;

    /**
     * This is called in a child process after fork in order to send pipe input to STDIN.
     *
     * @param fd1 File descriptor of a pipe read end.
     * @param fd2 File descriptor of STDIN.
     * @return {@see dup(2)}
     */
    virtual int Dup(int fd1, int fd2) = 0;

    /**
     * This is called in a parent process in order to send the whole input through a pipe.
     *
     * @param fileDescriptor File descriptor of a pipe write end.
     */
    virtual void CopyContentToFile(int fileDescriptor) = 0;

    /**
     * Returns the wrapped input stream.
     */
    virtual std::istream& WrappedIStream() = 0;
};

/**
 * A base class for input steram wrappers.
 *
 * This class stores a reference to an input stream and guarantees its children to be not copy-constructble nor
 * copy-assignable nor move-constructible nor move-assignable. The main reason for such a restriction is to guarantee
 * that instances of {@link NCli::IInputStream} are always passed by (non-const) reference.
 */
class TIStreamWrapperBase : public IIStreamWrapper {
public:
    explicit TIStreamWrapperBase(std::istream& is);

    ~TIStreamWrapperBase() override = default;
    TIStreamWrapperBase(const TIStreamWrapperBase&) = delete;
    TIStreamWrapperBase& operator=(const TIStreamWrapperBase&) = delete;
    TIStreamWrapperBase(TIStreamWrapperBase&&) = delete;
    TIStreamWrapperBase& operator=(TIStreamWrapperBase&&) = delete;

    /**
     * {@link NCli::IIStreamWrapper::WrappedIStream}
     */
    std::istream& WrappedIStream() final;
private:
    std::istream& WrappedIStream_;
};

/**
 * This is a wrapper for piped input from another command.
 */
class TPipeIStreamWrapper final : public TIStreamWrapperBase {
public:
    explicit TPipeIStreamWrapper(std::istream& is);

    ~TPipeIStreamWrapper() override = default;

    /**
     * {@link NCli::IIStreamWrapper::Dup}
     */
    int Dup(int fd1, int fd2) override;

    /**
     * {@link NCli::IIStreamWrapper::CopyContentToFile}
     */
    void CopyContentToFile(int fileDescriptor) override;

    using TIStreamWrapperBase::WrappedIStream;
};

/**
 * This is a wrapper for stdin.
 */
class TStdinIStreamWrapper final : public TIStreamWrapperBase {
public:
    explicit TStdinIStreamWrapper(std::istream& is);

    ~TStdinIStreamWrapper() override = default;

    /**
     * Actually, does nothing.
     */
    int Dup(int fd1, int fd2) override;

    /**
     * Actually, does nothing.
     */
    void CopyContentToFile(int fileDescriptor) override;

    using TIStreamWrapperBase::WrappedIStream;
};

}
