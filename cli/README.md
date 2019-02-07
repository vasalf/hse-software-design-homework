# CLI

A bash-like command line emulator.

## Features

CLI has a bash-like syntax. It supports environment variable manipulation, several built-in commands, launch of external commands and pipes (`|`).
CLI features are demonstrated in the following example:

```
cli > echo "Hello, World!"
Hello, World!
cli > FILE=example.txt
cli > cat $FILE
Some example text
cli > cat example.txt | wc
    1   3   18
cli > echo 123 | wc
    1   1   4
cli > x=exit
cli > $x
```

Note that every command in the example is built-in.

## Build instructions

The emulator is written in C++17 and built by CMake, so modern versions of both CMake and C++ compilator are required.

### Ubuntu 18.04+

CMake and g++ from the official repository are fine.
You may install it by executing

```bash
# apt install cmake g++
```

The only dependency not covered by this command is GTest.
Note that even though the library is provided in the repository,
you still have to install it system-wide as it is needed by CMake.
You may do it by executing

```bash
# apt install libgtest-dev
# cmake /usr/src/gtest/CMakeLists.txt
# make -C /usr/src/gtest
# cp /usr/src/gtest/*.a /usr/lib
```

Now everything is ready for the build.
The release version may be built by executing the following in the project root:

```bash
$ mkdir cmake-build-release && cd cmake-build-release
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make
```

Optionally, you may launch the tests by executing

```bash
$ ./cli_test
```

Or, alternatively,

```bash
$ make test
```

The executable is now located in `cmake-build-release/cli`.
To build the debug version, just ommit the `-DCMAKE_BUILD_TYPE=Release` argument to `cmake`.

### Ubuntu 16.04

Some of C++17 features used in this project, such as `std::optional` or `std::filesystem`,
are not fully supported by GCC 5.3.1, which is installed by default in Ubuntu 16.04.
So you need to install the modern compiler first.
In order to do so, you need to execute the following commands instead of the very first command of the previous guide:

```bash
# apt remove cmake
# wget https://cmake.org/files/v3.10/cmake-3.10.0-Linux-x86_64.sh
# sudo sh cmake-3.10.0-Linux-x86_64.sh --prefix=/usr/local --exclude-subdir
# add-apt-repository ppa:ubuntu-toolchain-r/test
# apt update
# apt install g++ g++-8
```

That's it. After this, you may follow the installation instructions for Ubuntu 18.04+.
The only change you need to perform is to execute

```bash
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="/usr/bin/g++-8"
```

instead of normal `cmake` command.

### CLion

The project was developed by using the educational version of CLion and must be viewable and buildable from it.

## Architectural overview

### Build structure

Three targets are build by CMake:

* `liblcli.a` (target `lcli`) — a static library with nearly all of the code.

    Sources of the library are located in `lib/` directory.

* `cli` (target `cli`) — the command line emulator.
    
    Built from `main.cpp` and linked with `lcli`.
    
* `cli_test` (target `cli_test`) — an executable with unit tests.

    Sources of the tests are located in `test/` directory.
    The executable is also linked with `lcli`.
    
Note that the only action performed in `int main()` is a call to `NCli::RunMain`.
This solution was chosen in order to make the whole execution process (even from running the main function) testable.
This is demonstrated in the `ExampleTest::Example` test.

### Upper-level structure

It is not very surprising that the commands are first read, then the environmental variables used in the command are 
substitued by their values, then the command is parsed and then executed.
The whole action is performed in `NCli::RunMain` (`lib/cli.cpp`).
The following sections describes each of the processes.

### Reading

The important feature of `cli` is similar to `bash` support for single and double quotes.
A consequent feature is support for multiline commands as demonstrated by the following example:

```
cli > VAR=value
cli > echo '$VAR' "$VAR
> other value"
$VAR value
other value
cli > echo $VAR \
> "other value"
value other value
cli > echo $V\
> AR
value
```

This means that the lexical analysis of the command must be performed iteratively at the same time with reading.
There seems no other efficient way to determine whether the command is finished.

The lexical analysis splits the command into so-called "tokens"
(represented by class `NCli::TToken` defined in `lib/tokenize/token.h`).
A token is a sequence of "extended chars" (`NCli::TExtChar` defined in `lib/common/ext_char.h`).
The token idea is to represent a single "word" as understood by bash.
All of the escape symbols and quotes are parsed on this stage.

An extended char stores:

* The character it responds to.
* The "escape status" — whether the character was escaped by backslash or not.
* The "ignorance status" — whether the character is from single quotes (so some special meaning must be ignored) or not.

In fact, there is a special character type called "false delimiter".
It was added as a barrier symbol in parsing variable names and has special ignorance status.
It is added to a token instead of a quote of any kind.
This is demonstrated by the following example:

```
cli > A=value AB=other_value
cli > echo $A"B"
valueB
```

The whole lexical analysis is called "tokenization".
It is done by class `NCli::TTokenizer` defined in `lib/tokenize/tokenizer.h`.
It has a DFA under the hood (`NCli::TTokenizeDfa` defined in `lib/tokenize/tokenize_dfa.h`).
This abstract DFA is also reused in parsing the `$PATH` in executor system.

A subsequent feature of DFA is bash-like string joining:

```
cli > X=value
cli > VAR="$X"\ '$X'
cli > echo $VAR
value $X
```

### Environment variable substitution

On this stage, only global environment is used.

Here we need to parse the variable name in each token and substitute it by its value.
The global environment is represented by `NCli::TEnvironment` declared in `lib/environment/environment.h`.

The whole substitution process is performed in `NCli::TVarExpander` class defined in `lib/environment/var_expander.h`.
It also has a DFA under the hood. The DFA from tokenizer is not reused, though, due to the overall process simplicity.
Another reason is that the tokenizer DFA is designed for splitting an `std::string` into tokens, which is not the exact case.

The expander returns a sequence of tokens.

### Parsing

There are currently two bash-like syntax features supported:

* Variable assignments (global and local).
* Pipes (`|`).

Pipes are understood as separate tokens by the tokenizer.

This implies the following architectural solution for representing the command:

* The full command (`NCli::TFullCommand` from `lib/parser/command.h`) is a sequence of commands separated by pipe tokens.
* The command(`NCli::TCommand` from `lib/parser/command.h`) contains:
    * A sequence of assignments (`VAR=value` tokens represented by `NCli::TAssignment` declared in `lib/parser/command.h`).
    * The command to be executed. An empty command means a global variable assignment.
    * The command arguments (`argv` in C or C++) — just a sequence of tokens.

On this stage the `NCli::TToken` becomes redundant, so it is replaced by `std::string`.

### Execution

A command might be either external or built-in.
Each built in command has its own action.
This implies the following architectural solution.

The execution of a single command (`NCli::TCommand`) is performed by an instance of `NCli::IExecutor` declared in `lib/executor/executor.h`.
The executor for each command is selected by a factory method (`NCli::TExecutorFactory::MakeExecutor`).
This is the only place in the code where there is some interaction with concrete executors.

An executor accepts the global environment, an input stream and an output stream.
Its responsibilities are:

* Expand the global environment with local variable assignments (`NCli::TCmdEnvironment` from `lib/environment/environment.h`).
* Perform the actions, taking the input from the given input stream and saving its result to the given output stream.

The first stage is often redundant and skipped in the built-in commands.

The concrete executors are declared in `lib/executor/private` in namespace `NCli::NPrivate`.

Some commands are executed in separate processes (controlled by fork-exec technique).
For example, all external commands are executed separately.
Another example is `cat` built-in command, which may take some input from stdin, which may become corrupted after receiving an EOF.
The interaction with those processes is performed with pipes (`NCli::TPipe` from `lib/common/pipe.h`).
Another important note is that the behaviour of pipes should be different for standard input or input from output of another command.
When the input from another command is read and then fully redirected, we cannot afford to read the whole stdin.
As long as forked command has the same stdin, in case of reading commands from stdin, we just do nothing.
This logic is represented by `NCli::IIStreamWrapper` class from `lib/command/istream_wrapper.h` and its children.

The execution of a full command is performed by `NCli::Execute` declared in `lib/executor/execute.h`.
It accepts the same arguments as an executor.
It creates the needed number of streams and executes the commands subsequently.
