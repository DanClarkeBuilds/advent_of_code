module;

#include <expected>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

export module aoc.io;

export namespace aoc {
    std::filesystem::path resolve_input_path(const std::filesystem::path &path) {
        return path.is_absolute() ? path : std::filesystem::path(AOC_SOURCE_DIR) / path;
    }

    std::expected<std::string, std::string> read_input(const std::filesystem::path &path) {
        const auto resolved = resolve_input_path(path);
        std::ifstream file(resolved, std::ios::binary);
        if (!file) {
            return std::unexpected("Could not open input: " + resolved.string());
        }

        std::string contents{std::istreambuf_iterator<char>(file),
                             std::istreambuf_iterator<char>()};
        if (file.bad()) {
            return std::unexpected("Could not read input: " + resolved.string());
        }
        return contents;
    }
} // namespace aoc
