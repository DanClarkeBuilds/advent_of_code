module;

#include <chrono>
#include <concepts>
#include <cstdio>
#include <expected>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <utility>

export module aoc.runner;

import aoc.io;

export namespace aoc {
    template <typename Day>
    using ParsedInput = typename decltype(Day::parse(std::declval<std::string_view>()))::value_type;

    template <typename Day>
    concept DayInterface = requires(std::string_view text, const ParsedInput<Day> &input) {
        { Day::input_path } -> std::convertible_to<std::string_view>;
        { Day::example_path } -> std::convertible_to<std::string_view>;
        { Day::parse(text) } -> std::same_as<std::expected<ParsedInput<Day>, std::string>>;
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

    template <DayInterface Day> int run_day(int day) {
        const std::string_view path = Day::input_path;
        auto text = read_input(std::string(path));
        if (!text) {
            std::println(stderr, "{}", text.error());
            return 1;
        }

        auto input = Day::parse(*text);
        if (!input) {
            std::println(stderr, "{}: {}", path, input.error());
            return 1;
        }

        std::println("Advent of Code 2026 - Day {}", day);
        run_part(1, [&] { return Day::part1(*input); });
        run_part(2, [&] { return Day::part2(*input); });
        return 0;
    }
} // namespace aoc
