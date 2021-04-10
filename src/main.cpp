#include <iostream>
#include "lexer.hpp"
#include "vmachine.hpp"

int main() {

    Lexer l;
    l.load("./test/unit1.ml");

    l.parse();
    l.finalize();
    l.giveBIN("out.bin");

    VirtualMachine vm;

    vm.loadBIN("out.bin");

    vm.run();

    return 0;
}
