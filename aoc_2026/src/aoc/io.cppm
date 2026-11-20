module;

#include <expected>
#include <fstream>
#include <iterator>
#include <string>

export module aoc.io;

export namespace aoc {
    std::expected<std::string, std::string> read_input(const std::string &path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return std::unexpected("Could not open input: " + path);
        }

        std::string contents{std::istreambuf_iterator<char>(file),
                             std::istreambuf_iterator<char>()};
        if (file.bad()) {
            return std::unexpected("Could not read input: " + path);
        }
        return contents;
    }
} // namespace aoc
