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
#include <executor/private/detached_executor_base.h>

namespace NCli {
namespace NPrivate {

/**
 * Performs the assignments on global environment.
 *
 * This executor is created when the command token is empty. It is designed to performed actions like `FOO=bar`.
 */
class TAssignmentExecutor final : public IExecutor {
public:
    /**
     * Creates the executor.
     */
    explicit TAssignmentExecutor(TEnvironment& env);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TAssignmentExecutor() override = default;
    TAssignmentExecutor(const TAssignmentExecutor&) = delete;
    TAssignmentExecutor& operator=(const TAssignmentExecutor&) = delete;
    TAssignmentExecutor(TAssignmentExecutor&&) noexcept = delete;
    TAssignmentExecutor& operator=(TAssignmentExecutor&&) noexcept = delete;

    /**
     * {@link NCli::IExecutor::Execute}
     */
    void Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& os) override;

private:
    TEnvironment& Environment_;
};

/**
 * Throws an {@link NCli::ExitException}.
 *
 * This is the executor for builtin command `exit`.
 */
class TExitExecutor final : public IExecutor {
public:
    /**
     * Creates the executor.
     */
    explicit TExitExecutor(TEnvironment&);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TExitExecutor() override = default;
    TExitExecutor(const TExitExecutor&) = delete;
    TExitExecutor& operator=(const TExitExecutor&) = delete;
    TExitExecutor(TExitExecutor&&) noexcept = delete;
    TExitExecutor& operator=(TExitExecutor&&) noexcept = delete;

    /**
     * {@link NCli::IExecutor::Execute}
     */
    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;
};

/**
 * Writes its arguments to the output, separating with spaces.
 *
 * This is the executor for builtin command `echo`.
 */
class TEchoExecutor final : public IExecutor {
public:
    /**
     * Creates the executor.
     */
    explicit TEchoExecutor(TEnvironment&);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TEchoExecutor() override = default;
    TEchoExecutor(const TEchoExecutor&) = delete;
    TEchoExecutor& operator=(const TEchoExecutor&) = delete;
    TEchoExecutor(TEchoExecutor&&) noexcept = delete;
    TEchoExecutor& operator=(TEchoExecutor&&) noexcept = delete;

    /**
     * {@link NCli::IExecutor::Execute}
     */
    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;
};

/**
 * Writes the file content on stdout.
 *
 * When given no arguments or `-` as an argument, writes content of stdout.
 *
 * This is the executor for builtin command `cat`.
 */
class TCatExecutor final : public TDetachedExecutorBase {
public:
    /**
     * Creates the executor.
     */
    explicit TCatExecutor(TEnvironment& environment);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TCatExecutor() override = default;
    TCatExecutor(const TCatExecutor&) = delete;
    TCatExecutor& operator=(const TCatExecutor&) = delete;
    TCatExecutor(TCatExecutor&&) noexcept = delete;
    TCatExecutor& operator=(TCatExecutor&&) noexcept = delete;

    /**
     * Executes the command.
     */
    int ExecuteChild(const TCommand& command, TCmdEnvironment& env) override;
};

/**
 * Prints the $PWD content.
 *
 * This is the executor for builtin command `pwd`.
 */
class TPwdExecutor final : public IExecutor {
public:
    /**
     * Creates the executor.
     */
    explicit TPwdExecutor(TEnvironment& environment);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TPwdExecutor() override = default;
    TPwdExecutor(const TPwdExecutor&) = delete;
    TPwdExecutor& operator=(const TPwdExecutor&) = delete;
    TPwdExecutor(TPwdExecutor&&) noexcept = delete;
    TPwdExecutor& operator=(TPwdExecutor&&) noexcept = delete;

    /**
     * {@link NCli::IExecutor::Execute}
     */
    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;

private:
    TEnvironment& Environment_;
};

/**
 * Prints number of symbols, words and lines in standard input or in file.
 *
 * This is the executor for builtin command `wc`.
 */
class TWcExecutor final : public TDetachedExecutorBase {
public:
    /**
     * Creates the executor.
     */
    explicit TWcExecutor(TEnvironment& environment);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TWcExecutor() override = default;
    TWcExecutor(const TWcExecutor&) = delete;
    TWcExecutor& operator=(const TWcExecutor&) = delete;
    TWcExecutor(TWcExecutor&&) noexcept = delete;
    TWcExecutor& operator=(TWcExecutor&&) noexcept = delete;

    /**
     * Executes the command.
     */
    int ExecuteChild(const TCommand& command, TCmdEnvironment& env) override;
};

/**
 * Prints the line from the input that are matched by a regular expression.
 *
 * @see grep(1)
 */
class TGrepExecutor final : public TDetachedExecutorBase {
public:
    explicit TGrepExecutor(TEnvironment& globalEnvironment);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TGrepExecutor() override = default;
    TGrepExecutor(const TGrepExecutor&) = delete;
    TGrepExecutor& operator=(const TGrepExecutor&) = delete;
    TGrepExecutor(TGrepExecutor&&) noexcept = delete;
    TGrepExecutor& operator=(TGrepExecutor&&) = delete;

    /**
     * Executes the command
     */
     int ExecuteChild(const TCommand& command, TCmdEnvironment& env) override;
};

/**
 * Prints a list of names of all files and directiries in the current directory.
 *
 * This is the executor for builtin command `ls`.
 */
class TLsExecutor final : public IExecutor {
public:
    explicit TLsExecutor(TEnvironment& globalEnvironmrnt);
    
    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TLsExecutor() override = default;
    TLsExecutor(const TLsExecutor&) = delete;
    TLsExecutor& operator=(const TLsExecutor&) = delete;
    TLsExecutor(TLsExecutor&&) noexcept = delete;
    TLsExecutor& operator=(TLsExecutor&&) = delete;

    /**
     * {@link NCli::IExecutor::Execute}
     */
    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;

private:
    TEnvironment& Environment_;
};

/**
 * Changes directory to directory given as arg (or changes directory to home directory if arg is empty).
 *
 * This is the executor for builtin command `cd`.
 */
class TCdExecutor final : public IExecutor {
public:
    explicit TCdExecutor(TEnvironment& globalEnvironmrnt);
    
    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TCdExecutor() override = default;
    TCdExecutor(const TCdExecutor&) = delete;
    TCdExecutor& operator=(const TCdExecutor&) = delete;
    TCdExecutor(TCdExecutor&&) noexcept = delete;
    TCdExecutor& operator=(TCdExecutor&&) = delete;

    /**
     * {@link NCli::IExecutor::Execute}
     */
    void Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) override;

private:
    TEnvironment& Environment_;
};

} // namespace NPrivate
} // namespace NCli