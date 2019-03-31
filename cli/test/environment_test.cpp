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

#include <environment/environment.h>

using namespace NCli;

TEST(EnvironmentTest, LoadEmptyGlobalEnvironment) {
    const char* envp[] = {nullptr};
    auto env = LoadGlobalEnvironment(envp);
    ASSERT_TRUE(env.empty());
}

TEST(EnvironmentTest, LoadGlobalEnvironment) {
    const char* envp[] = {
            "PATH=/usr/bin:/bin",
            "SOME_VAR=SOME VALUE",
            "somevarwithsmallletters=some value with small letters",
            nullptr
    };

    auto env = LoadGlobalEnvironment(envp);

    TEnvironment expected;
    expected["PATH"] = "/usr/bin:/bin";
    expected["SOME_VAR"] = "SOME VALUE";
    expected["somevarwithsmallletters"] = "some value with small letters";

    ASSERT_EQ(expected, env);
}

TEST(EnvironmentTest, GetGlobalValue) {
    std::string SECRET = "/usr/bin:/bin";

    TEnvironment env;
    env["PATH"] = SECRET;

    TCmdEnvironment environment(env);
    ASSERT_EQ(SECRET, environment.GetValue("PATH"));
}

TEST(EnvironementTest, GetLocalValue) {
    std::string SECRET = "/usr/bin:/bin:/usr/local/bin";

    TEnvironment env;
    env["PATH"] = "/usr/bin:/bin";
    auto envCopy = env;

    TCmdEnvironment environment(env);

    environment.SetLocalValue("PATH", SECRET);
    ASSERT_EQ(SECRET, environment.GetValue("PATH"));
    ASSERT_EQ(envCopy, env);
}

TEST(EnvironmentTest, GetUnexistentValue) {
    TEnvironment env;
    TCmdEnvironment environment(env);
    ASSERT_EQ("", environment.GetValue(""));
}
