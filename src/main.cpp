#include <fstream>
#include <iostream>

#include "Club/Club.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    club::Club club(argv[1]);
    return 0;
}
