/* Автоматизация тестирования MLC компилятора. */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

int compare(std::ifstream & file1, std::ifstream & file2) {
    int ret = 0;
    std::string line1, line2;
    while (!file1.eof()) {
        file1 >> line1;
        file2 >> line2;
        //std::cout << "[" << line1 << "] [" << line2 << "]\n";
        if (!file1.eof()) {
            if (line1 != line2) {ret = 1; break;}
        } else {
            if (line2 != "[ENDCASE]") {
                while ((line2 != "[ENDCASE]") && (!file2.eof()))
                    file2 >> line2;
                    
                ret = 1;
            }
        }
    }

    return ret;
}

int runA(std::string filename, std::string input, std::string output) {
    int caseIterator = 0, errorIterator = 0;
    std::cout << "[A-unit ";
    std::system( ("./mlc -c -i " + filename + " -s -o test.bin > a.out").data() );
    std::ifstream log("a.out");
    std::ifstream expected(output);

    // Проверка успешной компиляции
    std::string line;
    log >> line;
    log.close();
    if (line != "КОМПИЛЯЦИЯ:") errorIterator = 1;
    else {
        std::ifstream cases(input);
        cases >> line;
        while (line == "case") {
            cases >> line;
            if (line != "{") {
                std::cout << "Некорректный тест: " << input << std::endl;
                break;
            } else {
                char c;
                std::ofstream singleCase("case.in");
                cases.read(&c, sizeof(char));
                while ((c != '}') && !cases.eof()) {
                    singleCase.write(&c, sizeof(char));
                    cases.read(&c, sizeof(char));
                }
                singleCase.close();

                std::system("./mlc -r -s -i test.bin > a.out < case.in");
                std::system("rm ./case.in");
                std::ifstream actual("a.out");
                errorIterator += compare(actual, expected);
                actual.close();
            }
            cases >> line;
            caseIterator++;
        }
        cases.close();
    }

    if ((errorIterator != 0) || (caseIterator == 0))
        std::cout << "ERROR ]: ";
    else
        std::cout << "  OK  ]: ";

    std::cout << caseIterator - errorIterator << "/" << caseIterator << " ";
    std::cout << filename;

    expected.close();

    std::system("rm ./a.out ./test.bin");
    
    if (caseIterator == 0) {
        errorIterator = -1;
        std::cout << " [Тесты не найдены]";
    } else errorIterator = errorIterator != 0 ? 1 : 0;

    std::cout << std::endl;

    return errorIterator;
}


int runB(std::string filename, std::string output) {
    int ret = 0;
    std::cout << "[B-unit ";
    std::system( ("./mlc -c -i " + filename + " > a.out").data() );
    std::ifstream log("a.out");
    std::ifstream expected(output);
    
    ret = compare(log, expected);
    
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
    int errors = 0, notFound = 0;

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
            int x = runA(filename, input, output);
            if (x > 0) errors += x;
            else notFound -= x;
        } else errors += runB(filename, output);

    }

    std::cout << "\nПройдено " << argc - 1 << " тестов, из них:\n\t";
    std::cout << argc - 1 - errors - notFound << " успешно\n\t" << errors;
    std::cout << " с ошибкой\n\t" << notFound << " тестов не было запущено\n";

    return errors;
}