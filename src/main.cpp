#include "common.hpp"

#include "runner.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << '\n';
        return 1;
    }

    std::vector<u8> memory;

    memory.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    sim::runner::Runner runner(std::move(memory));
    runner.run();

    return 0;
}
