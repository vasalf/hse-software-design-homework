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

#include <gtest/gtest.h>

#include <executor/executor.h>
#include <parser/parse.h>
#include <tokenize/tokenizer.h>

#include <sstream>

using namespace NCli;

namespace {

void MakeCommand(std::string cmdline, TCommand& cmd) {
    TTokenizer tokenizer;
    tokenizer.Update(cmdline);
    ASSERT_EQ(TTokenizer::EState::DONE, tokenizer.State());
    cmd = TCommand(Parse(tokenizer.ParsedTokens())[0]);
}

}

TEST(ExecutorTest, echoAsExternalCommand) {
    TEnvironment env;
    env["PATH"] = getenv("PATH");
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("notbuiltin", env);

    std::istringstream is;
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand("echo -n abc   def\n", cmd);

    TPipeIStreamWrapper isw(is);
    executor->Execute(cmd, isw, os);

    ASSERT_EQ("abc def", os.str());
}

TEST(ExecutorTest, catMinusAsExternalCommand) {
    TEnvironment env;
    env["PATH"] = getenv("PATH");
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("notbuiltin", env);

    std::istringstream is("Hey there!\n");
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand("cat -\n", cmd);

    TPipeIStreamWrapper isw(is);
    executor->Execute(cmd, isw, os);

    ASSERT_EQ("Hey there!\n", os.str());
}