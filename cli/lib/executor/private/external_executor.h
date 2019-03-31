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

#include <executor/private/detached_executor_base.h>

namespace NCli {
namespace NPrivate {

/**
 * Executes an external command.
 *
 * Searches $PATH for an executable name, then does the actual execution.
 */
class TExternalExecutor final : public TDetachedExecutorBase {
public:
    /**
     * Creates the executor.
     */
    explicit TExternalExecutor(TEnvironment& globalEnv);

    /**
     * It is supposed that all executors are wrapped in std::shared_ptr. Every executor is not copy-constructible nor
     * -assignable nor move-constructible nor -assignable in order to ensure no illegal action is performed.
     */
    ~TExternalExecutor() override = default;
    TExternalExecutor(const TExternalExecutor&) = delete;
    TExternalExecutor& operator=(const TExternalExecutor&) = delete;
    TExternalExecutor(TExternalExecutor&&) = delete;
    TExternalExecutor& operator=(TExternalExecutor&&) = delete;

    /**
     * Searches through $PATH and finds an executable to be launched.
     */
    void PreExec(TCmdEnvironment& cmdEnv, const TCommand& command) override;

    /**
     * Executes the external command.
     */
    int ExecuteChild(const TCommand& command, TCmdEnvironment& env) override;

private:
    std::string CmdPath_;
};

} // namespace NPrivate
} // namespace NCli