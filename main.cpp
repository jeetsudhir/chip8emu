#include "screen.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: chip8 <ROM file>" << std::endl;
        return 1;
    }
    Screen screen(1024, 512, argv[1]);
    screen.loadROM(argv[1]);
    screen.run();

    return 0;
}
