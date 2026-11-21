module;

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

namespace aoc::day00 {
    using Input = std::vector<int>;

    namespace {
        namespace dsl = lexy::dsl;

        // Warm-up grammar: one or more nonnegative integers separated by commas.
        struct numbers {
            static constexpr auto whitespace = dsl::ascii::space;
            static constexpr auto rule =
                dsl::list(dsl::integer<int>, dsl::sep(dsl::comma)) + dsl::eof;
            static constexpr auto value = lexy::as_list<Input>;
        };
    } // namespace

    export struct Solution {
        static constexpr auto input_path = "inputs/day00.txt";
        static constexpr auto example_path = "examples/day00.txt";

        static std::expected<Input, std::string> parse(std::string_view text) {
            const lexy::string_input input(text.data(), text.size());
            auto result = lexy::parse<numbers>(input, lexy::noop);
            if (!result) {
                return std::unexpected(
                    "Expected comma-separated nonnegative integers in the int range");
            }
            return std::move(result).value();
        }

        static std::int64_t part1(const Input &input) {
            std::int64_t sum = 0;
            for (const auto number : input) {
                sum += number;
            }
            return sum;
        }

        static std::size_t part2(const Input &input) { return input.size(); }
    };

#ifdef AOC_TESTING
    namespace {
        const auto validate = aoc::test::Validator(Solution::parse);
    } // namespace

    TEST_CASE("day00 part1 example", "[day00]") {
        validate(Solution::part1).with(Solution::example_path).equals(60);
    }

    TEST_CASE("day00 part1", "[day00][input]") {
        validate(Solution::part1).with_optional(Solution::input_path).equals(60);
    }

    TEST_CASE("day00 part2 example", "[day00]") {
        validate(Solution::part2).with(Solution::example_path).equals(3U);
    }

    TEST_CASE("day00 part2", "[day00][input]") {
        validate(Solution::part2).with_optional(Solution::input_path).equals(3U);
    }

    TEST_CASE("Day 0 parses comma-separated integers", "[day00]") {
        auto input = Solution::parse("10,20,30\n");
        REQUIRE(input.has_value());
        CHECK(*input == std::vector<int>{10, 20, 30});
    }

    TEST_CASE("Day 0 accepts whitespace and zero", "[day00]") {
        auto input = Solution::parse(" 0, 2,\r\n 3 \n");
        REQUIRE(input.has_value());
        CHECK(*input == std::vector<int>{0, 2, 3});
    }

    TEST_CASE("Day 0 rejects malformed input", "[day00]") {
        for (const auto *text :
             {"", " ", "1,", "1,,2", "1 2", "-1", "1,x", "1,2junk", "9999999999999999999999999"}) {
            CAPTURE(text);
            auto input = Solution::parse(text);
            REQUIRE_FALSE(input.has_value());
            CHECK_FALSE(input.error().empty());
        }
    }
#endif
} // namespace aoc::day00
