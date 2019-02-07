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

#include <cli.h>
#include <environment/environment.h>

#include <filesystem>
#include <fstream>

#include <unistd.h>

TEST(ExampleTest, Example) {
    std::string exampleContent = "Some example text\n";

    std::string filename = "tempXXXXXX";
    int fd = mkstemp(const_cast<char*>(filename.c_str()));
    {
        std::ofstream out(filename);
        out << exampleContent;
    }

    std::string input = R"(echo "Hello, World!")"       "\n"
                        R"(FILE=)" + filename +         "\n"
                        R"(cat $FILE)"                  "\n"
                        R"(cat )" + filename + " | wc"  "\n"
                        R"(echo 123 | wc)"              "\n"
                        R"(x=exit)"                     "\n"
                        R"($x)"                         "\n";
    std::string expectedOutput = "cli > "
                                 "Hello, World!\n"
                                 "cli > "
                                 "cli > "
                                 + exampleContent +
                                 "cli > "
                                 "\t1\t3\t18\n"
                                 "cli > "
                                 "\t1\t1\t4\n"
                                 "cli > "
                                 "cli > ";

    std::istringstream is(input);
    NCli::TStdinIStreamWrapper isw(is);
    std::ostringstream os;
    std::ostringstream err;

    NCli::TEnvironment env;
    env["PWD"] = getenv("PWD");
    auto envpVec = NCli::TCmdEnvironment(env).ToEnvP();
    std::vector<char*> envp(envpVec.size() + 1);
    std::transform(envpVec.begin(), envpVec.end(), envp.begin(),
        [](const std::string& s) { return const_cast<char*>(s.c_str()); }
    );

    NCli::RunMain(isw, os, err, envp.data());

    ASSERT_TRUE(err.str().empty());
    ASSERT_EQ(expectedOutput, os.str());

    close(fd);
    std::filesystem::remove(std::filesystem::path(filename));
}