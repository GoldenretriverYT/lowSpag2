#include <iostream>
#include <filesystem>
#include <fstream>
#include "num.h"
#include "cpu.h"
#include "MiniFB.h"

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

    // Init window
    struct mfb_window *window = mfb_open_ex("my display", 800, 600, WF_RESIZABLE);
    if (!window)
        return 0;

    // Init CPU
    initCpu(buffer, std::filesystem::file_size(path));

    // Init window frame buffer
    u32* frameBuffer = (u32*) malloc(800 * 600 * 4);

    do {
        int state;

        // TODO: add some fancy rendering to the buffer of size 800 * 600

        state = mfb_update_ex(window, frameBuffer, 800, 600);

        if (state < 0) {
            window = NULL;
            return 0;
        }

        // Run 10000 instructions per frame
        for(int i = 0; i < 100; i++) {
            for(int i = 0; i < 100; i++) {
                if(registers[PROGRAM_COUNTER_REGISTER] < RAM_TOTAL_SIZE - 8) {
                    runInstruction();
                }
            }

            std::cout << std::flush; // we flush every 100 instructions
        }
    } while(mfb_wait_sync(window));

    return 0;
}