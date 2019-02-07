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

#include <tokenize/tokenizer.h>

#include <string>
#include <vector>

using namespace NCli;

namespace {

void DoTest(std::vector<std::string> test, std::vector<std::string> expected) {
    TTokenizer tokenizer;
    for (std::size_t i = 0; i != test.size(); i++) {
        tokenizer.Update(test[i]);
        if (i + 1 != test.size()) {
            ASSERT_EQ(TTokenizer::EState::WAITING, tokenizer.State());
        }
    }
    ASSERT_EQ(TTokenizer::EState::DONE, tokenizer.State());

    auto actualTokens = tokenizer.ParsedTokens();
    std::vector<std::string> actual;
    actual.reserve(actualTokens.size());
    for (const auto& t : actualTokens) {
        actual.push_back(t.ToString());
    }
    ASSERT_EQ(expected, actual);
}

} // namespace <anonymous>

TEST(TokenizerTest, OneToken) {
    DoTest({"a\n"}, {"a"});
}

TEST(TokenizerTest, SeveralTokens) {
    DoTest({"a b c   defgh  ij\n"}, {"a", "b", "c", "defgh", "ij"});
}

TEST(TokenizerTest, EscapedEOL) {
    DoTest(
            {
                    R"(a b\
)",
                    "c d\n"
            },
            {"a", "bc", "d"}
    );
}

TEST(TokenizerTest, EscapedSpace) {
    DoTest(
            {R"(abc\ def)" "\n"},
            {"abc def"}
    );
}

TEST(TokenizerTest, EscapedQuotes) {
    DoTest(
            {R"(ab\'\\cdef\'\"ghi\"\ \"jk\')" "\n"},
            {R"(ab'\cdef'"ghi" "jk')"}
    );
}

TEST(TokenizerTest, SingleQuotes) {
    DoTest(
            {R"('ab cd' '\')" "\n"},
            {"ab cd", "\\"}
    );
}

TEST(TokenizerTest, DoubleQuotes) {
    DoTest(
            {R"("ab cd\"" gh\""ij")" "\n"},
            {R"(ab cd")", R"(gh"ij)"}
    );
}

TEST(TokenizerTest, SingleQuotesInsideDoubleQuotes) {
    DoTest(
            {R"(ab"cd'\\' ef" "'gh\''")" "\n"},
            {R"(abcd'\' ef)", "'gh''"}
    );
}

TEST(TokenizerTest, Pipelines) {
    DoTest(
            {"echo $VAR | cat - |cat -|grep value1|grep value2\n"},
            {"echo", "$VAR", "|", "cat", "-", "|", "cat", "-", "|", "grep", "value1", "|", "grep", "value2"}
    );
}