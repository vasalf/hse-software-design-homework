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

#include "executor.h"

#include <algorithm>
#include <system_error>

#include <sys/wait.h>
#include <unistd.h>

namespace NCli {
namespace {

void DoExecute(TEnvironment& env, TCommand& command, std::istream& in, std::ostream& out) {
    TCmdEnvironment cmdEnv(env);
    for (const auto& a: command.Assignments()) {
        cmdEnv.SetLocalValue(a.Name, a.Value);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // child
        std::vector<char*> args(command.Args().size() + 1);
        std::transform(command.Args().begin(), command.Args().end(), args.begin(),
                       [](const std::string& s) {return const_cast<char*>(s.c_str());}
        );
        args.back() = nullptr;

        std::vector<std::string> envps = cmdEnv.ToEnvP();
        std::vector<char*> envp(envps.size() + 1);
        std::transform(envps.begin(), envps.end(), envp.begin(),
                       [](const std::string& s) { return const_cast<char*>(s.c_str()); }
        );
        envp.back() = nullptr;

        if (execvpe(const_cast<char*>(command.Command().c_str()), args.data(), envp.data()) == -1) {
            throw std::system_error(std::error_code(errno, std::system_category()));
        }
    } else {
        // parent
        waitpid(pid, nullptr, 0);
    }
}

} // namespace <anonymous>

TExecutor::TExecutor(TEnvironment& environment)
    : Environment_(environment)
{}

void TExecutor::Execute(const TFullCommand& command, std::istream& in, std::ostream& out) {

}

} // namespace NCli