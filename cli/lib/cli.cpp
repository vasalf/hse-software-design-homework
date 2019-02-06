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

#include "cli.h"

#include <environment/environment.h>
#include <environment/var_expander.h>
#include <executor/execute.h>
#include <tokenize/tokenizer.h>
#include <parser/parse.h>

namespace NCli {
namespace {

void LoopIteration(std::istream& in,
                   std::ostream& out,
                   std::ostream& err,
                   TEnvironment& env,
                   TVarExpander& varExpander) {
    out << "cli ";
    TTokenizer tokenizer;
    do {
        out << "> ";
        out.flush();
        std::string s;
        std::getline(in, s);
        s += "\n";
        tokenizer.Update(s);
    } while (tokenizer.State() == TTokenizer::EState::WAITING);

    std::vector<TToken> tokens = varExpander.Expand(tokenizer.ParsedTokens());

    TFullCommand command = Parse(tokens);

    Execute(command, env, in, out);
}

} // namespace <anonymous>

void RunMain(std::istream& in, std::ostream& out, std::ostream& err, char* envp[]) {
    TEnvironment environment = LoadGlobalEnvironment(const_cast<const char**>(envp));
    TVarExpander varExpander(environment);
    while (!in.eof()) {
        try {
            LoopIteration(in, out, err, environment, varExpander);
        } catch (std::exception& e) {
            err << e.what() << std::endl;
        } catch(...) {
            err << "unknown error" << std::endl;
        }
    }
}

} // namespace NCli
