module;

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>

#ifdef AOC_TESTING
#include <initializer_list>

#include <catch2/catch_test_macros.hpp>
#endif

export module aoc.day00;

#ifdef AOC_TESTING
import aoc.test;
#endif

namespace {
    namespace dsl = lexy::dsl;

    // Warm-up grammar: one or more nonnegative integers separated by commas.
    struct numbers {
        static constexpr auto whitespace = dsl::ascii::space;
        static constexpr auto rule = dsl::list(dsl::integer<int>, dsl::sep(dsl::comma)) + dsl::eof;
        static constexpr auto value = lexy::as_list<std::vector<int>>;
    };
} // namespace

export namespace aoc::day00 {
    inline constexpr auto input_path = "inputs/day00.txt";
    inline constexpr auto example_path = "examples/day00.txt";

    using Input = std::vector<int>;

    std::expected<Input, std::string> parse(std::string_view text) {
        const lexy::string_input input(text.data(), text.size());
        auto result = lexy::parse<numbers>(input, lexy::noop);
        if (!result) {
            return std::unexpected(
                "Expected comma-separated nonnegative integers in the int range");
        }
        return std::move(result).value();
    }

    std::int64_t part1(const Input &input) {
        std::int64_t sum = 0;
        for (const auto number : input) {
            sum += number;
        }
        return sum;
    }

    std::size_t part2(const Input &input) { return input.size(); }
} // namespace aoc::day00

#ifdef AOC_TESTING
namespace aoc::day00 {
    namespace {
        const auto validate = aoc::test::Validator(parse);
    } // namespace

    TEST_CASE("day00 part1 example", "[day00]") { validate(part1).with(example_path).equals(60); }

    TEST_CASE("day00 part1", "[day00][input]") {
        validate(part1).with_optional(input_path).equals(60);
    }

    TEST_CASE("day00 part2 example", "[day00]") { validate(part2).with(example_path).equals(3U); }

    TEST_CASE("day00 part2", "[day00][input]") {
        validate(part2).with_optional(input_path).equals(3U);
    }

    TEST_CASE("Day 0 parses comma-separated integers", "[day00]") {
        auto input = parse("10,20,30\n");
        REQUIRE(input.has_value());
        CHECK(*input == std::vector<int>{10, 20, 30});
    }

    TEST_CASE("Day 0 accepts whitespace and zero", "[day00]") {
        auto input = parse(" 0, 2,\r\n 3 \n");
        REQUIRE(input.has_value());
        CHECK(*input == std::vector<int>{0, 2, 3});
    }

    TEST_CASE("Day 0 rejects malformed input", "[day00]") {
        for (const auto *text :
             {"", " ", "1,", "1,,2", "1 2", "-1", "1,x", "1,2junk", "9999999999999999999999999"}) {
            CAPTURE(text);
            auto input = parse(text);
            REQUIRE_FALSE(input.has_value());
            CHECK_FALSE(input.error().empty());
        }
    }
} // namespace aoc::day00
#endif
