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

#include "builtin_executors.h"

#include <common/exit_exception.h>
#include <common/pipe.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <optional>
#include <regex>

#include <unistd.h>
#include <sys/wait.h>

#include <CLI/CLI11.hpp>

namespace NCli {
namespace NPrivate {

TAssignmentExecutor::TAssignmentExecutor(TEnvironment& env)
    : Environment_(env)
{}

void TAssignmentExecutor::Execute(const TCommand& command, IIStreamWrapper&, std::ostream&) {
    for (const auto& assignment : command.Assignments()) {
        Environment_[assignment.Name] = assignment.Value;
    }
}

TExitExecutor::TExitExecutor(TEnvironment&) {}

void TExitExecutor::Execute(const TCommand&, IIStreamWrapper&, std::ostream&) {
    throw TExitException();
}

TEchoExecutor::TEchoExecutor(TEnvironment&) {}

void TEchoExecutor::Execute(const TCommand& cmd, IIStreamWrapper&, std::ostream& os) {
    for (std::size_t i = 1; i < cmd.Args().size() - 1; i++) {
        os << cmd.Args()[i] << " ";
    }
    if (cmd.Args().size() > 1) {
        os << cmd.Args().back();
    }
    os << std::endl;
}

namespace {

std::optional<std::string> ResolveFilename(TCmdEnvironment& env, std::string filename) {
    namespace fs = std::filesystem;
    fs::path probe = fs::path(env.GetValue("PWD")) / filename;
    if (fs::exists(probe)) {
        return probe;
    }
    if (fs::exists(fs::path(filename))) {
        return filename;
    }
    return {};
}

} // namespace <anonymous>

TCatExecutor::TCatExecutor(TEnvironment& environment)
    : TDetachedExecutorBase(environment)
{}

int TCatExecutor::ExecuteChild(const TCommand& command, TCmdEnvironment& env) {
    if (command.Args().size() > 2) {
        std::cerr << "cat: Too many arguments" << std::endl;
        return 1;
    }

    if (command.Args().size() == 1 || command.Args()[1] == "-") {
        std::copy(std::istreambuf_iterator<char>(std::cin),
                  std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(std::cout));
    } else {
        auto file = ResolveFilename(env, command.Args()[1]);
        if (file.has_value()) {
            std::ifstream fin(file.value());
            std::copy(std::istreambuf_iterator<char>(fin),
                      std::istreambuf_iterator<char>(),
                      std::ostreambuf_iterator<char>(std::cout));
        } else {
            std::cerr << "cat: " << command.Args()[1] << ": No such file or directory" << std::endl;
            return 1;
        }
    }

    return 0;
}

TPwdExecutor::TPwdExecutor(TEnvironment& environment)
    : Environment_(environment)
{}

void TPwdExecutor::Execute(const TCommand&, IIStreamWrapper&, std::ostream& os) {
    os << Environment_["PWD"] << std::endl;
}

TWcExecutor::TWcExecutor(TEnvironment& environment)
    : TDetachedExecutorBase(environment)
{}

int TWcExecutor::ExecuteChild(const TCommand& cmd, TCmdEnvironment& cmdEnv) {
    if (cmd.Args().size() > 2) {
        std::cerr << "wc: Too many arguments" << std::endl;
        return 1;
    }

    bool readFromStdin = cmd.Args().size() == 1 || cmd.Args()[1] == "-";

    std::optional<std::string> resolvedFile = {};
    if (!readFromStdin) {
        resolvedFile = ResolveFilename(cmdEnv, cmd.Args()[1]);
    }
    if (!readFromStdin && !resolvedFile.has_value()) {
        std::cerr << "wc: " + cmd.Args()[1] + ": No such file or directory" << std::endl;
        return 1;
    }
    std::unique_ptr<std::istream> fs;
    if (!readFromStdin) {
        fs = std::make_unique<std::ifstream>(resolvedFile.value());
    } else {
        std:: string allStdin;
        std::copy(std::istreambuf_iterator<char>(std::cin),
                  std::istreambuf_iterator<char>(),
                  std::back_inserter(allStdin));
        fs = std::make_unique<std::istringstream>(allStdin);
    }

    std::istream& readFrom = *fs;

    long lines = 0;
    {
        std::string s;
        while (std::getline(readFrom, s)) {
            lines++;
        }
        readFrom.clear();
        readFrom.seekg(0, std::ios::beg);
    }

    long words = 0;
    {
        std::string s;
        while (readFrom >> s) {
            words++;
        }
        readFrom.clear();
        readFrom.seekg(0, std::ios::beg);
    }

    long bytes = 0;
    {
        readFrom.seekg(0, std::ios::end);
        bytes = readFrom.tellg();
    }

    std::cout << "\t" << lines << "\t" << words << "\t" << bytes << std::endl;
    return 0;
}

namespace {

class TGrepBadOptionsException final : public std::runtime_error {
public:
    TGrepBadOptionsException(std::string message, int retCode)
        : std::runtime_error(message)
        , RetCode_(retCode)
    {}

    int RetCode() const {
        return RetCode_;
    }

private:
    int RetCode_;
};

struct TGrepOpts {
    bool IgnoreCase;
    std::optional<int> AfterContext;
    std::string Pattern;
    std::vector<std::string> Filenames;
};

TGrepOpts ParseGrepArgs(const TCommand& command) {
    CLI::App app{"Search for PATTERN in each FILE"};

    TGrepOpts opts;
    app.add_flag("-i,--ignore-case", opts.IgnoreCase, "ignore case distinctions");
    app.add_option("-A,--after-context", opts.AfterContext, "print NUM lines of trailing context");
    app.add_option("pattern", opts.Pattern, "PATTERN")->required();
    app.add_option("files", opts.Filenames, "FILES");

    try {
        std::vector<char*> args;
        std::transform(command.Args().begin(), command.Args().end(), std::back_inserter(args),
                       [](const std::string& s) { return const_cast<char*>(s.c_str()); }
        );
        app.parse((int)args.size(), args.data());
    } catch (CLI::ParseError& e) {
        throw TGrepBadOptionsException(e.what(), app.exit(e));
    }

    return opts;
}

void DoGrepIStream(const TGrepOpts& opts, const std::regex& pattern, std::istream& is) {
    int printLines = -1;
    std::string s;
    while (std::getline(is, s)) {
        if (std::regex_search(s, pattern)) {
            printLines = opts.AfterContext.value_or(0);
        }
        if (printLines >= 0) {
            std::cout << s << std::endl;
            printLines--;
        }
    }
}

} // namespace <anonymous>

TGrepExecutor::TGrepExecutor(TEnvironment& globalEnvironment)
        : TDetachedExecutorBase(globalEnvironment)
{}

int TGrepExecutor::ExecuteChild(const TCommand& command, TCmdEnvironment& env) {
    TGrepOpts opts;
    try {
        opts = ParseGrepArgs(command);
    } catch (TGrepBadOptionsException& e) {
        return e.RetCode();
    }

    auto regexStyle = std::regex_constants::grep;
    if (opts.IgnoreCase) {
        regexStyle |= std::regex_constants::icase;
    }
    std::regex pattern;
    try {
        pattern = std::regex(opts.Pattern, regexStyle);
    } catch (std::regex_error& e) {
        std::cerr << "grep: " << e.what() << std::endl;
        return 2;
    }

    int exitCode = 0;
    if (opts.Filenames.empty()) {
        DoGrepIStream(opts, pattern, std::cin);
    } else {
        for (const auto& file : opts.Filenames) {
            auto filename = ResolveFilename(env, file);
            if (!filename.has_value()) {
                std::cerr << "grep: " << file << ": No such file or directory" << std::endl;
                exitCode = 2;
            } else {
                std::ifstream fin(filename.value());
                DoGrepIStream(opts, pattern, fin);
            }
        }
    }

    return exitCode;
}

TLsExecutor::TLsExecutor(TEnvironment &globalEnvironment)
    : Environment_(globalEnvironment)
{} 

void TLsExecutor::Execute(const TCommand& command, IIStreamWrapper&, std::ostream& os) {
    namespace fs = std::filesystem;
    fs::path path; 
    if (command.Args().size() > 2) {
        std::cerr << "ls: Too many arguments" << std::endl;
        return;
    } else if (command.Args().size() < 2) {
        path = fs::path(Environment_["PWD"]);
    } else {
        path = fs::path(Environment_["PWD"]) / command.Args()[1];
        if (!fs::exists(path)) {
            std::cerr << "ls: " << command.Args()[1] << ": No such file or directory" << std::endl;
            return;
        }
    }
    for (const auto &entry: fs::directory_iterator(path)) {
        os << entry.path().filename().string() << "  ";
    }
    os << std::endl;
}

TCdExecutor::TCdExecutor(TEnvironment &globalEnvironment)
    : Environment_(globalEnvironment)
{} 

void TCdExecutor::Execute(const TCommand& command, IIStreamWrapper&, std::ostream& os) {
    namespace fs = std::filesystem;
    fs::path path; 
    if (command.Args().size() > 2) {
        std::cerr << "ls: Too many arguments" << std::endl;
        return;
    } else if (command.Args().size() < 2) {
        path = fs::path(Environment_["HOME"]);
    } else {
        path = fs::path(Environment_["PWD"]) / command.Args()[1];
        if (!fs::exists(path)) {
            std::cerr << "ls: " << command.Args()[1] << ": No such file or directory" << std::endl;
            return;
        }
    }
    Environment_["PWD"] = fs::canonical(path).string();
}

} // namespace NPrivate
} // namespace NCli