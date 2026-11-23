module;

#include <algorithm>
#include <expected>
#include <ranges>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
                    dsl::hash_sign >> dsl::n_digits<6, dsl::hex> + dsl::eof;
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
            struct Validator {
                virtual ~Validator() = default;

                virtual bool validate(const std::string value) = 0;
            };

            struct MinMaxValidator : Validator {
                size_t len;
                int min;
                int max;

                explicit MinMaxValidator(const int min, const int max, const size_t len = 0)
                    : len(len), min{min}, max{max} {}

                bool validate(const std::string value) override {
                    const auto num = std::stoi(value);

                    return (len == 0 || value.size() == len) && min <= num && num <= max;
                }
            };

            struct HeightValidator : Validator {
                std::unordered_map<std::string, MinMaxValidator> validators;

                explicit HeightValidator(
                    const std::unordered_map<std::string, MinMaxValidator> &validators)
                    : validators{validators} {}

                bool validate(const std::string value) override {
                    const lexy::string_input input(value.data(), value.size());

                    const auto fragment = lexy::parse<Height>(input, lexy::noop);
                    if (!fragment) {
                        return false;
                    }

                    return validators.at(fragment.value().second)
                        .validate(std::to_string(fragment.value().first));
                }
            };

            struct HexValidator : Validator {
                bool validate(const std::string value) override {
                    const lexy::string_input input(value.data(), value.size());

                    return !!lexy::validate<Hex>(input, lexy::noop);
                }
            };

            struct EyeValidator : Validator {
                std::unordered_set<std::string> expected_colours{
                    "amb", "blu", "brn", "gry", "grn", "hzl", "oth",
                };

                bool validate(const std::string value) override {
                    return expected_colours.contains(value);
                }
            };

            struct PidValidator : Validator {
                bool validate(const std::string value) override {
                    const lexy::string_input input(value.data(), value.size());

                    return !!lexy::validate<Pid>(input, lexy::noop);
                }
            };

            auto expected_fields = [] {
                std::unordered_map<std::string, std::unique_ptr<Validator>> fields;

                fields.emplace("byr", std::make_unique<MinMaxValidator>(1920, 2002, 4));
                fields.emplace("iyr", std::make_unique<MinMaxValidator>(2010, 2020, 4));
                fields.emplace("eyr", std::make_unique<MinMaxValidator>(2020, 2030, 4));

                fields.emplace("hgt", std::make_unique<HeightValidator>(
                                          std::unordered_map<std::string, MinMaxValidator>{
                                              {"cm", MinMaxValidator(150, 193)},
                                              {"in", MinMaxValidator(59, 76)},
                                          }));

                fields.emplace("hcl", std::make_unique<HexValidator>());
                fields.emplace("ecl", std::make_unique<EyeValidator>());
                fields.emplace("pid", std::make_unique<PidValidator>());

                return fields;
            }();
        } // namespace
    } // namespace

    export struct Solution {
        static constexpr auto input_path = "inputs/day04_2020.txt";
        static constexpr auto example_path = "examples/day04_2020.txt";

        [[nodiscard]] static std::expected<Input, std::string>
        parse(const std::span<const std::string> lines) {
            return lines | std::views::split(std::string{}) |
                   std::views::transform([=](auto group) -> ExpectedRecord {
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
            return std::ranges::count_if(input, [=](const Record &record) {
                return std::ranges::all_of(
                    expected_fields | std::views::keys,
                    [&record](const auto &key) -> bool { return record.contains(key); });
            });
        }

        [[nodiscard]] static std::int64_t part2(const Input &input) {
            return std::ranges::count_if(input, [=](const Record &record) {
                return std::ranges::all_of(expected_fields, [&record](const auto &field) -> bool {
                    const auto &[key, validator] = field;

                    return record.contains(key) && validator->validate(record.at(key));
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
        SKIP("TODO: set the expected answer, then remove this SKIP.");
        validate(Solution::part2).with_optional(Solution::input_path).equals(188);
    }
#endif
} // namespace aoc::day04_2020
