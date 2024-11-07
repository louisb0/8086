#include "common.hpp"

#include "decode.hpp"
#include "print.hpp"
#include "register_file.hpp"
#include "table.hpp"

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

    sim::decode::Decoder decoder(file);

    std::vector<sim::instructions::Instruction> instructions;
    while (file && !file.eof()) {
        u8 first_byte = file.get();

        // NOTE(louis): inefficient. precompute a jump table or something
        for (const auto &encoding : sim::decode::table::instruction_encodings) {
            if (auto inst = decoder.try_decode(encoding, first_byte)) {
                instructions.push_back(inst.value());
                break;
            }
        }
    }

    sim::print::instructions(instructions);

    sim::simulator::RegisterFile regfile;
    for (const auto &inst : instructions) {
        if (inst.mnemonic == "mov") {
            regfile.write(inst.dst.reg_access, inst.src.imm);
            continue;
        }

        u16 dst = regfile.read(inst.dst.reg_access);
        u16 src;
        switch (inst.src.type) {
        case sim::instructions::Operand::Type::REGISTER:
            src = regfile.read(inst.src.reg_access);
            break;
        case sim::instructions::Operand::Type::IMMEDIATE:
            src = inst.src.imm;
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

        if (inst.dst.reg_access.is_wide) {
            regfile.flags.ZF = (res == 0);
            regfile.flags.SF = (res & 0x8000) != 0;
        } else {
            res &= 0xFF;
            regfile.flags.ZF = (res == 0);
            regfile.flags.SF = (res & 0x80) != 0;
        }

        std::cout << "\nSF: " << static_cast<int>(regfile.flags.SF)
                  << ", ZF: " << static_cast<int>(regfile.flags.ZF);
    }

    std::cout << std::endl << regfile.string();
    return 0;
}
