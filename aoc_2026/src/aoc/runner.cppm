module;

#include <chrono>
#include <concepts>
#include <cstdio>
#include <expected>
#include <format>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <utility>

export module aoc.runner;

import aoc.io;

export namespace aoc {
    template <typename Day>
    using ParsedInput =
        decltype(Day::parse(std::declval<std::span<const std::string>>()))::value_type;

    template <typename Day>
    concept DayInterface =
        requires(std::span<const std::string> lines, const ParsedInput<Day> &input) {
            { Day::input_path } -> std::convertible_to<std::string_view>;
            { Day::parse(lines) } -> std::same_as<std::expected<ParsedInput<Day>, std::string>>;
            { Day::part1(input) } -> std::formattable<char>;
            { Day::part2(input) } -> std::formattable<char>;
        };

    template <typename Function> void run_part(int part, Function solve) {
        const auto start = std::chrono::steady_clock::now();
        const auto answer = solve();
        const auto elapsed =
            std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - start);
        std::println("Part {}: {} ({:.1f} us)", part, answer, elapsed.count());
    }

    template <DayInterface Day> int run_day(int day, int year = 2026) {
        const std::string_view path = Day::input_path;
        auto lines = read_input(path);
        if (!lines) {
            std::println(stderr, "{}", lines.error());
            return 1;
        }

        auto input = Day::parse(*lines);
        if (!input) {
            std::println(stderr, "{}: {}", path, input.error());
            return 1;
        }

        std::println("Advent of Code {} - Day {}", year, day);
        run_part(1, [&] { return Day::part1(*input); });
        run_part(2, [&] { return Day::part2(*input); });
        return 0;
    }
} // namespace aoc
