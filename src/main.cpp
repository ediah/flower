#include <iostream>
#include <chrono>
#include "parser.hpp"
#include "vmachine.hpp"

int main(int argc, char** argv) {
    std::string filename = "unit1.ml";

    if (argc == 2) {
        filename = argv[1];

        auto start = std::chrono::steady_clock::now();

        Parser pworker;
        pworker.load("./test/" + filename);

        pworker.parse();
        pworker.finalize();
        pworker.giveBIN("out.bin");

        auto end = std::chrono::steady_clock::now();
        auto diff = end-start;
        int time = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        std::cout << "КОМПИЛЯЦИЯ: " << time << " мс"<< std::endl;
    } else {
        auto start = std::chrono::steady_clock::now();

        VirtualMachine vm;

        vm.loadBIN("out.bin");

        vm.run();

        auto end = std::chrono::steady_clock::now();
        auto diff = end-start;
        int time = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        std::cout << "\nВРЕМЯ РАБОТЫ: " << time << " мс"<< std::endl;
    }



    return 0;
}
