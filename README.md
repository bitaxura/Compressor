```
# Huffman Compression and Decompression

This project implements a file compression and decompression utility using the Huffman coding algorithm. It supports both single-threaded and multi-threaded operations for efficient processing of large files.

## Features

- **Compression**: Compresses files using Huffman coding to reduce file size.
- **Decompression**: Decompresses files back to their original form.
- **Multi-threading**: Utilizes multiple threads for faster compression of large files.
- **Custom Huffman Tree**: Builds and serializes a Huffman tree for encoding and decoding.

## File Structure

- **`main.cpp`**: Contains the main logic for file compression and decompression, including multi-threaded operations.
- **`huffman.cpp`** and **`huffman.h`**: Implements the Huffman tree structure and related operations.
- **`utils.cpp`** and **`utils.h`**: Provides utility functions for bit manipulation, byte conversion, and other helper methods.

## How to Build

1. Ensure you have a C++ compiler installed (e.g., GCC or MSVC).
2. Compile the project using the following command:
   ```
   g++ -std=c++17 -O3 -o compressor main.cpp huffman.cpp utils.cpp
   ```

## How to Use

1. Run the compiled executable:
   ```
   ./compressor
   ```
2. Follow the prompts:
   - Enter `c` to compress a file.
   - Enter `d` to decompress a file.
   - Enter any other key to exit.

### Compression

- Input the name of the file you want to compress.
- The compressed file will be saved with the `.huf` extension.

### Decompression

- Input the name of the `.huf` file you want to decompress.
- The decompressed file will be saved with the `.orig` extension.
- Remove the ".huf.orig" extension to get original file back.

## Example

### Compressing a File

```
Enter c to compress, d to decompress, or any other key to exit: c
File to compress: example.txt
```

Output:
```
Build Frequency Table took 0.01 seconds
Build Huffman Table took 0.02 seconds
Get Codes took 0.01 seconds
Number Nodes took 0.01 seconds
Calculate Avg Length took 0.01 seconds
Time to compress bytes took 0.05 seconds
Total Time taken: 0.11 seconds
File compressed
```

### Decompressing a File

```
Enter c to compress, d to decompress, or any other key to exit: d
File to decompress: example.txt.huf
```

Output:
```
Time to bytes to nodes took 0.01 seconds
Time to generate tree general took 0.02 seconds
Bytes to int took 0.01 seconds
decompress bytes took 0.05 seconds
Total Time taken: 0.09 seconds
File Decompressed
```

## Dependencies

- C++17 or later
- Threading support for multi-threaded operations

## Notes

- The project uses a custom Huffman tree implementation for encoding and decoding.
- Multi-threading is used to speed up the compression process by dividing the workload into chunks.

## License

This project is open-source and available under the MIT License.
