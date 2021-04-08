#include <iostream>
#include "lexer.hpp"

int main() {
    Lexer l;
    l.load("./test/unit1.ml");

    l.parse();
    l.finalize();

    return 0;
}
