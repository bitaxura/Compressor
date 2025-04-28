#include "huffman.h"

HuffmanTree::HuffmanTree(optional<uint8_t> symbol,
                         shared_ptr<HuffmanTree> left,
                         shared_ptr<HuffmanTree> right)
    : symbol(symbol), number(nullopt), left(left), right(right) {}

bool HuffmanTree::is_leaf() const {
    return !left && !right;
}

bool HuffmanTree::operator==(const HuffmanTree& other) const {
    return symbol == other.symbol && number == other.number &&
           left == other.left && right == other.right;
}

bool HuffmanTree::operator<(const HuffmanTree& other) const {
    return false;
}

vector<uint8_t> HuffmanTree::num_nodes_to_bytes() const {
    return {static_cast<uint8_t>(*number + 1)};
}
