#include <iostream>
#include <filesystem>
#include <fstream>
#include "num.h"
#include "cpu.h"

#define endl std::endl

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <file>" << endl;
        return 1;
    }

    std::filesystem::path path(argv[1]);
    if(!std::filesystem::exists(path)) {
        std::cout << "File does not exist" << endl;
        return 1;
    }

    // Read file into buffer
    u8* buffer = (u8*)malloc(std::filesystem::file_size(path));

    std::ifstream file(path, std::ios::binary);
    file.read((char*)buffer, std::filesystem::file_size(path));

    // Run CPU
    initCpu(buffer, std::filesystem::file_size(path));
    runCpu();

    return 0;
}