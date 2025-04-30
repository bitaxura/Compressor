#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <C:\Users\jdaks\OneDrive\Documents\Coding Projects\compressor\huffman.h>
#include <C:\Users\jdaks\OneDrive\Documents\Coding Projects\compressor\utils.h>
using namespace std;

void build_frequency_table_parralel_helper(const vector<uint8_t>& data, int start, int end, unordered_map<uint8_t, int>& dict) { 
    for (int i = start; i < end; i++){
        dict[data[i]]++;
    }
}

unordered_map<uint8_t, int> build_frequency_table_parralel(const vector<uint8_t>& data){
    const int num_threads = thread::hardware_concurrency();
    const int size = data.size();
    const int chunk = (size + num_threads - 1) / num_threads;

    vector<unordered_map<uint8_t, int>> partial_dicts(num_threads);
    vector<thread> threads;

    for (int i = 0; i < num_threads; i++){
        int start = i * chunk;
        int end = min(start + chunk, size);

        threads.emplace_back(build_frequency_table_parralel_helper, cref(data), start, end, ref(partial_dicts[i]));
    }
    
    for (auto& t: threads){
        t.join();
    }

    unordered_map<uint8_t, int> combined_dict;

    for (const auto& d : partial_dicts) {
        for (const auto& x : d) {
            combined_dict[x.first] += x.second;
        }
    }

    return combined_dict;
}

unordered_map<uint8_t, int> build_frequency_table(const vector<uint8_t>& data) { 
    unordered_map<uint8_t, int> frequency_table;
    for (const uint8_t &byte : data) {
        frequency_table[byte]++;
    }
    return frequency_table;
}

vector<pair<uint8_t, int>> merge_sort(const vector<pair<uint8_t, int>>& lst);
vector<pair<uint8_t, int>> merge_vectors(const vector<pair<uint8_t, int>>& left, const vector<pair<uint8_t, int>>& right);
    
shared_ptr<HuffmanTree> build_huffman_tree(const unordered_map<uint8_t, int>& frequency_table) {
    vector<pair<uint8_t, int>> list;
    for (const auto& pair : frequency_table) {
        list.push_back(pair);
    }

    if (list.size() == 1) {
        shared_ptr<HuffmanTree> left = make_shared<HuffmanTree>(list[0].first);
        uint8_t dummy_symbol = (list[0].first + 1) % 256;
        shared_ptr<HuffmanTree> right = make_shared<HuffmanTree>(dummy_symbol);
        return make_shared<HuffmanTree>(nullopt, left, right);
    }

    auto sorted_list = merge_sort(list);
    
    vector<shared_ptr<HuffmanTree>> trees;
    vector<int> frequencies;

    for (const auto& pair : sorted_list) {
        trees.push_back(make_shared<HuffmanTree>(pair.first));
        frequencies.push_back(pair.second);
    }

    while (trees.size() > 1){
        int total_freq = frequencies[0] + frequencies[1];
        auto left = trees[0];
        auto right = trees[1];

        trees.erase(trees.begin(), trees.begin() + 2);
        frequencies.erase(frequencies.begin(), frequencies.begin() + 2);

        auto new_tree = make_shared<HuffmanTree>(nullopt, left, right);

        int i = 0;
        while (i < static_cast<int>(frequencies.size()) && frequencies[i] <= total_freq) {
            i++;
        }
        trees.insert(trees.begin() + i, new_tree);
        frequencies.insert(frequencies.begin() + i, total_freq);
    }
    return trees[0];
}

vector<pair<uint8_t, int>> merge_sort(const vector<pair<uint8_t, int>>& lst) {
    if (lst.size() <= 1) return lst;

    int mid = lst.size() / 2;
    vector<pair<uint8_t, int>> left(lst.begin(), lst.begin() + mid);
    vector<pair<uint8_t, int>> right(lst.begin() + mid, lst.end());

    vector<pair<uint8_t, int>> left_half = merge_sort(left);
    vector<pair<uint8_t, int>> right_half = merge_sort(right);

    return merge_vectors(left_half, right_half);
}

vector<pair<uint8_t, int>> merge_vectors(const vector<pair<uint8_t, int>>& left, const vector<pair<uint8_t, int>>& right) {
    vector<pair<uint8_t, int>> result;
    size_t i = 0, j = 0;

    while (i < left.size() && j < right.size()) {
        if (left[i].second <= right[j].second) {
            result.push_back(left[i]);
            i++;
        } else {
            result.push_back(right[j]);
            j++;
        }
    }
    while (i < left.size()) {
        result.push_back(left[i]);
        i++;
    }
    while (j < right.size()) {
        result.push_back(right[j]);
        j++;
    }
    return result;
}

unordered_map<uint8_t, string> get_codes(const shared_ptr<HuffmanTree>& tree, const string& prefix) {
    unordered_map<uint8_t, string> codes;
    if (tree->is_leaf()) {
        codes[tree->symbol.value()] = prefix;
    } else {
        auto left_codes = get_codes(tree->left, prefix + "0");
        auto right_codes = get_codes(tree->right, prefix + "1");
        codes.insert(left_codes.begin(), left_codes.end());
        codes.insert(right_codes.begin(), right_codes.end());
    }
    return codes;
}

int nodes_numberer(const shared_ptr<HuffmanTree>& tree, int number) {
    if (tree->is_leaf()) {
        return number;
    }
    number = nodes_numberer(tree->left, number);
    number = nodes_numberer(tree->right, number);

    tree->number = number;
    number++;

    return number;
}

void number_nodes(shared_ptr<HuffmanTree> tree) {
    nodes_numberer(tree, 0);
}

vector<shared_ptr<HuffmanTree>> internal_post_order(const shared_ptr<HuffmanTree>& tree) {
    if (!tree) return {};

    vector<shared_ptr<HuffmanTree>> trees;

    auto left_trees = internal_post_order(tree->left);
    trees.insert(trees.end(), left_trees.begin(), left_trees.end());

    auto right_trees = internal_post_order(tree->right);
    trees.insert(trees.end(), right_trees.begin(), right_trees.end());

    if (!tree->is_leaf()) trees.push_back(tree);

    return trees;
}

vector<uint8_t> tree_to_bytes(const shared_ptr<HuffmanTree>& tree) {
    vector<shared_ptr<HuffmanTree>> internal_nodes_list = internal_post_order(tree);
    vector<uint8_t> output;

    for (const auto &node : internal_nodes_list){
        if (node->left->is_leaf()){
            output.push_back(0);
            output.push_back(*node->left->symbol);
        }
        else{
            output.push_back(1);
            output.push_back(*node->left->number);
        }
        
        if (node->right->is_leaf()){
            output.push_back(0);
            output.push_back(*node->right->symbol);
        }
        else{
            output.push_back(1);
            output.push_back(*node->right->number);
        }
    }
    return output;
}

vector<uint8_t> compress_bytes(const vector<uint8_t>& data, unordered_map<uint8_t, string>& codes) {
    string mapped_string;
    mapped_string.reserve(data.size() * 8);

    auto start = chrono::high_resolution_clock::now();

    for (const uint8_t& x : data){
        mapped_string.append(codes[x]);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Time taken to map string: " << elapsed.count() << " seconds" << endl;

    auto start1 = chrono::high_resolution_clock::now();
    vector<uint8_t> compressed_data;

    for (size_t i = 0; i < mapped_string.size(); i += 8) {
        string byte = mapped_string.substr(i, 8);

        if (byte.size() < 8) {
            byte.append(8 - byte.size(), '0');
        }

        compressed_data.push_back(bits_to_byte(byte));
    }

    auto end1 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed1 = end1 - start1;
    cout << "Time taken to actuall compress with 8 bytes: " << elapsed1.count() << " seconds" << endl;

    return compressed_data;
}

double avg_length(const shared_ptr<HuffmanTree>& tree, const unordered_map<uint8_t, int>& freq_dict) {
    int freq_total = 0;
    for (const auto &x: freq_dict) freq_total += x.second;

    auto codes = get_codes(tree, "");
    int weighted_sum = 0;
    for (const auto &x : freq_dict) weighted_sum += (codes[x.first].length() * x.second);

    return static_cast<double>(weighted_sum) / freq_total;
}

void print_time_take(const chrono::high_resolution_clock::time_point& start, const string& label);

void compress_file(const string& infile, const string& outfile) {
    ifstream file(infile, ios::binary | ios::ate);

    size_t size = file.tellg();
    vector<uint8_t> buffer(size);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    auto start = chrono::high_resolution_clock::now();

    auto start1 = chrono::high_resolution_clock::now();
    unordered_map<uint8_t, int> freq = build_frequency_table_parralel(buffer); //Multithreaded
    //unordered_map<uint8_t, int> freq = build_frequency_table(buffer); //Single-Threaded
    print_time_take(start1, "Build Frequency Table");

    auto start2 = chrono::high_resolution_clock::now();
    shared_ptr<HuffmanTree> tree = build_huffman_tree(freq);
    print_time_take(start2, "Build Huffman Table");

    auto start3 = chrono::high_resolution_clock::now();
    unordered_map<uint8_t, string> code = get_codes(tree, "");
    print_time_take(start3, "Get Codes");

    auto start4 = chrono::high_resolution_clock::now();
    number_nodes(tree);
    print_time_take(start4, "Number Nodes");

    auto start5 = chrono::high_resolution_clock::now();
    cout << "Bits Per Symbol: " << avg_length(tree, freq) << endl;
    print_time_take(start5, "Calculate Avg Length");

    vector<uint8_t> result;
    vector<uint8_t> header = tree->num_nodes_to_bytes();
    vector<uint8_t> structure = tree_to_bytes(tree);
    vector<uint8_t> text_len = int32_to_bytes(buffer.size());

    auto start6 = chrono::high_resolution_clock::now();
    vector<uint8_t> compressed_text = compress_bytes(buffer, code);
    print_time_take(start6, "Time to compress bytes");

    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), structure.begin(), structure.end());
    result.insert(result.end(), text_len.begin(), text_len.end());
    result.insert(result.end(), compressed_text.begin(), compressed_text.end());

    ofstream output(outfile, ios::binary);
    output.write(reinterpret_cast<char*>(result.data()), result.size());
    output.close();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Total Time taken: " << elapsed.count() << " seconds" << endl;

    cout << "File compressed" << endl;
}

void print_time_take(const chrono::high_resolution_clock::time_point& start, const string& label){
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << label << " took " << elapsed.count() << " seconds " << endl;
}

shared_ptr<HuffmanTree> generate_tree_general(const vector<ReadNode>& node_list, int root_index) {
    auto node = node_list[root_index];

    shared_ptr<HuffmanTree> left;
    if (node.l_type == 0) {
        left = make_shared<HuffmanTree>(node.l_data);
    } else {
        left = generate_tree_general(node_list, node.l_data);
    }

    shared_ptr<HuffmanTree> right;
    if (node.r_type == 0) {
        right = make_shared<HuffmanTree>(node.r_data);
    } else {
        right = generate_tree_general(node_list, node.r_data);
    }

    return make_shared<HuffmanTree>(nullopt, left, right);
}

vector<uint8_t> decompress_bytes(const shared_ptr<HuffmanTree>& tree, const vector<uint8_t>& bytes, int size) {
    unordered_map<uint8_t, string> codes_dict = get_codes(tree, "");
    unordered_map<string, uint8_t> codes_dict_reversed;
    string bits_string;
    string current_bits;
    vector<uint8_t> output;

    for (const auto& x : codes_dict) {
        codes_dict_reversed[x.second] = x.first;
    }

    for (uint8_t byte : bytes) {
        bits_string += byte_to_bits(byte);
    }

    for (char bit : bits_string) {
        current_bits += bit;
        if (codes_dict_reversed.count(current_bits)) {
            output.push_back(codes_dict_reversed[current_bits]);
            current_bits = "";
            if (output.size() == size){
                break;
            }
        }
    }
    return output;
}

void decompress_file(const string& in_file, const string& out_file) {
    ifstream fin(in_file, ios::binary);
    ofstream fout(out_file, ios::binary);

    auto start = chrono::high_resolution_clock::now();

    uint8_t num_nodes_byte;
    fin.read(reinterpret_cast<char*>(&num_nodes_byte), 1);
    int num_nodes = static_cast<int>(num_nodes_byte);

    auto start1 = chrono::high_resolution_clock::now();
    vector<uint8_t> node_buf(num_nodes * 4);
    fin.read(reinterpret_cast<char*>(node_buf.data()), num_nodes * 4);
    vector<ReadNode> node_lst = bytes_to_nodes(node_buf);
    print_time_take(start1, "Time to bytes to nodes");

    auto start2 = chrono::high_resolution_clock::now();
    shared_ptr<HuffmanTree> tree = generate_tree_general(node_lst, num_nodes - 1);
    print_time_take(start1, "Time to generate tree general");

    auto start3 = chrono::high_resolution_clock::now();
    vector<uint8_t> size_buf(4);
    fin.read(reinterpret_cast<char*>(size_buf.data()), 4);
    int size = bytes_to_int(size_buf);
    print_time_take(start3, "Bytes to int");

    auto start4 = chrono::high_resolution_clock::now();
    vector<uint8_t> text((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());

    vector<uint8_t> decompressed = decompress_bytes(tree, text, size);
    print_time_take(start4, "decompress bytes");
    
    fout.write(reinterpret_cast<const char*>(decompressed.data()), decompressed.size());

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Total Time taken: " << elapsed.count() << " seconds" << endl;

    cout << "File Decompressed" << endl;
}


int main() {
    char input;

    cout << "Enter c to compress, d to decompress, or any other key to exit: ";
    cin >> input;

    if (input == 'c') {
        string file;
        cout << "File to compress: ";
        cin >> file;
        compress_file(file, file + ".huf");
    }
    else if (input == 'd'){
        string file;
        cout<<"File to decompress: ";
        cin>>file;
        decompress_file(file, file + ".orig");
    }
}