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

#include <parser/parse.h>
#include <tokenize/tokenizer.h>

using namespace NCli;

namespace {

bool operator==(const TAssignment& lhs, const TAssignment& rhs) {
    return lhs.Name == rhs.Name && lhs.Value == rhs.Value;
}

TFullCommand DoParse(std::string input) {
    TTokenizer tokenizer;
    tokenizer.Update(std::move(input));
    auto tokens = tokenizer.ParsedTokens();
    return Parse(tokens);
}

} // namespace <anonymous>

TEST(ParseTest, empty) {
    ASSERT_TRUE(DoParse("\n").empty());
}

TEST(ParseTest, noAssignments) {
    auto result = DoParse(R"(echo \| b "| c"at -)" "\n");
    ASSERT_EQ(1, result.size());
    ASSERT_TRUE(result[0].Assignments().empty());
    ASSERT_EQ("echo", result[0].Command());
    ASSERT_EQ(5, result[0].Args().size());
}

TEST(ParseTest, assignments) {
    auto result = DoParse(R"(x=x echo $y | o=o cat -)" "\n");
    ASSERT_EQ(2, result.size());
    ASSERT_EQ(1, result[0].Assignments().size());
    ASSERT_EQ(1, result[1].Assignments().size());
}