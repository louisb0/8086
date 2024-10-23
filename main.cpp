#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

using u8 = std::uint8_t;
using u16 = std::uint16_t;

constexpr std::array<std::string_view, 8> WORD_REGISTERS = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
constexpr std::array<std::string_view, 8> BYTE_REGISTERS = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
constexpr std::array<std::string_view, 8> EQUATIONS = {"bx + si", "bx + di", "bp + si", "bp + di",
                                                       "si",      "di",      "bp",      "bx"};
const std::unordered_map<u8, std::string_view> JUMP_MNEMONICS = {
    {0x74, "je"},  {0x7C, "jl"},  {0x7E, "jle"},  {0x72, "jb"},    {0x76, "jbe"},    {0x7A, "jp"},  {0x70, "jo"},
    {0x78, "js"},  {0x75, "jne"}, {0x7D, "jnl"},  {0x7F, "jg"},    {0x73, "jnb"},    {0x77, "ja"},  {0x7B, "jnp"},
    {0x71, "jno"}, {0x79, "jns"}, {0xE2, "loop"}, {0xE1, "loopz"}, {0xE0, "loopnz"}, {0xE3, "jcxz"}};

class InstructionDecoder {
public:
    InstructionDecoder(std::ifstream &f) : file(f) {}

    void rm_with_reg(const std::string &mnemonic, u16 word) {
        bool is_reg_dst = (word & 0x0200) != 0;
        bool is_word = (word & 0x0100) != 0;
        u8 mod = (word & 0x00C0) >> 6;
        u8 reg = (word & 0x0038) >> 3;
        u8 rm = word & 0x0007;

        std::string reg_operand = fmt_reg_operand(is_word, reg);
        std::string rm_operand = read_and_fmt_rm_operand(mod, rm, is_word);

        if (is_reg_dst) {
            print_instruction(mnemonic, reg_operand, rm_operand);
        } else {
            print_instruction(mnemonic, rm_operand, reg_operand);
        }
    }

    void imm_to_rm(const std::string &mnemonic, u16 word) {
        bool should_sign_extend = (word & 0x0200) != 0;
        bool is_word = (word & 0x0100) != 0;
        u8 mod = (word & 0x00C0) >> 6;
        u8 rm = word & 0x0007;

        std::string rm_operand = read_and_fmt_rm_operand(mod, rm, is_word);

        u16 data = read_byte();
        if ((!should_sign_extend || mnemonic == "mov") && is_word) {
            data = (read_byte() << 8) | data;
        }

        print_instruction(mnemonic, rm_operand, std::to_string(data));
    }

    void mov_imm_to_reg(u16 word) {
        bool is_word = word & 0x0800;
        u8 reg = (word >> 8) & 0x07;

        u16 data = word & 0x00FF;
        if (is_word)
            data = (read_byte() << 8) | data;

        print_instruction("mov", fmt_reg_operand(is_word, reg), std::to_string(data));
    }

    void imm_to_accumulator(const std::string &mnemonic, u16 word) {
        bool is_word = word & 0x0100;

        u16 data = word & 0x00FF;
        if (is_word)
            data = (read_byte() << 8) | data;

        print_instruction(mnemonic, is_word ? "ax" : "al", std::to_string(data));
    }

    void jump(u16 word) {
        int8_t displacement = static_cast<int8_t>(word & 0x00FF);

        std::cout << JUMP_MNEMONICS.find(word >> 8)->second << " " << (int)displacement << std::endl;
    }

private:
    std::ifstream &file;

    u8 read_byte() { return file.get(); }
    u16 read_word() { return (read_byte() << 8) | read_byte(); }

    std::string fmt_reg_operand(bool is_word, u8 reg) {
        return std::string(is_word ? WORD_REGISTERS[reg] : BYTE_REGISTERS[reg]);
    }

    std::string read_and_fmt_rm_operand(u8 mod, u8 rm, bool is_word) {
        switch (mod) {
        case 0b00:
            if (rm == 0b110) {
                u16 addr = read_word();
                addr = (addr << 8) | (addr >> 8);

                return "[" + std::to_string(addr) + "]";
            }

            return "[" + std::string(EQUATIONS[rm]) + "]";
        case 0b01: {
            u8 disp = read_byte();

            return "[" + std::string(EQUATIONS[rm]) + " + " + std::to_string(disp) + "]";
        }
        case 0b10: {
            u16 disp = read_word();
            disp = (disp << 8) | (disp >> 8);

            return "[" + std::string(EQUATIONS[rm]) + " + " + std::to_string(disp) + "]";
        }
        case 0b11:
            return fmt_reg_operand(is_word, rm);

        default:
            return "unknown mod";
        }
    }

    void print_instruction(const std::string &mnemonic, const std::string &dst, const std::string &src) {
        std::cout << mnemonic << " " << dst << ", " << src << '\n';
    }
};

u16 bits(u16 value, size_t start, size_t end) {
    u8 num_bits = end - start + 1;
    u16 mask = ((1U << num_bits) - 1) << (15 - end);
    u16 masked = value & mask;

    return masked >> (15 - end);
}

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
        u16 word = (file.get() << 8) | file.get();
        if (file.eof())
            break;

        // should be in the decoder
        if (bits(word, 0, 5) == 0b100010)
            decoder.rm_with_reg("mov", word);
        else if (bits(word, 0, 6) == 0b1100011)
            decoder.imm_to_rm("mov", word);
        else if (bits(word, 0, 3) == 0b1011)
            decoder.mov_imm_to_reg(word);

        else if (bits(word, 0, 5) == 0b000000)
            decoder.rm_with_reg("add", word);
        else if (bits(word, 0, 5) == 0b100000 && bits(word, 10, 12) == 0b000)
            decoder.imm_to_rm("add", word);
        else if (bits(word, 0, 6) == 0b0000010)
            decoder.imm_to_accumulator("add", word);

        else if (bits(word, 0, 5) == 0b001010)
            decoder.rm_with_reg("sub", word);
        else if (bits(word, 0, 5) == 0b100000 && bits(word, 10, 12) == 0b101)
            decoder.imm_to_rm("sub", word);
        else if (bits(word, 0, 6) == 0b0010110)
            decoder.imm_to_accumulator("sub", word);

        else if (bits(word, 0, 5) == 0b001110)
            decoder.rm_with_reg("cmp", word);
        else if (bits(word, 0, 5) == 0b100000 && bits(word, 10, 12) == 0b111)
            decoder.imm_to_rm("cmp", word);
        else if (bits(word, 0, 6) == 0b0011110)
            decoder.imm_to_accumulator("cmp", word);

        else if (JUMP_MNEMONICS.find(word >> 8) != JUMP_MNEMONICS.end())
            decoder.jump(word);
    }
}
