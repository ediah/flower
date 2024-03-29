#include <unistd.h>
#include <iomanip>
#include <algorithm>
#include "debugger/debugger.hpp"
#include "common/exprtype.hpp"

void Debugger::help(std::vector<std::string> argv) {
    if (argv.size() == 1) {
        std::cout << "Возможные команды:" << std::endl;
        for (auto pair: synonims)
            std::cout << pair.first << ", ";
        std::cout << "\b\b." << std::endl;
        return;
    }

    if (compare(argv[1], "inspect")) {
        std::cout << "stack/inspect/ins/i [<n> <type>]-- ";
        std::cout << "показать содержимое стека." << std::endl;
        std::cout << "Если задаются аргументы, выводит содержимое ";
        std::cout << "n-той ячейки, предполагая тип type." << std::endl;
    }
}

bool Debugger::step(int * eip, int n) {
    op_t op;

    for (int i = 0; i < n; i++) {
        if (*reinterpret_cast<bool*>(cmd + 4 + *eip * 5)) { // ExecBit = 1
            op = (op_t) *reinterpret_cast<int*>(cmd + *eip * 5);
            debugOp((operation_t) (op & 0xFF));
            std::cout << std::endl;
            if (exec(op, eip)) return false;
            if ((operation_t) (op & 0xFF) == READ)
                while ((getchar()) != '\n');
        } else { // ExecBit = 0
            int shift = *reinterpret_cast<int*>(cmd + *eip * 5);
            stackVM.push(static_cast<void *>(base + shift));
        }
        *eip += 1;
    }

    return true;
}

bool Debugger::compare(const std::string & str, const std::string ref) {
    auto equals = [str](const std::string &r){return r == str;};
    return std::any_of(synonims[ref].cbegin(), synonims[ref].cend(), equals);
}

void Debugger::printData(type_t type, void * data) {
    switch (type) {
        case _INT_:
            std::cout << * static_cast<int *>(data);
            break;
        case _REAL_:
            std::cout << * static_cast<float *>(data);
            break;
        case _STRING_:
            std::cout << getString(data);
            break;
        case _BOOLEAN_:
            std::cout << * static_cast<bool *>(data);
            break;
        default:
            break;
    }
}

void Debugger::inspect(std::vector<std::string> argv, const Stack * stack) {
    if ((argv.size() != 1) && (argv.size() != 3)) {
        help({"help", "inspect"});
        return;
    }

    const type_t * types = stack->getTypes();
    void * const * data = stack->data();

    if (argv.size() == 1) {
        int size = stack->size();
        for (int i = 0; i < size; i++) {
            std::cout << i << ") " << typetostr(types[i]) << " ";
            printData(types[i], data[i]);
            std::cout << std::endl;
        }
    } else {
        int pos = std::stoi(argv[1]);
        type_t mtype;
        
        if (argv[2] == "int") mtype = _INT_;
        else if (argv[2] == "real") mtype = _REAL_;
        else if (argv[2] == "string") mtype = _STRING_;
        else if (argv[2] == "bool") mtype = _BOOLEAN_;
        else {
            std::cout << "Нельзя выбрать тип " << argv[2];
            std::cout << "." << std::endl;
            return;
        }

        std::cout << typetostr(mtype) << " ";
        printData(mtype, data[pos]);
        std::cout << std::endl;
    }
}

void Debugger::memory(void) const {
    for (char * p = base; p < cmd + 8; p = p + 4) {
        if ((base != p) && ((p - base) % (4 * 5) == 0))
            std::cout << std::endl;
        std::cout << "0x" << std::hex << std::setfill('0') << std::setw(8)
                  << * reinterpret_cast<int *>(p) << std::ends << " ";
    }
    std::cout << std::endl;
}

bool Debugger::prompt(int * eip) {
    std::string cmd;

    std::cout << "fldbg> ";
    std::getline(std::cin, cmd);


    std::vector<std::string> argv;

    bool lastWasW = true;
    std::string::iterator wstart;
    for (auto it = cmd.begin(); it != cmd.end(); ++it) {
        if (*it == ' ') {
            if (!lastWasW) {
                int pos = std::distance(cmd.begin(), wstart);
                int n = std::distance(wstart, it);
                argv.push_back(cmd.substr(pos, n));
            }
            lastWasW = true;
        } else {
            if (lastWasW) wstart = it;
            lastWasW = false;
        }
    }

    if (!lastWasW) {
        int pos = std::distance(cmd.begin(), wstart);
        int n = std::distance(wstart, cmd.end());
        argv.push_back(cmd.substr(pos, n));
    }

    if (argv.size() == 0)
        argv = lastcmd;
    else lastcmd = argv;

    if (compare(argv[0], "step"))
        return step(eip, 1);
    else if (compare(argv[0], "help"))
        help(argv);
    else if (compare(argv[0], "inspect"))
        inspect(argv, &stackVM);
    else if (compare(argv[0], "registry"))
        inspect(argv, &registerVM);
    else if (compare(argv[0], "memory"))
        memory();
    else if (compare(argv[0], "quit"))
        return false;
    

    return true;
}

void Debugger::run(void) {
    int * eip = new int (0);
    std::cout << std::boolalpha;

    while (prompt(eip));

    if (inThread) {
        void * nullp = nullptr;
        write(pipefd.back()[1], &nullp, sizeof(void*));
        #ifdef DEBUG
        std::cout << "Передал отцовскому процессу нуль." << std::endl;
        #endif
    }

    #ifdef DEBUG
    std::cout << "Машина завершила свою работу." << std::endl;
    #endif

    while (pipefd.size() != 0) {
        delete[] pipefd.back();
        pipefd.pop_back();
    }

    delete eip;
}