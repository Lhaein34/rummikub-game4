#include <iostream>
#include "shared/common.hpp"

int main() {
    // ASCII Art (numbers 1~13, 'Rummikub Solver', 'Game over')
    printAsciiArt();

    // 컬러 스타일 데모
    std::cout << "\n=== ANSI Color Demo ===\n";
    demoAnsiStyles();

    return 0;
}
