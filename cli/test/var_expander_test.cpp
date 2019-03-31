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
#include <environment/var_expander.h>
#include <tokenizer/tokenizer.h>

using namespace NCli;

namespace {

TEnvironment DefaultTestEnv() {
    TEnvironment environment;
    environment["PATH"] = "/bin:/usr/bin";
    environment["VAR"] = "value";
    environment["VARSP"] = "value with spaces";
    environment["BIN_NAME"] = "a";
    environment["x"] = "ex";
    environment["y"] = "it";
    return environment;
}

void DoTest(std::string test, std::vector<std::string> expected) {
    auto env = DefaultTestEnv();
    TVarExpander expander(env);

    TTokenizer tokenizer;
    tokenizer.Update(std::move(test));
    ASSERT_EQ(TTokenizer::EState::DONE, tokenizer.State());
    auto tokens = tokenizer.ParsedTokens();

    auto replacedTokens = expander.Expand(tokens);
    std::vector<std::string> actual;
    for (const auto& token: replacedTokens) {
        actual.push_back(token.ToString());
    }

    ASSERT_EQ(expected, actual);
}

} // namespace <anonymous>

TEST(VarExpanderTest, NoVars) {
    DoTest(
        "CC=/usr/local/bin make -j9\n",
        {"CC=/usr/local/bin", "make", "-j9"}
    );
}

TEST(VarExpanderTest, Vars) {
    DoTest(
        "PATH=$PATH:/usr/local/bin clang++ a.cpp -o$BIN_NAME\n",
        {"PATH=/bin:/usr/bin:/usr/local/bin", "clang++", "a.cpp", "-oa"}
    );
}

TEST(VarExpanderTest, VarsAndQuotes) {
    DoTest(
        R"('$VARSP' "$VARSP")" "\n",
        {"$VARSP", "value with spaces"}
    );
}

TEST(VarExpanderTest, QuotesAfterVars) {
    DoTest(
        R"(echo $VAR"SP")" "\n",
        {"echo", "valueSP"}
    );
}

TEST(VarExpanderTest, PipeAfterVar) {
    DoTest(
        "echo $VAR|cat -\n",
        {"echo", "value", "|", "cat", "-"}
    );
}

TEST(VarExpanderTest, SubsequentVars) {
    DoTest(
        "$x$y\n",
        {"exit"}
    );
}