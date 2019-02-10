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

#include <executor/execute.h>
#include <parser/parse.h>
#include <tokenizer/tokenizer.h>

#include <sstream>

using namespace NCli;

namespace {

void DoTest(std::string command, std::string in, std::string expectedOut) {
    TTokenizer tokenizer;
    tokenizer.Update(command);
    ASSERT_EQ(TTokenizer::EState::DONE, tokenizer.State());
    TFullCommand cmd = Parse(tokenizer.ParsedTokens());

    std::istringstream input(in);
    TPipeIStreamWrapper inputWrapper(input);
    std::ostringstream output;

    TEnvironment env;
    env["PATH"] = getenv("PATH");
    Execute(cmd, env, inputWrapper, output);

    ASSERT_EQ(expectedOut, output.str());
}

} // namespace <anonymous>

TEST(ExecuteTest, SingleCatMinus) {
    DoTest("cat -\n", "TOP SECRET", "TOP SECRET");
}

TEST(ExecuteTest, PipedCatMinuses) {
    DoTest("cat - | cat - | cat -\n", "TOP SECRET", "TOP SECRET");
}

TEST(ExecuteTest, DestroyedStdout) {
    DoTest("cat - | echo \"TOP SECRET\"\n", "UNEXPECTED", "TOP SECRET\n");
}

TEST(ExecuteTest, OneAssignment) {
    DoTest("FILE=example.txt\n", "", "");
}