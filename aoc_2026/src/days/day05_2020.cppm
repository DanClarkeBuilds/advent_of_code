module;

#include <algorithm>
#include <array>
#include <expected>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>

#ifdef AOC_TESTING
#include <catch2/catch_test_macros.hpp>
#endif

export module aoc.day05_2020;

#ifdef AOC_TESTING
import aoc.test;
#endif

namespace aoc::day05_2020 {
    namespace {
        enum class DIR { F, B };

        struct Seat {
            std::array<DIR, 7> down;
            std::array<DIR, 3> across;
        };

        using Input = std::vector<Seat>;
        using ExpectedSeat = std::expected<Seat, std::string>;

        namespace dsl = lexy::dsl;

        struct Down {
            static constexpr auto down_sym =
                lexy::symbol_table<DIR>.map<'F'>(DIR::F).map<'B'>(DIR::B);
            static constexpr auto rule = dsl::times<7>(dsl::symbol<down_sym>);
            static constexpr auto value = lexy::callback<std::array<DIR, 7>>(
                [](auto... xs) { return std::array<DIR, 7>{xs...}; });
        };

        struct Across {
            static constexpr auto across_sym =
                lexy::symbol_table<DIR>.map<'L'>(DIR::F).map<'R'>(DIR::B);
            static constexpr auto rule = dsl::times<3>(dsl::symbol<across_sym>);
            static constexpr auto value = lexy::callback<std::array<DIR, 3>>(
                [](auto... xs) { return std::array<DIR, 3>{xs...}; });
        };

        struct SeatParser {
            static constexpr auto rule = dsl::p<Down> + dsl::p<Across> + dsl::eof;
            static constexpr auto value = lexy::construct<Seat>;
        };

        ExpectedSeat parse_seats(const std::string_view line) {
            const lexy::string_input input(line.data(), line.size());

            auto result = lexy::parse<SeatParser>(input, lexy::noop);
            if (!result) {
                return std::unexpected("Expected a vector of seats");
            }
            return std::move(result).value();
        }

        struct CollectRecords : std::ranges::range_adaptor_closure<CollectRecords> {
            template <std::ranges::input_range Range>
            std::expected<std::vector<Seat>, std::string> operator()(Range &&records) const {
                Input input;

                for (auto record : records) {
                    if (!record) {
                        return std::unexpected(record.error());
                    }

                    input.push_back(std::move(*record));
                }

                return input;
            }
        };

        std::pair<int, int> locate(std::pair<int, int> acc, const DIR dir) {
            const auto half = (acc.second - acc.first + 1) / 2;

            switch (dir) {
                case DIR::F:
                    acc.second -= half;
                    break;
                case DIR::B:
                    acc.first += half;
                    break;
            }

            return acc;
        }

        std::vector<int64_t> seat_numbers(const Input &input) {
            return input | std::views::transform([](const auto &seat) -> std::int64_t {
                       const auto down =
                           std::ranges::fold_left(seat.down, std::make_pair(0, 127), locate);
                       const auto across =
                           std::ranges::fold_left(seat.across, std::make_pair(0, 7), locate);

                       return down.first * 8 + across.first;
                   }) |
                   std::ranges::to<std::vector<std::int64_t>>();
        }
    } // namespace

    export struct Solution {
        static constexpr auto input_path = "inputs/day05_2020.txt";
        static constexpr auto example_path = "examples/day05_2020.txt";

        [[nodiscard]] static std::expected<Input, std::string>
        parse(const std::span<const std::string> lines) {
            return lines | std::views::transform(parse_seats) | CollectRecords{};
        }

        [[nodiscard]] static std::int64_t part1(const Input &input) {
            return std::ranges::max(seat_numbers(input));
        }

        [[nodiscard]] static std::int64_t part2(const Input &input) {
            auto numbers = seat_numbers(input);
            std::ranges::sort(numbers);

            auto [fst, snd] = std::make_pair(numbers.begin(), numbers.end() - 1);
            while (snd - fst > 1) {
                const auto mid = fst + (snd - fst) / 2;

                if (*mid - *fst > mid - fst) {
                    snd = mid;
                } else {
                    fst = mid;
                }
            }

            return *fst + 1;
        }
    };

#ifdef AOC_TESTING
    namespace {
        const auto validate = aoc::test::Validator(Solution::parse);
    }

    TEST_CASE("day05_2020 part1 example", "[day05_2020]") {
        validate(Solution::part1).with(Solution::example_path).equals(357);
    }

    TEST_CASE("day05_2020 part1", "[day05_2020][input]") {
        validate(Solution::part1).with_optional(Solution::input_path).equals(883);
    }

    TEST_CASE("day05_2020 part2", "[day05_2020][input]") {
        validate(Solution::part2).with_optional(Solution::input_path).equals(532);
    }
#endif
} // namespace aoc::day05_2020
