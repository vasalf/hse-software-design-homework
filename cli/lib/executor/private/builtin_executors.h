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

#include <executor/executor.h>

namespace NCli {
namespace NPrivate {

class TAssignmentExecutor final : public IExecutor {
public:
    explicit TAssignmentExecutor(TEnvironment& env);

    ~TAssignmentExecutor() override = default;
    TAssignmentExecutor(const TAssignmentExecutor&) = delete;
    TAssignmentExecutor& operator=(const TAssignmentExecutor&) = delete;
    TAssignmentExecutor(TAssignmentExecutor&&) noexcept = delete;
    TAssignmentExecutor& operator=(TAssignmentExecutor&&) noexcept = delete;

    void Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& os);

private:
    TEnvironment& Environment_;
};

class TExitExecutor final : public IExecutor {
public:
    explicit TExitExecutor(TEnvironment&);
    ~TExitExecutor() override = default;

    TExitExecutor(const TExitExecutor&) = delete;
    TExitExecutor& operator=(const TExitExecutor&) = delete;
    TExitExecutor(TExitExecutor&&) noexcept = delete;
    TExitExecutor& operator=(TExitExecutor&&) noexcept = delete;

    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os);
};

class TEchoExecutor final : public IExecutor {
public:
    explicit TEchoExecutor(TEnvironment&);
    ~TEchoExecutor() override = default;

    TEchoExecutor(const TEchoExecutor&) = delete;
    TEchoExecutor& operator=(const TEchoExecutor&) = delete;
    TEchoExecutor(TEchoExecutor&&) noexcept = delete;
    TEchoExecutor& operator=(TEchoExecutor&&) noexcept = delete;

    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;
};

class TCatExecutor final : public IExecutor {
public:
    explicit TCatExecutor(TEnvironment& environment);
    ~TCatExecutor() override = default;

    TCatExecutor(const TCatExecutor&) = delete;
    TCatExecutor& operator=(const TCatExecutor&) = delete;
    TCatExecutor(TCatExecutor&&) noexcept = delete;
    TCatExecutor& operator=(TCatExecutor&&) noexcept = delete;

    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;

private:
    TEnvironment& Environment_;
};

class TPwdExecutor final : public IExecutor {
public:
    explicit TPwdExecutor(TEnvironment& environment);
    ~TPwdExecutor() override = default;

    TPwdExecutor(const TPwdExecutor&) = delete;
    TPwdExecutor& operator=(const TPwdExecutor&) = delete;
    TPwdExecutor(TPwdExecutor&&) noexcept = delete;
    TPwdExecutor& operator=(TPwdExecutor&&) noexcept = delete;

    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;

private:
    TEnvironment& Environment_;
};

} // namespace NPrivate
} // namespace NCli