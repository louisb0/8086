#include "8086.hpp"

#include "8086_decoder.hpp"
#include "8086_matcher.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>

// TODO(louis): move
class InstructionPrinter {
public:
    static void print(const Instruction &inst) {
        std::cout << format_instruction(inst) << '\n';
    }

    static std::string format_instruction(const Instruction &inst) {
        std::stringstream ss;

        ss << std::hex << std::setfill('0') << std::setw(4) << inst.address << "  ";

        for (u8 byte : inst.bytes) {
            ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte)
               << " ";
        }

        for (size_t i = inst.bytes.size(); i < 6; i++) {
            ss << "   ";
        }

        ss << inst.op << " " << inst.dst;
        if (!inst.src.empty()) {
            ss << ", " << inst.src;
        }

        return ss.str();
    }
};

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

    InstructionDecoder decoder(file);

    while (file && !file.eof()) {
        u8 first_byte = file.get();
        if (file.eof())
            break;

        for (const auto &pattern : InstructionMatcher::patterns) {
            if (auto inst = decoder.try_decode(pattern, first_byte)) {
                InstructionPrinter::print(inst.value());
                break;
            }
        }
    }

    return 0;
}
