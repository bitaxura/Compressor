#include "utils.h"

ReadNode::ReadNode(int l_type, int l_data, int r_type, int r_data)
    : l_type(l_type), l_data(l_data), r_type(r_type), r_data(r_data) {}

bool get_bit(uint8_t byte, int bit_num) {
    return (byte >> bit_num) & 1;
}

string byte_to_bits(uint8_t byte) {
    string bits(8, '0');
    for (int i = 0; i < 8; ++i)
        if (get_bit(byte, 7 - i)) bits[i] = '1';
    return bits;
}

uint8_t bits_to_byte(const string& bits) {
    uint8_t result = 0;
    int len = bits.length();
    for (int i = 0; i < len; ++i) {
        result <<= 1;
        result |= (bits[i] == '1');
    }
    result <<= (8 - len);
    return result;
}

vector<uint8_t> int32_to_bytes(int num) {
    return { 
        static_cast<uint8_t>(num & 0xFF),
        static_cast<uint8_t>((num >> 8) & 0xFF),
        static_cast<uint8_t>((num >> 16) & 0xFF),
        static_cast<uint8_t>((num >> 24) & 0xFF)
    };
}

int bytes_to_int(const vector<uint8_t>& buf) {
    return static_cast<int>(buf[0])
         | (static_cast<int>(buf[1]) << 8)
         | (static_cast<int>(buf[2]) << 16)
         | (static_cast<int>(buf[3]) << 24);
}

vector<ReadNode> bytes_to_nodes(const std::vector<uint8_t>& buf) {
    std::vector<ReadNode> nodes;
    for (size_t i = 0; i + 3 < buf.size(); i += 4) {
        int l_type = buf[i];
        int l_data = buf[i + 1];
        int r_type = buf[i + 2];
        int r_data = buf[i + 3];
        nodes.emplace_back(l_type, l_data, r_type, r_data);
    }
    return nodes;
}