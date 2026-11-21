# Advent of Code 2026

C++23 solutions using named modules, Catch2 v3, and lexy.

## Setup on macOS

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
example `make test CXX=/path/to/clang++`. Plain `make` configures and builds;
`make run` and `make test` also build first. Run `make help` for the shortcuts.

Verified locally with LLVM/Clang 23.1.6, CMake 4.5.1, and Ninja 1.13.2 on Apple Silicon.

The debug preset enables AddressSanitizer, UndefinedBehaviorSanitizer, and compiler
warnings. The release preset enables optimized builds without sanitizers:

```sh
make test PRESET=release
```

On other platforms, select a compiler supported by CMake's module scanning with a
C++23 standard library. The macOS commands above are the primary development setup.

## Debugging in IntelliJ IDEA on macOS

Working setup: IntelliJ IDEA 2026.2 on Apple Silicon, with Homebrew Clang and LLDB
23.1.6. Install the debugger alongside the build tools:

```sh
brew install llvm lldb cmake ninja
```

### Plugins

Enable JetBrains' **CLion C and C++**, **Native Build Tools**, and **CMake** plugins.
If Native Build Tools reports a duplicate `intellij.cidr.asm` module, disable
**Kotlin Multiplatform** and restart IntelliJ. That conflict also prevents the
dependent C++ and CMake plugins from loading.

### Toolchain and CMake profile

Open this directory as a CMake project, or load `aoc_2026/CMakeLists.txt` when the
whole repository is open. Under **Settings → Build, Execution, Deployment →
Toolchains**, create a **System** toolchain named `Homebrew LLVM`:

| Setting | Path |
|---|---|
| CMake | `/opt/homebrew/bin/cmake` |
| Build tool | `/opt/homebrew/bin/ninja` |
| C compiler | `/opt/homebrew/opt/llvm/bin/clang` |
| C++ compiler | `/opt/homebrew/opt/llvm/bin/clang++` |

These paths assume Apple Silicon Homebrew. Use `brew --prefix` and
`brew --prefix llvm` to find the corresponding paths on another installation.

Under **Build, Execution, Deployment → CMake**, select the Debug profile you will
use and assign it the `Homebrew LLVM` toolchain. Creating a toolchain does not
automatically assign it to existing profiles. Use **Ninja** and build type **Debug**.
Set **Build directory** to `build/debug` relative to the `aoc_2026` CMake project
directory, or use its absolute path. This shares the build directory with
`make build` and `make test` instead of creating a separate `cmake-build-debug`.
The repository's `debug` preset also enables sanitizers; imported presets may need
to be enabled first. If its settings are read-only, make `Homebrew LLVM` the default
toolchain or edit a copy of the preset's profile.

When sharing `build/debug` through a manually configured profile, add
`-DAOC_ENABLE_SANITIZERS=ON` to its **CMake options** to match the debug preset.
Run IDE and terminal builds sequentially when they use the same directory.

After changing compilers, use **Reset Cache and Reload Project** from the CMake
tool window. Configuration should identify **Clang 23.1.6**, not **AppleClang**.
An import-graph discovery error usually means CMake is still using the system
compiler (`/usr/bin/c++`); keep module scanning enabled and fix the compiler selection.

### Debugger profile

Under **Build, Execution, Deployment → Debugger → Debug Profiles**, create an
**LLDB** profile with **Executable** set to:

```text
/opt/homebrew/opt/lldb/bin/lldb
```

Select this debug profile in the toolbar. It is separate from the CMake profile
(how to build) and the run configuration (what to execute).

Use LLDB matching the Homebrew LLVM version. Xcode's older LLDB could step through
this project but failed to display LLVM 23's `std::vector` layout, reporting
`Failed to determine start/end of vector data`. Homebrew LLDB 23.1.6 correctly
displayed all three warm-up values: `10`, `20`, and `30`.

### Run configuration

Under **Run → Edit Configurations → + → CMake Application**, configure:

| Setting | Value |
|---|---|
| Name | `AoC Day 0` |
| Target / executable | `aoc` |
| CMake profile | The Debug profile using Homebrew LLVM |
| Program arguments | `0` |
| Working directory | The absolute path to `aoc_2026` |
| Before launch | Build |

Prepare `inputs/day00.txt` as shown below, set a breakpoint inside `part1` in
`src/days/day00.cppm`, and click **Debug**. Change the program argument to run a
different day. To debug tests, select `aoc_tests` instead; its arguments are Catch2
filters such as `[day00]`.

Input paths are resolved from the configured `aoc_2026` source directory, so input
loading also works if the debugger or executable uses another working directory.

## Run a day

Run commands from this directory. Day 0 is a warm-up, not an Advent of Code puzzle:
it sums comma-separated nonnegative integers for part 1 and counts them for part 2.

```sh
# One-time setup for the invented warm-up input:
mkdir -p inputs
cp examples/day00.txt inputs/day00.txt
make run DAY=0
```

Expected answers: **60** and **3**. Each part reports its execution time, excluding
input loading and parsing.

Each day's module exports a `Solution` struct containing `input_path` and
`example_path`. The runner uses `Solution::input_path`, and the tests use the same
constants. Day 0 reads
`inputs/day00.txt` relative to this project's source directory, regardless of the
process's working directory. Absolute paths are used as supplied. Keep personal
puzzle inputs under `inputs/`, which is ignored
by Git. Small invented examples can live in `examples/` or directly in tests.

`DAY` defaults to `0`. Select `PRESET=debug` (the default) or `PRESET=release`
consistently for building, testing, and running:

```sh
make build PRESET=release
make test PRESET=release
make run PRESET=release DAY=0
```

Both `test` and `run` configure and build automatically. Debug includes sanitizers;
release uses optimizations.

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

## Structure

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
├── examples/day00.txt      # Invented warm-up input
└── src/
    ├── main.cpp            # CLI parsing and day selection
    ├── aoc/
    │   ├── io.cppm         # aoc.io: input loading via std::expected
    │   ├── runner.cppm     # aoc.runner: parsing, timing, errors, and output
    │   └── test.cppm       # aoc.test: shared answer validation (test build only)
    └── days/day00.cppm     # Paths, parsing, both parts, and tests
```

Project code uses standard named modules (`export module aoc.day00;` and
`import aoc.day00;`). Standard-library and dependency headers are included normally;
`import std;` is not required. lexy stays in the module's global fragment so its
templates do not become part of our module's public API. All shared C++ helpers
are modules; `main.cpp` remains a normal translation unit containing the entry
point and imports the generated `aoc.registry` module.

The root `CMakeLists.txt` defines the language settings, shared module list, and
main library/executable targets. It loads the build options, dependencies, and day
discovery in sequence from `cmake/`, then loads the testing setup when
`BUILD_TESTING` is enabled. Catch2 is only fetched for test-enabled builds.

## Add a day

Generate a module and empty example and input files:

```sh
make new DAY=1
```

This creates `src/days/day01.cppm`, `examples/day01.txt`, and `inputs/day01.txt`, with the module name,
namespace, paths, and four answer tests filled in. Days from 1 to 25 are accepted;
zero-padded values such as `DAY=08` work too. Day 0 remains the warm-up. The command
refuses to overwrite any existing module, example, or input file. Personal inputs
remain ignored by Git.

1. Fill in the input representation, lexy grammar, and static `parse`, `part1`, and
   `part2` methods. The scaffold initially keeps the raw text and returns zero for
   both parts; replace these placeholders with the puzzle implementation.
2. Paste the example into `examples/day01.txt` and your input into `inputs/day01.txt`.
3. Set each test's expected answer and remove its `SKIP` once that answer is known.
   All four generated tests initially report as skipped, rather than validating
   placeholder answers. Add parser edge cases alongside them as useful.
4. Build and run the tests:

```sh
make test
make test FILTER='day01'       # Filter by test name (regular expression)
./build/debug/aoc_tests '[day01]' # Filter by day
```

CMake automatically discovers day modules, including added or removed files on
the next build. Modules must follow the two-digit naming
convention: `dayXX.cppm`, `export module aoc.dayXX`, and namespace `aoc::dayXX`.
Each namespace exports a `Solution` struct. The generated registry checks its
interface at compile time:

```cpp
static_assert(aoc::DayInterface<aoc::day00::Solution>);
```

- Both paths must be convertible to `std::string_view`.
- Static `parse(std::string_view)` must return `std::expected<T, std::string>`.
- Static `part1` and `part2` must accept `const T&` and return printable answers.

`aoc::ParsedInput<Solution>` infers `T` from the parser's return type. It can be a
container, a custom grid or graph, or any other parsed value type. An `Input` alias
is optional; day 0 keeps one inside its namespace for readability. Helpers and the
lexy grammar can also stay in that namespace without being exported.

The solvers may return different printable types, such as an integer for part 1
and a string for part 2. Missing members or incompatible signatures fail the build.
No inheritance or manual registration is needed.

CMake generates the `aoc.registry` module's imports and dispatch table in
`build/<preset>/generated/day_registry.cppm`.
Day discovery and generation live in `cmake/days.cmake`; the root build file calls
`aoc_discover_days(DAY_MODULES DAY_REGISTRY)` to obtain the module list and registry path.
The runner loads the selected module's input, parses it once, and runs both parts.
No edits to `CMakeLists.txt` or `main.cpp` are needed for new days. For example,
adding `day01.cppm` makes `make run DAY=1` available. Unknown days list the available
day numbers. Only put day modules ready to compile under this filename pattern.

Tests live in each module under `#ifdef AOC_TESTING`. CMake compiles the modules
separately for the runner and the test executable, with distinct compiled module
artifacts. Only the test build enables Catch2 and the `aoc.test` validation module.
Keep the conditional Catch2 includes in the global module fragment (before
`export module`), and the conditional `import aoc.test;` after the module
declaration. Catch2 macros still need a header include because named module
imports do not expose macros. Copying day 0 preserves this setup; no separate
per-day test file is needed. The scaffold preserves this layout automatically.

When adding a helper used by both solutions and tests, add its module once to
`AOC_COMMON_MODULES` in `CMakeLists.txt`. Runner-only helpers are listed on
`aoc_days`; the Catch2-dependent `aoc.test` module stays on the test target only.

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

Both the runner and validator use `aoc.io` for path resolution and reading.
`read_input` accepts `std::filesystem::path`, and `resolve_input_path` anchors
relative paths to the source directory recorded by CMake. The source tree and its
input files must still be present when running a built executable.

Day 0 uses the invented warm-up input `10,20,30` for both sets of answer checks
(60 and 3). To run its personal-input checks, put that content in `inputs/day00.txt`;
if you use different content, update the expected answers accordingly.

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
