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

class IIStreamWrapper {
public:
    virtual ~IIStreamWrapper() = default;

    virtual int Dup(int fd1, int fd2) = 0;
    virtual void CopyContentToFile(int fileDescriptor) = 0;

    virtual std::istream& WrappedIStream() = 0;
};

class TIStreamWrapperBase : public IIStreamWrapper {
public:
    explicit TIStreamWrapperBase(std::istream& is);

    ~TIStreamWrapperBase() override = default;
    TIStreamWrapperBase(const TIStreamWrapperBase&) = delete;
    TIStreamWrapperBase& operator=(const TIStreamWrapperBase&) = delete;
    TIStreamWrapperBase(TIStreamWrapperBase&&) = delete;
    TIStreamWrapperBase& operator=(TIStreamWrapperBase&&) = delete;

    std::istream& WrappedIStream() final;
private:
    std::istream& WrappedIStream_;
};

class TPipeIStreamWrapper final : public TIStreamWrapperBase {
public:
    explicit TPipeIStreamWrapper(std::istream& is);

    ~TPipeIStreamWrapper() override = default;

    int Dup(int fd1, int fd2) override;
    void CopyContentToFile(int fileDescriptor) override;

    using TIStreamWrapperBase::WrappedIStream;
};

class TStdinIStreamWrapper final : public TIStreamWrapperBase {
public:
    explicit TStdinIStreamWrapper(std::istream& is);

    ~TStdinIStreamWrapper() override = default;

    int Dup(int fd1, int fd2) override;
    void CopyContentToFile(int fileDescriptor) override;

    using TIStreamWrapperBase::WrappedIStream;
};

}
