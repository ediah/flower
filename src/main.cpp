#define VERSION "v1.2.0 (Календула)"

#include <iostream>
#include <chrono>
#include "compiler/parser.hpp"
#include "runtime/vmachine.hpp"
#include "debugger/debugger.hpp"

struct flags_t {
    bool debug;    // -d
    bool optimize; // -O
    bool compile;  // -c
    bool run;      // -r
    bool poliz;    // -p
    bool verbose;  // -v
    bool infile;   // -i
    bool outfile;  // -o
};

void help(void) {
    std::cout << "Компилятор Модельного Языка Программирования " << VERSION << std::endl;
    std::cout << "Флаги командной строки:\n\t-c\tКомпиляция\n\t-r\tВыполнение\n\t";
    std::cout << "-d\tОтладка\n\t-O\tОптимизация\n\t-p\tПечатать ПОЛИЗ\n\t";
    std::cout << "-v\tВыводить сообщения\n\t";
    std::cout << "-i\tУказать входной файл\n\t-o\tУказать выходной файл\n";
}

bool compile(flags_t options, std::string ifname, std::string ofname) {
    auto start = std::chrono::steady_clock::now();

    Parser pworker;
    pworker.load(ifname);
    bool ok = pworker.parse();

    if (ok) {
        pworker.giveBIN(ofname.data(), options.optimize, options.poliz, options.verbose);

        auto end = std::chrono::steady_clock::now();
        auto diff = end-start;
        int time = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        std::cout << "КОМПИЛЯЦИЯ: " << time << " мс"<< std::endl;
    } else {
        std::cout << "ОШИБКА КОМПИЛЯЦИИ" << std::endl;
    }
    return ok;
}

void run(flags_t options, std::string ifname) {
    auto start = std::chrono::steady_clock::now();

    if (options.debug) {
        Debugger deb;
        deb.loadBIN(ifname.data());
        deb.run();
    } else {
        VirtualMachine vm;
        vm.loadBIN(ifname.data());
        vm.run();
    }

    if (options.verbose) {
        auto end = std::chrono::steady_clock::now();
        auto diff = end-start;
        int time = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        std::cout << "\nВРЕМЯ РАБОТЫ: " << time << " мс"<< std::endl;
    }
}

int main(int argc, const char** argv) {
    struct flags_t flags = {false, false, false, false, false, false, false, false};
    std::string flag, inname, outname;

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            flag = argv[i];

            if (flag[0] == '-') {
                switch (flag[1]) {
                    case 'd':
                        flags.debug = true;
                        flags.run = true;
                        break;
                    case 'O':
                        flags.optimize = true;
                        flags.compile = true;
                        break;
                    case 'i': 
                        flags.infile = true;
                        inname = argv[++i]; 
                        break;
                    case 'o': 
                        flags.outfile = true;
                        outname = argv[++i]; 
                        break;
                    case 'p': flags.poliz = true; break;
                    case 'v': flags.verbose = true; break;
                    case 'c': flags.compile = true; break;
                    case 'r': flags.run = true; break;
                    default:
                        std::cout << flag << std::endl;
                    case 'h':
                        help();
                        exit(0);
                }
            } else {
                help();
                exit(-1);
            }
        }
    }
        
    if (flags.compile) {
        if (!flags.infile) {
            std::cout << "Не указан входной файл." << std::endl;
            exit(-1);
        }

        if (!flags.outfile) {
            flags.outfile = true;
            outname = "out.bin";
        }

        bool ok = compile(flags, inname, outname);
        flags.run = flags.run && ok;

        if (flags.run) {
            flags.infile = true;
            inname = outname;
        }
    } 

    if (flags.run) {
        if (!flags.infile) {
            flags.infile = true;
            inname = "out.bin";
        }

        run(flags, inname);
    }

    return 0;
}
