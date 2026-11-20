module;

#include <chrono>
#include <cstdio>
#include <expected>
#include <print>
#include <string>

export module aoc.runner;

import aoc.io;

export namespace aoc {
    template <typename Function> void run_part(int part, Function solve) {
        const auto start = std::chrono::steady_clock::now();
        const auto answer = solve();
        const auto elapsed =
            std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start);
        std::println("Part {}: {} ({:.1f} us)", part, answer, elapsed.count());
    }

    template <typename Parser, typename Part1, typename Part2>
    int run_day(int day, const std::string &path, Parser parse, Part1 part1, Part2 part2) {
        auto text = read_input(path);
        if (!text) {
            std::println(stderr, "{}", text.error());
            return 1;
        }

        auto input = parse(*text);
        if (!input) {
            std::println(stderr, "{}: {}", path, input.error());
            return 1;
        }

        std::println("Advent of Code 2026 - Day {}", day);
        run_part(1, [&] { return part1(*input); });
        run_part(2, [&] { return part2(*input); });
        return 0;
    }
} // namespace aoc
