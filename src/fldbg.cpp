#include "version.hpp"
#include <iostream>
#include "debugger/debugger.hpp"

void help(void) {
    std::cout << "Отладчик Flower " << VERSION << std::endl;
    std::cout << "fldbg ИСПОЛНЯЕМЫЙ_ФАЙЛ" << std::endl;
}

int main(int argc, const char ** argv) {
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    if (argc != 2) {
        help();
        exit(-1);
    }

    Debugger deb;
    deb.loadBIN(argv[1]);
    deb.run();
}