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
            std::print(stderr, " {}", day.number);
        }
        std::println(stderr, "");
    }
} // namespace

int main(int argc, char *argv[]) {
    std::print("{}", aoc::wreath);

    if (argc != 2) {
        std::println(stderr, "Usage: aoc <day>");
        return 1;
    }

    const std::string_view argument(argv[1]);
    int day = -1;
    const auto [end, error] =
        std::from_chars(argument.data(), argument.data() + argument.size(), day);

    if (error != std::errc{} || end != argument.data() + argument.size()) {
        std::println(stderr, "Invalid day: '{}'", argument);
        print_available_days();
        return 1;
    }

    for (const auto &entry : aoc::available_days()) {
        if (entry.number == day) {
            return entry.run();
        }
    }

    std::println(stderr, "Day {} is not implemented.", day);
    print_available_days();
    return 1;
}
