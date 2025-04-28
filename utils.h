#pragma once

#include <optional>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>

using namespace std;

class ReadNode {
public:
    int l_type;
    int l_data;
    int r_type;
    int r_data;

    ReadNode(int l_type, int l_data, int r_type, int r_data);
};

bool get_bit(uint8_t byte, int bit_num);

string byte_to_bits(uint8_t byte);

uint8_t bits_to_byte(const string& bits);

vector<uint8_t> int32_to_bytes(int num);

int bytes_to_int(const vector<uint8_t>& buf);
