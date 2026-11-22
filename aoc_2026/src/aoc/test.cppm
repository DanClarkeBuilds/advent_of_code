module;

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <expected>
#include <filesystem>
#include <string>

export module aoc.test;

import aoc.io;

export namespace aoc::test {
    template <typename Parser, typename Solver = std::nullptr_t> class Validator {
      public:
        explicit Validator(Parser parse, Solver solve = {}) : parse(parse), solve(solve) {}

        template <typename Function> auto operator()(Function solver) const {
            return Validator<Parser, Function>(parse, solver);
        }

        Validator &with(const std::filesystem::path &input_path) {
            path = aoc::resolve_input_path(input_path);
            optional = false;
            return *this;
        }

        Validator &with_optional(const std::filesystem::path &input_path) {
            with(input_path);
            optional = true;
            return *this;
        }

        template <typename Answer> void equals(const Answer &expected) const {
            INFO("Input: " << path.string());
            if (optional && !std::filesystem::exists(path)) {
                SKIP("Personal input file is not available: " << path.string());
            }

            auto lines = aoc::read_input(path);
            if (!lines) {
                FAIL(lines.error());
            }
            auto input = parse(*lines);
            if (!input) {
                FAIL("Could not parse input: " << input.error());
            }
            CHECK(solve(*input) == expected);
        }

      private:
        Parser parse;
        Solver solve;
        std::filesystem::path path;
        bool optional = false;
    };

} // namespace aoc::test
