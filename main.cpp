#include "common.hpp"

#include "decode.hpp"
#include "print.hpp"
#include "table.hpp"

#include <iostream>

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

    sim::decode::Decoder decoder(file);

    while (file && !file.eof()) {
        u8 first_byte = file.get();

        // NOTE(louis): inefficient. precompute a jump table or something
        for (const auto &encoding : sim::decode::table::instruction_encodings) {
            if (auto inst = decoder.try_decode(encoding, first_byte)) {
                sim::print::instruction(inst.value());
                break;
            }
        }
    }

    return 0;
}
