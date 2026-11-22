module;

#include <expected>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

#ifdef AOC_TESTING
#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <system_error>
#include <utility>
#endif

export module aoc.io;

export namespace aoc {
    std::filesystem::path resolve_input_path(const std::filesystem::path &path) {
        return path.is_absolute() ? path : std::filesystem::path(AOC_SOURCE_DIR) / path;
    }

    std::expected<std::vector<std::string>, std::string>
    read_input(const std::filesystem::path &path) {
        const auto resolved = resolve_input_path(path);
        std::ifstream file(resolved, std::ios::binary);
        if (!file) {
            return std::unexpected("Could not open input: " + resolved.string());
        }

        const std::string contents{std::istreambuf_iterator<char>(file),
                                   std::istreambuf_iterator<char>()};
        if (file.bad()) {
            return std::unexpected("Could not read input: " + resolved.string());
        }

        auto lines =
            contents | std::views::split('\n') | std::views::transform([&contents](auto line) {
                std::string value(line.begin(), line.end());
                // Strip CR only when it belongs to a CRLF line ending.
                if (line.end() != contents.end() && !value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
                return value;
            }) |
            std::ranges::to<std::vector<std::string>>();

        // split includes an extra empty field after a final delimiter.
        if (!contents.empty() && contents.back() == '\n') {
            lines.pop_back();
        }
        return lines;
    }
} // namespace aoc

#ifdef AOC_TESTING
namespace {
    struct InputFixture {
        std::filesystem::path directory;

        InputFixture() {
            const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
            const auto base = std::filesystem::temp_directory_path();
            for (unsigned attempt = 0; attempt < 100; ++attempt) {
                auto candidate =
                    base / ("aoc-reader-" + std::to_string(stamp) + "-" + std::to_string(attempt));
                if (std::filesystem::create_directory(candidate)) {
                    directory = std::move(candidate);
                    return;
                }
            }
            FAIL("Could not create a temporary input directory");
        }

        ~InputFixture() {
            std::error_code error;
            std::filesystem::remove_all(directory, error);
        }
    };
} // namespace

TEST_CASE("Input reader preserves lines and whitespace", "[io]") {
    const InputFixture fixture;
    const auto path = fixture.directory / "input.txt";
    using Case = std::pair<std::string, std::vector<std::string>>;

    const auto text_content = std::vector<Case>{
        {"", {}},
        {"\n", {""}},
        {"\n\n", {"", ""}},
        {"alpha", {"alpha"}},
        {"alpha\n", {"alpha"}},
        {"alpha\n\n beta \t\n", {"alpha", "", " beta \t"}},
        {"alpha\r\n\r\n beta \t\r\n", {"alpha", "", " beta \t"}},
        {"alpha\nbeta", {"alpha", "beta"}},
        {"alpha\r", {"alpha\r"}},
    };

    for (const auto &[contents, expected] : text_content) {
        CAPTURE(contents);
        {
            std::ofstream file(path, std::ios::binary);
            file << contents;
            file.close();
            REQUIRE(file.good());
        }
        const auto lines = aoc::read_input(path);
        REQUIRE(lines.has_value());
        CHECK(*lines == expected);
    }
}

TEST_CASE("Input reader reports missing files", "[io]") {
    const InputFixture fixture;
    const auto path = fixture.directory / "missing.txt";
    const auto lines = aoc::read_input(path);
    REQUIRE_FALSE(lines.has_value());
    CHECK(lines.error().find(path.string()) != std::string::npos);
}
#endif
