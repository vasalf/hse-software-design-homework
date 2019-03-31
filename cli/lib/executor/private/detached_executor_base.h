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

#include <common/pipe.h>
#include <executor/executor.h>

#include <memory>

namespace NCli {
namespace NPrivate {

/**
 * This is a base class for executors which have to be executed in a separate process.
 *
 * All external commands have to be executed in separate processes. Also, every executor which is reading something
 * from stdin should be executed in separate process because std::cin may become inconsistent after receiving EOF.
 */
class TDetachedExecutorBase : public IExecutor {
public:
    /**
     * Constructs an executor.
     */
    explicit TDetachedExecutorBase(TEnvironment& globalEnvironment);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TDetachedExecutorBase() override = default;
    TDetachedExecutorBase(const TDetachedExecutorBase&) = delete;
    TDetachedExecutorBase& operator=(const TDetachedExecutorBase&) = delete;
    TDetachedExecutorBase(TDetachedExecutorBase&&) noexcept = delete;
    TDetachedExecutorBase& operator=(TDetachedExecutorBase&&) noexcept = delete;

    /**
     * Calls {@link NCli::NPrivate::TDetachedExecutor::PreExec}, creates a separate process, handles stdin and stdout
     * correctly and calls {@link NCli::NPrivate::TDetachedExecutor::ExecuteChild} in it.
     */
    void Execute(const TCommand& command, IIStreamWrapper& in, std::ostream& out) final;

    /**
     * This is called before creating child process.
     */
    virtual void PreExec(TCmdEnvironment& cmdEnv, const TCommand& command);

    /**
     * This is called in the child process. It should read from stdin and write to stdout.
     *
     * @param command The command to be executed.
     * @param env Environment in which it is executed.
     * @return Return status, as in main function.
     */
    virtual int ExecuteChild(const TCommand& command, TCmdEnvironment& env) = 0;

private:
    TEnvironment& GlobalEnvironment_;
};

} // namespace NPrivate
} // namespace NCli