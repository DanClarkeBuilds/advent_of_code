#include <charconv>
#include <cstdio>
#include <print>
#include <string_view>
#include <system_error>

import aoc.registry;
import aoc.wreath;

namespace {
    void print_available_days() {
        std::print(stderr, "Available days:");
        for (const auto &day : aoc::available_days()) {
            if (day.year == 2026) {
                std::print(stderr, " {}", day.number);
            } else {
                std::print(stderr, " {:02}_{}", day.number, day.year);
            }
        }
        std::println(stderr, "");
    }
} // namespace

int main(int argc, char *argv[]) {
    std::print("{}", aoc::wreath);

    if (argc != 2) {
        std::println(stderr, "Usage: aoc <day> (for practice: <day>_<year>)");
        return 1;
    }

    const std::string_view argument(argv[1]);
    const auto separator = argument.find('_');
    const auto day_argument = argument.substr(0, separator);
    const auto year_argument = separator == std::string_view::npos ? std::string_view("2026")
                                                                   : argument.substr(separator + 1);
    int day = -1;
    int year = 2026;
    const auto [end, error] =
        std::from_chars(day_argument.data(), day_argument.data() + day_argument.size(), day);
    const auto [year_end, year_error] =
        std::from_chars(year_argument.data(), year_argument.data() + year_argument.size(), year);

    if (error != std::errc{} || end != day_argument.data() + day_argument.size() ||
        year_error != std::errc{} || year_end != year_argument.data() + year_argument.size() ||
        year_argument.size() != 4) {
        std::println(stderr, "Invalid day: '{}'", argument);
        print_available_days();
        return 1;
    }

    for (const auto &entry : aoc::available_days()) {
        if (entry.number == day && entry.year == year) {
            return entry.run();
        }
    }

    std::println(stderr, "Day {} of {} is not implemented.", day, year);
    print_available_days();
    return 1;
}
