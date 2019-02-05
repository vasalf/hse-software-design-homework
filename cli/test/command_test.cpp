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

#include <parser/command.h>

using namespace NCli;

TEST(CommandTest, noAssignments) {
    TCommand command({"echo", "abc", "def", "ghi"});
    ASSERT_TRUE(command.Assignments().empty());
    ASSERT_EQ("echo", command.Args()[0]);
    std::vector<std::string> expected = {"echo", "abc", "def", "ghi"};
    ASSERT_EQ(expected, command.Args());
}

TEST(CommandTest, emptyCmdline) {
    TCommand command({});
    ASSERT_TRUE(command.Assignments().empty());
    ASSERT_EQ("", command.Command());
    ASSERT_TRUE(command.Args().empty());
}

TEST(CommandTest, withAssignments) {
    TCommand command({"a=A", "b=BC", "echo", "A", "BC"});
    ASSERT_EQ(2, command.Assignments().size());
    ASSERT_EQ("a", command.Assignments()[0].Name);
    ASSERT_EQ("A", command.Assignments()[0].Value);
    ASSERT_EQ("b", command.Assignments()[1].Name);
    ASSERT_EQ("BC", command.Assignments()[1].Value);
    ASSERT_EQ("echo", command.Command());
    std::vector<std::string> expected = {"echo", "A", "BC"};
    ASSERT_EQ(expected, command.Args());
}

TEST(CommandTest, noArgs) {
    TCommand command({"exit"});
    ASSERT_TRUE(command.Assignments().empty());
    ASSERT_EQ("exit", command.Command());
    std::vector<std::string> expected = {"exit"};
    ASSERT_EQ(expected, command.Args());
}