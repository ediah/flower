/* Автоматизация тестирования компилятора Flower. */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <chrono>
#include <ctime>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "version.hpp"

std::string gettime(void) {
    using namespace std::chrono;
    auto timepoint = system_clock::now();
    auto h_m_s = system_clock::to_time_t(timepoint);
    auto ms = time_point_cast<std::chrono::milliseconds>(timepoint);

    char buffer[sizeof "23:59:59.999"];
    std::snprintf(buffer + std::strftime(buffer, sizeof buffer - 3,
        "%T.", std::localtime(&h_m_s)), 4, "%03lu", 
        ms.time_since_epoch().count() % 1000);
    return buffer;
}

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

int checkMem(std::string cmd) {
    bool memLeak = true, memError = true;
    std::system( ("valgrind --log-file=\"a.out\" " + cmd).data() );
    std::ifstream log("a.out");
    std::string line;
    while (! log.eof()) {
        log >> line;
        if (line == "All") {
            char * tempbuf = new char[24];
            log.readsome(tempbuf, 23);
            tempbuf[23] = '\0';
            std::string temp(tempbuf);
            if (temp.find(" heap blocks were freed") != std::string::npos)
                memLeak = false;
            delete [] tempbuf;
        }
        
        if (line == "ERROR") {
            char * tempbuf = new char[24];
            log.readsome(tempbuf, 23);
            tempbuf[23] = '\0';
            std::string temp(tempbuf);
            if (temp.find(" SUMMARY: 0 errors") != std::string::npos)
                memError = false;
            delete [] tempbuf;
        }
    }
    std::cout << "[ " << (memLeak ? "УТЕЧКА" : "  ОК  ") << " ; ";
    std::cout << (memError ? "ОШИБКА" : "  ОК  " ) << " ] ";

    log.close();

    return (int)memLeak + (int)memError;
}

int genCaseIn(std::ifstream & cases) {
    int ret = 0;
    std::string line;
    cases >> line;
    if (line != "{")  ret = 1;
    else {
        char c;
        std::ofstream singleCase("case.in");
        cases.read(&c, sizeof(char));
        while ((c != '}') && !cases.eof()) {
            singleCase.write(&c, sizeof(char));
            cases.read(&c, sizeof(char));
        }
        singleCase.close();
    }
    return ret;
}

int runValgrind(std::string filename, bool optimize, std::string input = "") {
    int ret = 0;

    ret += checkMem("./flc " + std::string(optimize ? "-O " : "") + 
                    filename + " test.bin > /dev/null");
    std::cout << filename << std::endl;

    if (input != "") {
        int caseIterator = 0;
        std::ifstream cases(input);
        std::string line;
        cases >> line;
        while (line == "case") {
            if (genCaseIn(cases) != 0)
                break;
            ret += checkMem("./flvm test.bin > /dev/null < case.in");
            std::system("rm ./case.in");
            std::cout << "Case #" << caseIterator + 1 << std::endl;
            cases >> line;
            caseIterator++;
        }
        cases.close();
    }
    std::system("rm -f ./a.out ./test.bin");

    return (ret != 0) ? 1 : 0;
}

int runA(std::string filename, std::string input, std::string output, bool optimize) {
    int caseIterator = 0, errorIterator = 0;
    bool compileError = false;
    std::cout << "[A-unit ";
    std::system( ("./flc " + std::string(optimize ? "-O " : "") +
                  filename + " test.bin > a.out").data() );
    std::ifstream log("a.out");
    std::ifstream expected(output);

    // Проверка успешной компиляции
    std::string line;
    log >> line;
    log.close();
    //#ifndef DEBUG
    if (line != "КОМПИЛЯЦИЯ:") {
        errorIterator = 1;
        compileError = true;
    } else {
    //#endif
        std::ifstream cases(input);
        cases >> line;
        while (line == "case") {
            if (genCaseIn(cases) != 0)
                break;
            //#ifdef DEBUG
            //std::system("gdb --args ./flc -s -i test.bin");
            //#else
            std::system("./flvm test.bin > a.out < case.in");
            //#endif
            std::system("rm ./case.in");
            std::ifstream actual("a.out");
            int err = compare(actual, expected);
            if (err != 0) {
                std::system("cat ./a.out");
                errorIterator += err;
            }
            actual.close();

            cases >> line;
            caseIterator++;
        }
        cases.close();
    //#ifndef DEBUG
    }
    //#endif

    if ((errorIterator != 0) || (caseIterator == 0))
        std::cout << "ОШИБКА ]: ";
    else
        std::cout << "  OK  ]: ";

    std::cout << caseIterator - errorIterator << "/" << caseIterator << " ";
    std::cout << filename;

    expected.close();

    if (!compileError)
        std::system("rm ./a.out ./test.bin");
    
    if (caseIterator == 0) {
        if (compileError) {
            std::cout << " [ Ошибка компиляции ]";
            errorIterator = -1;
        } else {
            std::cout << " [ Тесты не найдены ]";
            errorIterator = -2;
        }
    } else errorIterator = errorIterator != 0 ? 1 : 0;

    std::cout << std::endl;

    return errorIterator;
}


int runB(std::string filename, std::string output, bool optimize) {
    int ret = 0;
    std::cout << "[B-unit ";
    std::system( ("./flc " + std::string(optimize ? "-O " : "") +
                  filename + " > a.out").data() );
    std::ifstream log("a.out");
    std::ifstream expected(output);
    
    ret = compare(log, expected);
    
    if (!(log.eof() && expected.eof()))
        ret = 1;

    if (ret == 1) {
        std::cout << "ОШИБКА ]: " << filename << std::endl;
    } else {
        std::cout << "  OK  ]: " << filename << std::endl;
    }

    log.close();
    expected.close();
    std::system("rm -f ./out.bin ./a.out");
    return ret;
}

void rebuild(const char * pattern, bool make = true) {
    std::system("cp ./Makefile ./temp");
    std::system( ("sed -e 's/" + std::string(pattern) + "/g' ./temp > ./Makefile").data() );
    std::system("rm ./temp");
    if (make)
        std::system("make -s clean; make -s");
}

int main(int argc, char ** argv) {
    int errors = 0, notFound = 0;
    bool mem = false;
    bool opt = false;
    bool bisect = false;
    bool restore = false;
    bool coverage = false;

    std::cout << "Автоматизация тестирования Flower " << VERSION << std::endl;

    for (int i = argc; i > 1; i--) {
        if (argv[i - 1][0] == '-') {
            switch (argv[i - 1][1]) {
                case 'm':
                    mem = true;
                    break;
                case 'O':
                    opt = true;
                    break;
                case 'f':
                    bisect = true;
                    break;
                case 'r':
                    restore = true;
                    break;
                case 'c':
                    coverage = true;
                    break;
                default:
                    throw std::runtime_error("Неизвестный флаг.");
            }
        }
    }

    std::vector<std::string> filesWithError, filesWithCompileError, testsNotFoundError;

    #ifdef DEBUG
    std::cout << "Собрано с отладочной информацией, пересборка.\n";
    rebuild("RELEASE=NO/RELEASE=YES");
    #endif

    #ifndef TCOV
    if (coverage) {
        std::cout << "Пересборка для анализа покрытия.\n";
        rebuild("COVERAGE=NO/COVERAGE=YES");
    }
    #else
    if (!coverage) {
        std::cout << "Пересборка без анализа покрытия.\n";
        rebuild("COVERAGE=YES/COVERAGE=NO");
    }
    #endif

    if (bisect) std::system("rm ./bin/*; make flc flvm");

    auto start = std::chrono::system_clock::now();
    
    std::cout << "Запуск тестирования" << (mem? " с Valgrind" : "");
    std::cout << " в " << gettime() << "\n\n";

    int flags = (int)mem + (int)opt + (int)bisect + (int)restore + (int)coverage;
    for (int i = flags + 1; i < argc; i++) {
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

        int x;

        if (unitA) {
            std::string input = path + "input" + source + ".in";
            if (mem) x = runValgrind(filename, opt, input);
            else x = runA(filename, input, output, opt);
        } else 
            if (mem) x = runValgrind(filename, opt); 
            else x = runB(filename, output, opt);

        if (x > 0) {
            errors += x;
            filesWithError.push_back(filename);
        } else if (x < 0) {
            notFound += 1;
            if (x == -1)
                filesWithCompileError.push_back(filename);
            else
                testsNotFoundError.push_back(filename);
        }

        if ((errors != 0) && bisect) break;
    }

    
    if (restore) {
        #if defined(DEBUG) || defined(TCOV)
        std::cout << "Возврат параметров Makefile...\n";
        #endif

        #if defined(DEBUG) && !defined(TCOV)
        rebuild("RELEASE=YES/RELEASE=NO", false);
        #endif

        #if defined(TCOV)
        rebuild("COVERAGE=YES/COVERAGE=NO");
        #endif
    }

    int all = argc - (flags + 1);
    std::cout << "\nПройдено " << all << " тестов, из них:\n\t";
    std::cout << all - errors - notFound << " успешно\n\t" << errors;
    std::cout << " с ошибкой\n\t" << notFound << " тестов не было запущено\n";

    if (filesWithError.size() > 0)
        std::cout << "\nОШИБКА НА ТЕСТАХ:\n";

    for (std::string ferr : filesWithError)
        std::cout << "\t" << ferr << "\n";

    if (filesWithCompileError.size() > 0)
        std::cout << "\nОШИБКА КОМПИЛЯЦИИ НА ТЕСТАХ:\n";

    for (std::string ferr : filesWithCompileError)
        std::cout << "\t" << ferr << "\n";

    if (testsNotFoundError.size() > 0)
        std::cout << "\nТЕСТЫ НЕ НАЙДЕНЫ:\n";

    for (std::string ferr : testsNotFoundError)
        std::cout << "\t" << ferr << "\n";

    auto end = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "\nТест завершён в " << gettime() << "\n";
    std::cout << "Прошло " << (float) diff / 1000 << " секунд.\n";

    return errors + notFound;
}