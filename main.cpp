#include <array>
#include <bitset>
#include <fstream>
#include <iostream>

constexpr std::array<std::string_view, 8> WORD_REGISTERS = {
    "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

constexpr std::array<std::string_view, 8> BYTE_REGISTERS = {
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

int main(int argc, char *argv[]) {
  std::ifstream file(argv[1], std::ios::binary);

  for (uint16_t word;
       file.read(reinterpret_cast<char *>(&word), sizeof(word));) {
    word = (word << 8) | (word >> 8);

    uint8_t opcode = (word & 0xFC00) >> 10;
    bool is_reg_dst = (word & 0x0200) != 0;
    bool is_word_data = (word & 0x0100) != 0;
    uint8_t mode = (word & 0x00C0) >> 6;
    uint8_t reg = (word & 0x0038) >> 3;
    uint8_t rm = word & 0x0007;

    uint8_t dst = is_reg_dst ? reg : rm;
    uint8_t src = is_reg_dst ? rm : reg;

    const auto &registers = is_word_data ? WORD_REGISTERS : BYTE_REGISTERS;
    std::cout << "mov " << registers[dst] << ", " << registers[src] << '\n';
  }
}
