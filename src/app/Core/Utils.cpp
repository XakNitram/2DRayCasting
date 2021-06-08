#include "pch.hpp"
#include "Utils.hpp"

std::string readFile(const std::string &filepath) {
    // need to figure out how to handle errors on this.
    std::ifstream file(filepath);

    std::stringstream output_stream;

    std::string line;
    while (getline(file, line)) {
        output_stream << line << '\n';
    }

    return output_stream.str();
}
