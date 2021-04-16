#include <iostream>
#include "lexer.hpp"
#include "vmachine.hpp"

int main(int argc, char** argv) {
    std::string filename = "unit1.ml";

    if (argc == 2) {
        filename = argv[1];
    }

    Lexer l;
    l.load("./test/" + filename);

    l.parse();
    l.finalize();
    l.giveBIN("out.bin");

    VirtualMachine vm;

    vm.loadBIN("out.bin");

    vm.run();

    return 0;
}
