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
        Validator(Parser parse, Solver solve = {}) : parse_(parse), solve_(solve) {}

        template <typename Function> auto operator()(Function solve) const {
            return Validator<Parser, Function>(parse_, solve);
        }

        Validator &with(const std::filesystem::path &path) {
            path_ = aoc::resolve_input_path(path);
            optional_ = false;
            return *this;
        }

        Validator &with_optional(const std::filesystem::path &path) {
            with(path);
            optional_ = true;
            return *this;
        }

        template <typename Answer> void equals(const Answer &expected) const {
            INFO("Input: " << path_.string());
            if (optional_ && !std::filesystem::exists(path_)) {
                SKIP("Personal input file is not available: " << path_.string());
            }

            auto text = aoc::read_input(path_);
            if (!text) {
                FAIL(text.error());
            }
            auto input = parse_(*text);
            if (!input) {
                FAIL("Could not parse input: " << input.error());
            }
            CHECK(solve_(*input) == expected);
        }

      private:
        Parser parse_;
        Solver solve_;
        std::filesystem::path path_;
        bool optional_ = false;
    };

} // namespace aoc::test
