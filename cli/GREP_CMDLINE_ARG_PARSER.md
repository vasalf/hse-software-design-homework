# Choosing a library for parsing command line arguments

This documents describes a process of choosing command line parsing library for `grep` built-in command.

## List of libraries

Googling leads to the following solutions:

* [C getopt + getopt_long](https://www.gnu.org/software/libc/manual/html_node/Getopt.html)
* [Boost.Program_options](https://www.boost.org/doc/libs/1_69_0/doc/html/program_options.html)
* [gflags](https://github.com/gflags/gflags)
* [CLI11](https://github.com/CLIUtils/CLI11) 
* [cxxopts](https://github.com/jarro2783/cxxopts)
* [TCLAP](http://tclap.sourceforge.net/)

## Features

The following criteria were used in the process of choosing library:

### Functionality

The technical task requires to parse named options and positional arguments, mixed in arbitrary order.
The formal task includes only one-letter named options, but it might become a serious problem in the project future in 
case of choosing library which supports only one-letter options.

Optionally, it might be useful if the selected library would be able to parse mixed one-letter named options, like

```
grep -iA 3
```

Any other functionality becomes a great advantage as it might be useful in future built-in commands.

### Isolation

Some non-detached built-in commands might re-use this dependency in future, so it is important for the command not to 
use any global variables.
Another edge of this requirement is that the arguments must be given to the library as some function or constructor
arguments, not parsed from any other place.

### Ease of use

The code describing the argument structure of `grep` command must be as easy as possible.
A good example of easy-to-use command line parser library is Python
[argparse](https://docs.python.org/3/library/argparse.html) module.

### Ease of build

The library is supposed to be included in the project sources (`../third_party`).

The framework should not create new build problems, as there already are some with GTest.
Ideally, this must be a header-only library, so that adding it leads only to adding a `-I` flag to compiler.

A static library built by CMake with no need in extra `Find*.cmake` files from any repo might be acceptable, but this is
a disadvantage.

## `getopt`

An example from [documentation](https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html):

```c
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main (int argc, char **argv)
{
  int aflag = 0;
  int bflag = 0;
  char *cvalue = NULL;
  int index;
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "abc:")) != -1)
    switch (c)
      {
      case 'a':
        aflag = 1;
        break;
      case 'b':
        bflag = 1;
        break;
      case 'c':
        cvalue = optarg;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }

  printf ("aflag = %d, bflag = %d, cvalue = %s\n",
          aflag, bflag, cvalue);

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
  return 0;
}
```

This solution seems to fully fulfil the "Functionality" and "Ease of build" criteria.
It fits perfectly in needed functionality as `/bin/grep` uses it and it is a part of POSIX standard so no additional
work is required to use it.

On the other hand, `getopt_long` is known to use global variables, so it is not isolated.
And its C interface might be quite easy to use for particular purpose, but it really is too ugly and needs a proper C++
wrapper.

## Boost.Program_options

An example from [documentation](https://www.boost.org/doc/libs/1_69_0/doc/html/program_options/tutorial.html#id-1.3.32.4.3):

```c++
// Declare the supported options.
po::options_description desc("Allowed options");
desc.add_options()
    ("help", "produce help message")
    ("compression", po::value<int>(), "set compression level")
;

po::variables_map vm;
po::store(po::parse_command_line(ac, av, desc), vm);
po::notify(vm);    

if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
}

if (vm.count("compression")) {
    cout << "Compression level was set to " 
 << vm["compression"].as<int>() << ".\n";
} else {
    cout << "Compression level was not set.\n";
}
```

This seems to fulfill the functionality and isolation requirements.
The code seems a bit creepy because a lot of `operator()` overloads, but at least it is better then getopt.

Nowadays separate boost libraries can be used with cloning [the github repo](https://github.com/boostorg/program_options)
so it is not obligatory to compile all of the boost.
But linking is required.

## gflags

An example from the [repo](https://github.com/gflags/example/blob/master/foo/main.cc)

```c++
#include <iostream>
#include "gflags/gflags.h"

DEFINE_bool(verbose, false, "Display program name before message");
DEFINE_string(message, "Hello world!", "Message to print");

static bool IsNonEmptyMessage(const char *flagname, const std::string &value)
{
  return value[0] != '\0';
}
DEFINE_validator(message, &IsNonEmptyMessage);

int main(int argc, char *argv[])
{
  gflags::SetUsageMessage("some usage message");
  gflags::SetVersionString("1.0.0");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_verbose) std::cout << gflags::ProgramInvocationShortName() << ": ";
  std::cout << FLAGS_message << std::endl;
  gflags::ShutDownCommandLineFlags();
  return 0;
}
```

It seems from examples that the flags totally fulfil the functionality requirements.

Isolation is the worst of all libraries: flags are defined inside the code and all flags from the same executable are linked
together. The most weird thing is that Google maintainers of the library believe that it increases flexibility.
The library does not seem to be usable in any way but creating a separate executable for every built-in command that
requires extended flags parsing.

Another important consequence is that code in the examples is awful, as the flags are defined in the global to use.
It is not very esthetically easy to use that library.

The library seems to be buildable with CMake but requires linking.

## CLI11

An example from [repo](https://github.com/CLIUtils/CLI11/blob/master/examples/simple.cpp):

```c++
#include "CLI/CLI.hpp"

int main(int argc, char **argv) {

    CLI::App app("K3Pi goofit fitter");

    std::string file;
    CLI::Option *opt = app.add_option("-f,--file,file", file, "File name");

    int count;
    CLI::Option *copt = app.add_option("-c,--count", count, "Counter");

    int v;
    CLI::Option *flag = app.add_flag("--flag", v, "Some flag that can be passed multiple times");

    double value; // = 3.14;
    app.add_option("-d,--double", value, "Some Value");

    CLI11_PARSE(app, argc, argv);

    std::cout << "Working on file: " << file << ", direct count: " << app.count("--file")
              << ", opt count: " << opt->count() << std::endl;
    std::cout << "Working on count: " << count << ", direct count: " << app.count("--count")
              << ", opt count: " << copt->count() << std::endl;
    std::cout << "Received flag: " << v << " (" << flag->count() << ") times\n";
    std::cout << "Some value: " << value << std::endl;

    return 0;
}
```

This library also claims to support all standard shell idioms. Additionally, it claims some features that might be
useful for other future built-in commands. For example, subcommands might be useful for `find`.

The flags seem to be stored in a single `CLI:App` instance, so the isolation requirement is satisfied.

It seems that creepy `CLI11_PARSE` may be replaced with `app.parse`.
There are some minor issues with architecture (for example, argument `"-f,--file,file"` is weird), but basically it is OK
and may be used.

The library is header-only, with only one header used, so it fully satisfies the "ease to build" requirement.

## cxxopt

According to its README, the positional arguments are only parsed after `--`.
This is not the desired functionality, so no additional review is done here.

## TCLAP

An example from the [documentation](http://tclap.sourceforge.net/manual.html):

(I deleted comments in order to save some space)
```c++
#include <string>
#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>

int main(int argc, char** argv)
{

	try {  
	TCLAP::CmdLine cmd("Command description message", ' ', "0.9");

	TCLAP::ValueArg<std::string> nameArg("n","name","Name to print",true,"homer","string");
	cmd.add( nameArg );

	TCLAP::SwitchArg reverseSwitch("r","reverse","Print name backwards", cmd, false);
	cmd.parse( argc, argv );

	std::string name = nameArg.getValue();
	bool reverseName = reverseSwitch.getValue();

	if ( reverseName )
	{
		std::reverse(name.begin(),name.end());
		std::cout << "My name (spelled backwards) is: " << name << std::endl;
	}
	else
		std::cout << "My name is: " << name << std::endl;


	} catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}
```

This library seems to be functionally OK.
And the isolation requirement is also satisfied.

The provided interface is most nice of all of those libraries.
It doesn't have neither ugly defines nor creepy operator overloads.

And the library is header-only, so it totally satisfies the last requirement.

## Combining the results

Everything is demonstrated in the following table:

| Library               | Functionality     | Isolation | Ease of use      | Ease of build       |
| ---                   | :---:             | :---:     | :---:            | :---:               |
| getopt                | **OK**            | No        | Ugly C-style     | **POSIX standard**  |
| Boost.Program_options | **OK**            | **OK**    | **Basically OK** | Static library      |
| gflags                | **OK**            | No        | Ugly defines     | Static library      |
| CLI11                 | **OK + features** | **OK**    | **Basically OK** | **Header-only**     |
| TCLAP                 | **OK**            | **OK**    | **OK**           | **Header-only**     |

There is a difficult choice between CLI11 and TCLAP, all other libraries are worse by at least one requirement.

# Final solution

The final solution is to use CLI11.
The reason for this solution is some features of CLI11 that are not supported by TCLAP, such as subcommands.