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

#include <common/exit_exception.h>
#include <executor/executor.h>
#include <parser/parse.h>
#include <tokenize/tokenizer.h>

#include <filesystem>
#include <fstream>
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

TEST(ExecutorTest, oneAssignment) {
    TEnvironment env;
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("", env);

    std::istringstream is;
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand("FILE=example.txt\n", cmd);

    executor->Execute(cmd, isw, os);

    ASSERT_EQ("example.txt", env["FILE"]);
}

TEST(ExecutorTest, manyAssignments) {
    TEnvironment env;
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("", env);

    std::istringstream is;
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand(R"(VAR_1="value 1"  VAR2='value 2' VAR_3=value\ 3)" "\n", cmd);

    executor->Execute(cmd, isw, os);

    ASSERT_EQ("value 1", env["VAR_1"]);
    ASSERT_EQ("value 2", env["VAR2"]);
    ASSERT_EQ("value 3", env["VAR_3"]);
}

TEST(ExecutorTest, exit) {
    TEnvironment env;
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("exit", env);

    std::istringstream is;
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand("exit\n", cmd);

    ASSERT_THROW(executor->Execute(cmd, isw, os), TExitException);
}

TEST(ExecutorTest, echoNothing) {
    TEnvironment env;
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("echo", env);

    std::istringstream is("ignored");
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand("echo\n", cmd);

    executor->Execute(cmd, isw, os);

    ASSERT_EQ("\n", os.str());
}

TEST(ExecutorTest, echoOneArg) {
    TEnvironment env;
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("echo", env);

    std::istringstream is("ignored");
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand(R"(echo "abc def"\ 'ghi jkl')" "\n", cmd);

    executor->Execute(cmd, isw, os);
    ASSERT_EQ("abc def ghi jkl\n", os.str());
}

TEST(ExecutorTest, echoManyArgs) {
    TEnvironment env;
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("echo", env);

    std::istringstream is("ignored");
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand(R"(echo a\   bcd 'ef\')" "\n", cmd);

    executor->Execute(cmd, isw, os);
    ASSERT_EQ("a  bcd ef\\\n", os.str());
}

namespace {

void DoCatStdinTest(std::string command) {
    std::string INPUT = "some\n  input";

    TEnvironment env;
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("cat", env);

    std::istringstream is(INPUT);
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand(std::move(command), cmd);

    executor->Execute(cmd, isw, os);
    ASSERT_EQ(INPUT, os.str());
}

} // namespace <anonymous>

TEST(ExecutorTest, catWithNoArgs) {
    DoCatStdinTest("cat\n");
}

TEST(ExecutorTest, catMinus) {
    DoCatStdinTest("cat -\n");
}

TEST(ExecutorTest, catFile) {
    std::string INPUT = "some\n input in the file";

    std::string filename = "tempXXXXXX";
    int fd = mkstemp(const_cast<char*>(filename.c_str()));
    {
        std::ofstream out(filename);
        out << INPUT;
    }

    TEnvironment env;
    env["PWD"] = getenv("PWD");
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("cat", env);

    std::istringstream is;
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand("cat " + filename + "\n", cmd);

    executor->Execute(cmd, isw, os);
    ASSERT_EQ(INPUT, os.str());

    close(fd);
    std::filesystem::remove(std::filesystem::path(filename));
}

TEST(ExecutorTest, pwd) {
    TEnvironment env;
    env["PWD"] = "/some/path";
    TExecutorPtr executor = TExecutorFactory::MakeExecutor("pwd", env);

    std::istringstream is;
    TPipeIStreamWrapper isw(is);
    std::ostringstream os;

    TCommand cmd({});
    MakeCommand("pwd\n", cmd);

    executor->Execute(cmd, isw, os);
    ASSERT_EQ("/some/path\n", os.str());
}