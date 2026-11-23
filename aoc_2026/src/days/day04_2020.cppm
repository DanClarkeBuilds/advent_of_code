module;

#include <algorithm>
#include <array>
#include <charconv>
#include <expected>
#include <ranges>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>

#ifdef AOC_TESTING
#include <catch2/catch_test_macros.hpp>
#endif

export module aoc.day04_2020;

#ifdef AOC_TESTING
import aoc.test;
#endif

namespace aoc::day04_2020 {
    using Record = std::unordered_map<std::string, std::string>;
    using ExpectedRecord = std::expected<Record, std::string>;
    using Input = std::vector<Record>;

    namespace {
        namespace {
            namespace dsl = lexy::dsl;

            struct KeyText {
                static constexpr auto rule = dsl::identifier(dsl::ascii::alpha);
                static constexpr auto value = lexy::as_string<std::string>;
            };

            struct ValueText {
                static constexpr auto rule = dsl::identifier(dsl::ascii::graph);
                static constexpr auto value = lexy::as_string<std::string>;
            };

            struct KeyValues {
                static constexpr auto key_value = dsl::p<KeyText> + dsl::colon + dsl::p<ValueText>;

                static constexpr auto rule =
                    dsl::list(key_value, dsl::sep(dsl::ascii::blank)) + dsl::eof;

                static constexpr auto value = lexy::as_collection<Record>;
            };

            struct Height {
                static constexpr auto rule = dsl::integer<int> + dsl::p<KeyText> + dsl::eof;
                static constexpr auto value = lexy::construct<std::pair<int, std::string>>;
            };

            struct Hex {
                static constexpr auto rule =
                    dsl::hash_sign >> dsl::n_digits<6, dsl::hex_lower> + dsl::eof;
            };

            struct Pid {
                static constexpr auto rule = dsl::n_digits<9, dsl::decimal> + dsl::eof;
            };
        } // namespace

        namespace {
            struct CollectRecords : std::ranges::range_adaptor_closure<CollectRecords> {
                template <std::ranges::input_range Range>
                std::expected<Input, std::string> operator()(Range &&records) const {
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

            constexpr CollectRecords collect_records{};
        } // namespace

        namespace {
            constexpr bool within(const int n, const int lo, const int hi) {
                return lo <= n && n <= hi;
            }

            bool year(const std::string_view &value, const int lo, const int hi) {
                const auto year = std::stoi(std::string(value));

                return value.size() == 4 && within(year, lo, hi);
            }

            bool validate_height(const std::string_view &value) {
                const lexy::string_input input(value.data(), value.size());

                const auto parsed_height = lexy::parse<Height>(input, lexy::noop);
                if (!parsed_height) {
                    return false;
                }

                const auto [height, unit] = parsed_height.value();
                if (unit == "cm") {
                    return within(height, 150, 193);
                }
                if (unit == "in") {
                    return within(height, 59, 76);
                }
                return false;
            }

            bool validate_hex(const std::string_view &value) {
                const lexy::string_input input(value.data(), value.size());

                return !!lexy::validate<Hex>(input, lexy::noop);
            }

            constexpr std::array<std::string, 7> expected_colours{
                "amb", "blu", "brn", "gry", "grn", "hzl", "oth",
            };
            bool validate_eye(const std::string_view &value) {
                return std::ranges::find(expected_colours, value) != expected_colours.end();
            }

            bool validate_pid(const std::string_view &value) {
                const lexy::string_input input(value.data(), value.size());

                return !!lexy::validate<Pid>(input, lexy::noop);
            }

            struct Field {
                std::string_view key;
                bool (*valid)(const std::string_view &);
            };

            constexpr std::array expected_fields{
                Field{.key = "byr", .valid = [](auto s) { return year(s, 1920, 2002); }},
                Field{.key = "iyr", .valid = [](auto s) { return year(s, 2010, 2020); }},
                Field{.key = "eyr", .valid = [](auto s) { return year(s, 2020, 2030); }},
                Field{.key = "hgt", .valid = validate_height},
                Field{.key = "hcl", .valid = validate_hex},
                Field{.key = "ecl", .valid = validate_eye},
                Field{.key = "pid", .valid = validate_pid},
            };
        } // namespace
    } // namespace

    export struct Solution {
        static constexpr auto input_path = "inputs/day04_2020.txt";
        static constexpr auto example_path = "examples/day04_2020.txt";

        [[nodiscard]] static std::expected<Input, std::string>
        parse(const std::span<const std::string> lines) {
            return lines | std::views::split(std::string{}) |
                   std::views::transform([](auto group) -> ExpectedRecord {
                       return std::ranges::fold_left(
                           group, ExpectedRecord{Record{}},
                           [=](ExpectedRecord acc, const auto &line) -> ExpectedRecord {
                               const lexy::string_input input(line.data(), line.size());

                               auto fragment = lexy::parse<KeyValues>(input, lexy::noop);
                               if (!fragment) {
                                   return std::unexpected("Expected a map of key->value");
                               }

                               acc->insert(fragment.value().begin(), fragment.value().end());
                               return acc;
                           });
                   }) |
                   collect_records;
        }

        [[nodiscard]] static std::int64_t part1(const Input &input) {
            return std::ranges::count_if(input, [](const Record &record) {
                return std::ranges::all_of(expected_fields, [&record](const auto &field) -> bool {
                    return record.contains(field.key.data());
                });
            });
        }

        [[nodiscard]] static std::int64_t part2(const Input &input) {
            return std::ranges::count_if(input, [](const Record &record) {
                return std::ranges::all_of(expected_fields, [&record](const auto &field) -> bool {
                    const auto it = record.find(field.key.data());

                    return it != record.end() && field.valid(it->second);
                });
            });
        }
    };

#ifdef AOC_TESTING
    namespace {
        const auto validate = aoc::test::Validator(Solution::parse);
    }

    TEST_CASE("day04_2020 part1 example", "[day04_2020]") {
        validate(Solution::part1).with(Solution::example_path).equals(2);
    }

    TEST_CASE("day04_2020 part1", "[day04_2020][input]") {
        validate(Solution::part1).with_optional(Solution::input_path).equals(239);
    }

    TEST_CASE("day04_2020 part2 example", "[day04_2020]") {
        validate(Solution::part2).with(Solution::example_path).equals(2);
    }

    TEST_CASE("day04_2020 part2", "[day04_2020][input]") {
        validate(Solution::part2).with_optional(Solution::input_path).equals(188);
    }
#endif
} // namespace aoc::day04_2020
