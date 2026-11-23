# Advent of Code 2026

C++23 solutions using named modules, Catch2 v3, and lexy.

## Contents

- [Setup](#setup)
- [Build and run](#build-and-run)
- [Add a day](#add-a-day)
- [Testing](#testing)
- [Inputs and examples](#inputs-and-examples)
- [Formatting](#formatting)
- [Project structure](#project-structure)
  - [Solution contract](#solution-contract)
  - [Day discovery](#day-discovery)
  - [Shared modules](#shared-modules)

## Setup

Install upstream LLVM (including `clang-scan-deps` and `clang-format`), CMake, and Ninja:

```sh
brew install llvm cmake ninja
cd aoc_2026
make test
```

CMake 3.28+ and Ninja 1.11+ are required. Use upstream LLVM rather than Apple Clang for
the module dependency scanner. The first configure downloads pinned versions of
Catch2 and lexy; subsequent builds reuse them. The compiler is cached at configure time;
use a fresh build directory when changing toolchains.

The Makefile selects Homebrew LLVM automatically. Override `CXX` if needed, for
example `make test CXX=/path/to/clang++`.

Verified locally with LLVM/Clang 23.1.6, CMake 4.5.1, and Ninja 1.13.2 on Apple Silicon.

On other platforms, select a compiler supported by CMake's module scanning with a
C++23 standard library. The macOS commands above are the primary development setup.

See the [IntelliJ setup and debugging guide](docs/intellij.md) for plugins,
toolchains, shared build directories, and debugger configuration on macOS.

## Build and run

Run commands from the `aoc_2026` directory:

```sh
make build
make run DAY=0
```

Plain `make` also configures and builds. Both `make run` and `make test` build
first. Run `make help` for the shortcuts. `DAY` defaults to `0`; unknown days list
the available day numbers. For the initial day 0 input, follow the
[warm-up instructions](#inputs-and-examples).

For a practice module from another year, include the year in the day selector:

```sh
make run DAY=04_2020
make test FILTER=day04_2020
```

Each part reports its execution time, excluding input loading and parsing.

Select `PRESET=debug` (the default) or `PRESET=release` consistently for building,
testing, and running. Debug enables AddressSanitizer and UndefinedBehaviorSanitizer,
with UBSan stopping at the first detected error. Debug configurations also enable
libc++ debug hardening to check standard-library preconditions, such as accessing
an empty container. These library checks apply when using libc++, as in the macOS
LLVM setup. Release uses optimizations and the library's default hardening level,
without sanitizers. Compiler warnings are enabled for both presets.

```sh
make build PRESET=release
make test PRESET=release
make run PRESET=release DAY=0
```

To remove compiled artifacts or perform a clean build:

```sh
make clean
make rebuild
make rebuild PRESET=release
```

These commands affect only the selected preset under `build/`. They keep CMake's
cache, downloaded dependencies, and configure-generated files such as the registry
module. With IntelliJ configured to use the same directory, cleaning also affects
its debug executable. A pre-existing `cmake-build-debug` directory is no longer used
after switching the IDE profile's build directory.

## Add a day

Generate a module and empty example and input files:

```sh
make new DAY=1
```

This creates `src/days/day01.cppm`, `examples/day01.txt`, and `inputs/day01.txt`,
with the module name, namespace, paths, and four answer tests filled in. Days from 1 to 25 are accepted;
zero-padded values such as `DAY=08` work too. Day 0 remains the warm-up. The command
refuses to overwrite any existing module, example, or input file. Personal inputs
remain ignored by Git.

Modules follow the two-digit naming convention: `dayXX.cppm`,
`export module aoc.dayXX`, and namespace `aoc::dayXX`. CMake discovers new days
automatically; no edits to `CMakeLists.txt` or `main.cpp` are needed.

1. Fill in the input representation, lexy grammar, and static `parse`, `part1`, and
   `part2` methods. The scaffold initially keeps the input lines and returns zero for
   both parts; replace these placeholders with the puzzle implementation.
2. Fill the generated [example and input files](#inputs-and-examples).
3. Set each test's expected answer and remove its `SKIP` once that answer is known.
4. Follow [Testing](#testing) to check the day, then run it with `make run DAY=1`.

See the [solution contract](#solution-contract) for the required signatures.

## Testing

Build and run all tests, or filter by day:

```sh
make test
make test FILTER='day01'         # Filter by test name (regular expression)
make test PRESET=release
./build/debug/aoc_tests '[day01]' # Catch2 tag filter; build first with make test
```

Each generated day has four answer tests: part 1 example, part 1 personal input,
part 2 example, and part 2 personal input. They initially report as skipped; set
the expected answers and remove `SKIP` as answers become known. Add parser edge
cases alongside them as useful.

Tests live alongside each solution under `#ifdef AOC_TESTING`; no separate test
file is needed. The scaffold supplies the conditional includes and imports.

The shared validation helper binds the day's parser once:

```cpp
// Inside the day's namespace and AOC_TESTING block:
const auto validate = aoc::test::Validator(Solution::parse);

TEST_CASE("day00 part1 example", "[day00]") {
    validate(Solution::part1).with(Solution::example_path).equals(60);
}
```

Paths are resolved from the project directory, including when CTest runs from a
build directory. `.with(path)` requires the file; `.with_optional(path)` skips the
test if a personal input file is absent. Files that exist but cannot be read or
parsed still fail. Personal-input tests carry the `[input]` tag.

Day 0 uses the invented warm-up input `10,20,30` for both sets of answer checks
(60 and 6,000). To run its personal-input checks, put that content in `inputs/day00.txt`;
if you use different content, update the expected answers accordingly.

## Inputs and examples

Each day's `Solution` struct defines `input_path` and `example_path`. The runner
reads `input_path`, and the tests use the same constants. Relative paths are
resolved from the configured `aoc_2026` source directory, regardless of the
process's working directory. Absolute paths are used as supplied.

Keep personal puzzle inputs under `inputs/`, which is ignored by Git. Small
invented examples can live in `examples/` or directly in tests.

Both the runner and validator use `aoc.io` for path resolution and reading.
`read_input` accepts `std::filesystem::path` and returns
`std::expected<std::vector<std::string>, std::string>`. Each string owns one line;
LF and CRLF line endings are removed, while blank lines and other whitespace are
preserved. An empty file produces no lines, and a final newline does not add an
extra line. Each day's `parse(std::span<const std::string>)` receives a read-only
view of these lines and produces its puzzle-specific input type.

`resolve_input_path` anchors relative paths to the source directory recorded by CMake. The source tree and its
input files must still be present when running a built executable.

Day 0 is an invented warm-up, not an Advent of Code puzzle. It sums comma-separated
nonnegative integers on a single line for part 1 and multiplies them for part 2. Prepare its input once:

```sh
mkdir -p inputs
cp examples/day00.txt inputs/day00.txt
make run DAY=0
```

Expected answers: **60** and **6,000**.

## Formatting

Format all project C++ sources and modules using `.clang-format`:

```sh
make format
```

The Makefile selects Homebrew's `clang-format` when available, otherwise it uses
`clang-format` from `PATH`. Override it with `CLANG_FORMAT=/path/to/clang-format`.
To check formatting without changing files:

```sh
"$(brew --prefix llvm)/bin/clang-format" --dry-run --Werror src/main.cpp src/aoc/*.cppm src/days/*.cppm
```

Compiler warnings are enabled for project targets; third-party headers are treated
as system headers.

## Project structure

```text
aoc_2026/
├── CMakeLists.txt
├── CMakePresets.json       # Debug/release build and test commands
├── Makefile                # Shortcuts for configuring, building, running, and testing
├── .clang-format
├── cmake/
│   ├── day.cppm.in         # New-day module and four answer tests
│   ├── days.cmake          # Day discovery, contract checks, and registry generation
│   ├── dependencies.cmake # Pinned lexy and optional Catch2 dependencies
│   ├── new_day.cmake       # Validates the day and creates the scaffold
│   ├── options.cmake       # Warnings, sanitizers, and source-directory definition
│   ├── testing.cmake       # Test executable and Catch2/CTest registration
│   └── day_registry.cppm.in # Template for the generated registry module
├── docs/intellij.md        # IntelliJ setup and debugging on macOS
├── examples/day00.txt      # Invented warm-up input
└── src/
    ├── main.cpp            # CLI parsing and day selection
    ├── aoc/
    │   ├── io.cppm         # aoc.io: input loading via std::expected
    │   ├── runner.cppm     # aoc.runner: parsing, timing, errors, and output
    │   ├── test.cppm       # aoc.test: shared answer validation (test build only)
    │   └── wreath.cppm     # aoc.wreath: Christmas banner
    └── days/day00.cppm     # Paths, parsing, both parts, and tests
```

The root `CMakeLists.txt` defines the language settings, shared module list, and
main library/executable targets. It loads the build options, dependencies, and day
discovery in sequence from `cmake/`, then loads the testing setup when
`BUILD_TESTING` is enabled. Catch2 is only fetched for test-enabled builds.

### Solution contract

Each namespace exports a `Solution` struct. The generated registry checks its
interface at compile time:

```cpp
static_assert(aoc::DayInterface<aoc::day00::Solution>);
```

- The input path must be convertible to `std::string_view`.
- Static `parse(std::span<const std::string>)` must return `std::expected<T, std::string>`.
- Static `part1` and `part2` must accept `const T&` and return printable answers.

`aoc::ParsedInput<Solution>` infers `T` from the parser's return type. It can be a
container, a custom grid or graph, or any other parsed value type. An `Input` alias
is optional; day 0 keeps one inside its namespace for readability. Helpers and the
lexy grammar can also stay in that namespace without being exported.

The solvers may return different printable types, such as an integer for part 1
and a string for part 2. Missing members or incompatible signatures fail the build.
No inheritance or manual registration is needed.

### Day discovery

CMake generates the `aoc.registry` module's imports and dispatch table in
`build/<preset>/generated/day_registry.cppm`.
Day discovery and generation live in `cmake/days.cmake`; the root build file calls
`aoc_discover_days(DAY_MODULES DAY_REGISTRY)` to obtain the module list and registry path.
The runner loads the selected module's input, parses it once, and runs both parts.
No edits to `CMakeLists.txt` or `main.cpp` are needed for new days. For example,
adding `day01.cppm` makes `make run DAY=1` available. Practice modules can use
`dayXX_YYYY.cppm`, with matching module and namespace names, such as
`aoc.day04_2020` and `aoc::day04_2020`. They are registered separately from the
current year's day and selected with `DAY=04_2020`. Use distinct input/example
paths and test names/tags for each practice module. `make new` scaffolds current-year
days; year-suffixed practice modules are added manually. Unknown days list the available
day numbers. Only put day modules ready to compile under this filename pattern.

### Shared modules

Project code uses standard named modules (`export module aoc.day00;` and
`import aoc.day00;`). Standard-library and dependency headers are included normally;
`import std;` is not required. lexy stays in the module's global fragment so its
templates do not become part of our module's public API. All shared C++ helpers
are modules; `main.cpp` remains a normal translation unit containing the entry
point and imports the generated `aoc.registry` module.

When adding a helper used by both solutions and tests, add its module once to
`AOC_COMMON_MODULES` in `CMakeLists.txt`. Runner-only helpers are listed on
`aoc_days`; the Catch2-dependent `aoc.test` module stays on the test target only.

Tests live in each module under `#ifdef AOC_TESTING`. CMake compiles the modules
separately for the runner and the test executable, with distinct compiled module
artifacts. Only the test build enables Catch2 and the `aoc.test` validation module.
Keep the conditional Catch2 includes in the global module fragment (before
`export module`), and the conditional `import aoc.test;` after the module
declaration. Catch2 macros still need a header include because named module
imports do not expose macros. Copying day 0 preserves this setup; no separate
per-day test file is needed. The scaffold preserves this layout automatically.
