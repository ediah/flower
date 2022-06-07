#include "version.hpp"
#include <iostream>
#include "runtime/vmachine.hpp"

void help(void) {
    std::cout << "Виртуальная машина Flower " << VERSION << std::endl;
    std::cout << "flvm ИСПОЛНЯЕМЫЙ_ФАЙЛ" << std::endl;
}

int main(int argc, const char ** argv) {
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    if (argc != 2) {
        help();
        exit(-1);
    }

    VirtualMachine vm;
    vm.loadBIN(argv[1]);
    vm.run();

    return 0;
}