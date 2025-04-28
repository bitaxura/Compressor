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

void compress_file(const string& infile, const string& outfile) {
    ifstream file(infile, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Failed to open input file." << endl;
        return;
    }

    size_t size = file.tellg();
    vector<uint8_t> buffer(size);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    auto start = chrono::high_resolution_clock::now();

    auto start1 = chrono::high_resolution_clock::now();
    unordered_map<uint8_t, int> freq = build_frequency_table_parralel(buffer);
    auto end1 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed1 = end1 - start1;
    cout << "Time taken to build dict: " << elapsed1.count() << " seconds" << endl;

    auto start2 = chrono::high_resolution_clock::now();
    shared_ptr<HuffmanTree> tree = build_huffman_tree(freq);
    auto end2 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed2 = end2 - start2;
    cout << "Time taken to build tree: " << elapsed2.count() << " seconds" << endl;

    auto start3 = chrono::high_resolution_clock::now();
    unordered_map<uint8_t, string> code = get_codes(tree, "");
    auto end3 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed3 = end3 - start3;
    cout << "Time taken to get codes: " << elapsed3.count() << " seconds" << endl;

    auto start4 = chrono::high_resolution_clock::now();
    number_nodes(tree);
    auto end4 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed4 = end4 - start4;
    cout << "Time taken to number nodes: " << elapsed4.count() << " seconds" << endl;

    auto start5 = chrono::high_resolution_clock::now();
    cout << "Bits Per Symbol: " << avg_length(tree, freq) << endl;
    auto end5 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed5 = end5 - start5;
    cout << "Time taken to calc avg length: " << elapsed5.count() << " seconds" << endl;

    vector<uint8_t> result;
    vector<uint8_t> header = tree->num_nodes_to_bytes();
    vector<uint8_t> structure = tree_to_bytes(tree);
    vector<uint8_t> text_len = int32_to_bytes(buffer.size());

    auto start6 = chrono::high_resolution_clock::now();
    vector<uint8_t> compressed_text = compress_bytes(buffer, code);
    auto end6 = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed6 = end6 - start6;
    cout << "Time taken to compress bytes: " << elapsed6.count() << " seconds" << endl;

    result.insert(result.end(), header.begin(), header.end());
    result.insert(result.end(), structure.begin(), structure.end());
    result.insert(result.end(), text_len.begin(), text_len.end());
    result.insert(result.end(), compressed_text.begin(), compressed_text.end());

    ofstream output(outfile, ios::binary);
    output.write(reinterpret_cast<char*>(result.data()), result.size());
    output.close();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Time taken: " << elapsed.count() << " seconds" << endl;

    cout << "File compressed AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl;
    int input;
    cin >> input;
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
    /*else if (input == 'd'){
        string file;
        cout<<"File to decompress: ";
        cin>>file;
        
    }*/
}