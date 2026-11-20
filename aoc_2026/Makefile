.DEFAULT_GOAL := build

PRESET ?= debug
DAY ?= 0
FILTER ?=
FORMAT_SOURCES := $(wildcard src/*.cpp src/aoc/*.cppm src/days/*.cppm)
LLVM_PREFIX := $(shell brew --prefix llvm 2>/dev/null)
CLANG_FORMAT ?= $(if $(LLVM_PREFIX),$(LLVM_PREFIX)/bin/clang-format,clang-format)

# Prefer Homebrew LLVM over Apple's compiler; allow CXX overrides.
ifeq ($(origin CXX),default)
ifneq ($(LLVM_PREFIX),)
CXX := $(LLVM_PREFIX)/bin/clang++
else
CXX := clang++
endif
endif

.PHONY: configure build clean rebuild run test format help

configure:
	CXX="$(CXX)" cmake --preset "$(PRESET)"

build: configure
	cmake --build --preset "$(PRESET)"

clean:
	@if [ -f "build/$(PRESET)/CMakeCache.txt" ]; then \
		cmake --build --preset "$(PRESET)" --target clean; \
	fi

rebuild: configure
	cmake --build --preset "$(PRESET)" --clean-first

run: build
	./build/$(PRESET)/aoc "$(DAY)"

test: build
	ctest --preset "$(PRESET)" -R "$(FILTER)"

format:
	"$(CLANG_FORMAT)" -i $(FORMAT_SOURCES)

help:
	@echo 'make                         Configure and build (debug with sanitizers)'
	@echo 'make run DAY=0               Build and run a day using its module input path'
	@echo 'make test                    Build and run all tests'
	@echo 'make test FILTER="Day 0"      Run tests matching a name pattern'
	@echo 'make test PRESET=release      Build and test in release mode'
	@echo 'make clean                   Remove compiled artifacts for the selected preset'
	@echo 'make rebuild                 Clean and build the selected preset'
	@echo 'make format                  Format C++ sources and modules'
	@echo 'PRESET=debug (default) or PRESET=release selects the build configuration.'
	@echo 'Override CXX to select a compiler on the first configure.'
