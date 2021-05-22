/* Автоматизация тестирования MLC компилятора. */

#include <iostream>
#include <fstream>
#include <cstdlib>

int runA(std::string filename, std::string input, std::string output) {
    

}


int runB(std::string filename, std::string output) {
    int ret = 0;
    std::cout << "[B-unit ";
    std::system( ("./mlc -c -i " + filename + " > a.out").data() );
    std::ifstream log("a.out");
    std::ifstream expected(output);
    while (!log.eof() && !expected.eof()) {
        std::string line1, line2;
        log >> line1;
        expected >> line2;
        if (line1 != line2) {
            ret = 1;
            break;
        }
    }

    if (!(log.eof() && expected.eof()))
        ret = 1;
    
    if (ret == 1) {
        std::cout << "ERROR ]: " << filename << std::endl;
    } else {
        std::cout << "  OK  ]: " << filename << std::endl;
    }

    log.close();
    expected.close();
    std::system("rm ./a.out");
    return ret;
}


int main(int argc, char ** argv) {
    int exitCode = 0;

    for (int i = 1; i < argc; i++) {
        std::string filename = argv[i];
        bool unitA = filename.find("/A-unit/") != std::string::npos;
        bool unitB = filename.find("/B-unit/") != std::string::npos;

        if (!unitA && !unitB) {
            std::cout << "Не удалось распознать группу теста.\n";
            exit(1);
        }

        std::size_t shift;
        if (unitA) shift = filename.find("/A-unit/");
        if (unitB) shift = filename.find("/B-unit/");

        std::string path = filename.substr(0, shift + 1);
        std::string source = filename.substr(shift);
        source = source.substr(0, source.find("."));
        std::string output = path + "output" + source + ".out";

        if (unitA) {
            std::string input = path + "input" + source + ".in";
            exitCode += runA(filename, input, output);
        } else exitCode += runB(filename, output);

    }

    std::cout << "\nПройдено " << argc - 1 << " тестов, из них:\n\t";
    std::cout << argc - 1 - exitCode << " успешно\n\t" << exitCode;
    std::cout << " с ошибкой\n";

    return exitCode;
}