# Debugging in IntelliJ IDEA on macOS

[Back to the README](../README.md)

Working setup: IntelliJ IDEA 2026.2 on Apple Silicon, with Homebrew Clang and LLDB
23.1.6. Install the debugger alongside the build tools:

```sh
brew install llvm lldb cmake ninja
```

## Contents

- [Plugins](#plugins)
- [Toolchain and CMake profile](#toolchain-and-cmake-profile)
- [Debugger profile](#debugger-profile)
- [Run configuration](#run-configuration)

## Plugins

Enable JetBrains' **CLion C and C++**, **Native Build Tools**, and **CMake** plugins.
If Native Build Tools reports a duplicate `intellij.cidr.asm` module, disable
**Kotlin Multiplatform** and restart IntelliJ. That conflict also prevents the
dependent C++ and CMake plugins from loading.

## Toolchain and CMake profile

Open the `aoc_2026` directory as a CMake project, or load `aoc_2026/CMakeLists.txt` when the
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
UBSan stops at the first detected error when sanitizers are enabled. libc++ debug
hardening is enabled automatically for Debug configurations.
Run IDE and terminal builds sequentially when they use the same directory.

After changing compilers, use **Reset Cache and Reload Project** from the CMake
tool window. Configuration should identify **Clang 23.1.6**, not **AppleClang**.
An import-graph discovery error usually means CMake is still using the system
compiler (`/usr/bin/c++`); keep module scanning enabled and fix the compiler selection.

## Debugger profile

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

## Run configuration

Under **Run → Edit Configurations → + → CMake Application**, configure:

| Setting | Value |
|---|---|
| Name | `AoC Day 0` |
| Target / executable | `aoc` |
| CMake profile | The Debug profile using Homebrew LLVM |
| Program arguments | `0` |
| Working directory | The absolute path to `aoc_2026` |
| Before launch | Build |

Prepare `inputs/day00.txt` using the [warm-up instructions](../README.md#inputs-and-examples),
set a breakpoint inside `part1` in
`src/days/day00.cppm`, and click **Debug**. Change the program argument to run a
different day. To debug tests, select `aoc_tests` instead; its arguments are Catch2
filters such as `[day00]`.

Input paths are resolved from the configured `aoc_2026` source directory, so input
loading also works if the debugger or executable uses another working directory.
