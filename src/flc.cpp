#include "version.hpp"
#include <iostream>
#include <chrono>
#include "compiler/parser.hpp"

struct flags_t {
    bool optimize; // -O
    bool poliz;    // -p
    bool verbose;  // -v
    bool infile;   
    bool outfile; 
};

void help(void) {
    std::cout << "Компилятор языка программирования Flower " << VERSION << std::endl;
    std::cout << "flc [флаги] ВХОДНОЙ_ФАЙЛ [ВЫХОДНОЙ_ФАЙЛ]";
    std::cout << "Флаги командной строки:\n\t-O\tОптимизировать\n\t-p\tПечатать ПОЛИЗ\n\t";
    std::cout << "-v\tВыводить сообщения\n";
}

void compile(flags_t options, const std::string &ifname, const std::string &ofname) {
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
}

int main(int argc, const char** argv) {
    struct flags_t flags = {false, false, false, false, false};
    std::string inname, outname;

    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                switch (argv[i][1]) {
                    case 'O':
                        flags.optimize = true;
                        break;
                    case 'p': flags.poliz = true; break;
                    case 'v': flags.verbose = true; break;
                    default:
                        std::cout << argv[i] << std::endl;
                    case 'h':
                        help();
                        exit(0);
                }
            } else {
                if (!flags.infile) {
                    flags.infile = true;
                    inname = argv[i]; 
                } else if (!flags.outfile){
                    flags.outfile = true;
                    outname = argv[i]; 
                } else {
                    help();
                    exit(1);
                }
            }
        }
    }
        
    if (!flags.infile) {
        std::cout << "Не указан входной файл." << std::endl;
        exit(-1);
    }

    if (!flags.outfile) {
        flags.outfile = true;
        outname = "out.bin";
    }

    compile(flags, inname, outname);

    return 0;
}
