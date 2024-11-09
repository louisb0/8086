#include "common.hpp"

#include "decode.hpp"
#include "print.hpp"
#include "registers.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
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

    sim::decode::Decoder decoder;
    sim::registers::RegFile regfile;

    size_t pc = 0;
    while (pc < memory.size()) {
        auto inst_optional = decoder.try_decode(memory, pc);
        if (!inst_optional) {
            // TODO(louis): something error handling
            break;
        }

        const auto &inst = *inst_optional;
        pc += inst.bytes.size();

        sim::print::instruction(inst);

        if (inst.mnemonic == "mov") {
            regfile.write(inst.dst.reg_access, inst.src.immediate);
            continue;
        }

        u16 dst = regfile.read(inst.dst.reg_access);
        u16 src;
        switch (inst.src.type) {
        case sim::instructions::Operand::Type::REGISTER:
            src = regfile.read(inst.src.reg_access);
            break;
        case sim::instructions::Operand::Type::IMMEDIATE:
            src = inst.src.immediate;
            break;
        default:
            UNREACHABLE();
        };

        u16 res;
        if (inst.mnemonic == "add") {
            res = dst + src;
            regfile.write(inst.dst.reg_access, res);
        } else if (inst.mnemonic == "sub") {
            res = dst - src;
            regfile.write(inst.dst.reg_access, res);
        } else if (inst.mnemonic == "cmp") {
            res = dst - src;
        }

        if (!inst.dst.reg_access.is_wide)
            res &= 0xFF;

        regfile.set_flag(sim::registers::ZF, res == 0);
        regfile.set_flag(sim::registers::SF, res & (inst.dst.reg_access.is_wide ? 0x8000 : 0x80));

        std::cout << "SF: " << static_cast<int>(regfile.test_flag(sim::registers::SF))
                  << ", ZF: " << static_cast<int>(regfile.test_flag(sim::registers::ZF)) << "\n";
    }

    std::cout << std::endl << regfile.string();
    return 0;
}
