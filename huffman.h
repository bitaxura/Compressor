#pragma once
#include <vector>
#include <memory>
#include <optional>
using namespace std;

class HuffmanTree {
public:
    optional<uint8_t> symbol;
    optional<int> number;
    shared_ptr<HuffmanTree> left;
    shared_ptr<HuffmanTree> right;

    HuffmanTree(optional<uint8_t> symbol = nullopt,
                shared_ptr<HuffmanTree> left = nullptr,
                shared_ptr<HuffmanTree> right = nullptr);

    bool is_leaf() const;
    bool operator==(const HuffmanTree& other) const;
    bool operator<(const HuffmanTree& other) const;
    vector<uint8_t> num_nodes_to_bytes() const;
};
