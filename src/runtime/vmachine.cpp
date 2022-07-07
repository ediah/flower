#include <iostream>
#include <cstring>
#include <ostream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "runtime/vmachine.hpp"
#include "common/exprtype.hpp"
#include "common/obstacle.hpp"

void VirtualMachine::loadBIN(const char * filename) {
    std::ifstream bin;
    bin.open(filename, std::ios_base::binary | std::ios_base::in);

    bin.seekg(0, std::ios::end);
    int size = bin.tellg();
    base = new char [size];
    bin.seekg(0, std::ios::beg);
    bin.read(base, size * sizeof(char));

    cmd = base + *reinterpret_cast<int*>(base);
    bin.close();
    cmdNum = (size - *reinterpret_cast<int*>(base)) / (sizeof(int) + sizeof(bool));
}

void VirtualMachine::run(void) {
    op_t op;
    int * eip = new int (0);
    std::cout << std::boolalpha;

    while (true) {
        if (*reinterpret_cast<bool*>(cmd + 4 + *eip * 5)) { // ExecBit = 1
            op = (op_t) *reinterpret_cast<int *>(cmd + *eip * 5);
            if (exec(op, eip)) break;
        } else { // ExecBit = 0
            int shift = * reinterpret_cast<int *>(cmd + *eip * 5);
            stackVM.push(static_cast<void *>(base + shift));
        }
        *eip += 1;
    }

    if (inThread) {
        for (auto var: sharedVars) {
            write(pipefd.back()[1], &var.first, sizeof(void*));
            write(pipefd.back()[1], &var.second, sizeof(int));
            write(pipefd.back()[1], var.first, var.second);
        }

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

    for (auto memBlock: allocated) {
        free(memBlock);
    }

    delete eip;
}

template <class lval_t, class rval_t, class res_t>
void VirtualMachine::tempOp(res_t (*f) (lval_t, rval_t), type_t TYPE) {
    rval_t b = * static_cast<rval_t *>(stackVM.pop());
    lval_t a = * static_cast<lval_t *>(stackVM.pop());
    stackVM.push(new res_t ( f(a, b) ), TYPE);
}

template <class lval_t, class rval_t>
void VirtualMachine::assign(void) {
    if (inThread) {
        rval_t x = * static_cast<rval_t *>(stackVM.pop());
        lval_t * p = static_cast<lval_t *>(stackVM.pop());
        sharedVars[p] = sizeof(rval_t);
        *p = x;
    } else {
        rval_t x = * static_cast<rval_t *>(stackVM.pop());
        * static_cast<lval_t *>(stackVM.pop()) = x;
    }
}

char * VirtualMachine::getString(void * x) {
    return (static_cast<char **>( x )[0] == nullptr)
                ? static_cast<char *>( x ) + sizeof(char*)
                : * static_cast<char**>( x );
}

bool VirtualMachine::exec(op_t op, int * eip) {
    bool exitStatus = false;
    type_t rest = (type_t) ((op >> 24) & 0xFF);
    type_t lval = (type_t) ((op >> 16) & 0xFF);
    type_t rval = (type_t) ((op >>  8) & 0xFF);

    if ((lval == _STRUCT_) || (rval == _STRUCT_))
        throw Obstacle(PANIC);

    switch(op & 0xFF) {
        case PLUS:
            if (rest == _STRING_){
                char * b = getString(stackVM.pop());
                char * a = getString(stackVM.pop());

                int alen = 0;
                int blen = 0;
                for (; a[alen] != '\0'; ++alen) {}
                for (; b[blen] != '\0'; ++blen) {}

                char * c = new char [alen + blen + 1 + sizeof(char*)];
                memcpy(c, "\0\0\0\0\0\0\0\0", sizeof(char*));
                memcpy(c + sizeof(char*), a, alen);
                memcpy(c + alen + sizeof(char*), b, blen);
                c[alen + blen + sizeof(char*)] = '\0';
                stackVM.push(c);
                dynamicStrings.push(c, _STRING_);
            } else {
                ARITH_OPERATION(+)
            }
            break;
        case MINUS: ARITH_OPERATION(-) break;
        case MUL:   ARITH_OPERATION(*) break;
        case DIV:   ARITH_OPERATION(/) break;
        case MOD: {
            int b = * static_cast<int *>(stackVM.pop());
            int a = * static_cast<int *>(stackVM.pop());
            stackVM.push(new int (a % b), _INT_);
            break;
        }
        case INV:
            if (rest == _INT_) {
                int a = * static_cast<int *>(stackVM.pop());
                stackVM.push(new int (-a), _INT_);
            } else {
                float a = * static_cast<float *>(stackVM.pop());
                stackVM.push(new float (-a), _REAL_);
            }
            break;
        case LOAD: {
            int x = * static_cast<int *>(stackVM.pop());
            void * a = registerVM.get(x);
            stackVM.push(a);
            break;
        }
        case ASSIGN:
            if (lval == _INT_) {
                if (rval == _INT_) assign<int, int>();
                else assign<int, float>();
            } else if (lval == _REAL_) {
                if (rval == _INT_) assign<float, int>();
                else assign<float, float>();
            } else if (lval == _STRING_){
                char * b = getString(stackVM.pop());
                char * a = static_cast<char *>(stackVM.pop());
                memcpy(a, &b, sizeof(void*));
            } else if (lval == _BOOLEAN_) {
                assign<bool, bool>();
            } else throw Obstacle(PANIC);
            break;
        case STOP:
            exitStatus = true;
            break;
        case WRITE:
            if (rval == _INT_) {
                int x = * static_cast<int *>(stackVM.pop());
                std::cout << x;
            } else if (rval == _REAL_) {
                float x = * static_cast<float *>(stackVM.pop());
                std::cout << x;
            } else if (rval == _STRING_) {
                char * x = getString(stackVM.pop());
                std::cout << x;
            } else if (rval == _BOOLEAN_) {
                bool x = * static_cast<bool *>(stackVM.pop());
                std::cout << x;
            } else throw Obstacle(PANIC);
            break;
        case ENDL:
            std::cout << std::endl;
            break;
        case JIT: {
            int offset = * static_cast<int *>(stackVM.pop());
            bool condition = * static_cast<bool *>(stackVM.pop());
            if (condition) *eip = offset - 1;
            break;
        }
        case JMP: {
            int offset = * static_cast<int *>(stackVM.pop());
            *eip = offset - 1;
            break;
        }
        case LESS:
            if ((lval == _STRING_) && (rval == _STRING_)) {
                char * b = getString(stackVM.pop());
                char * a = getString(stackVM.pop());
                int i = 0;
                for (; (a[i] != '\0') && (b[i] != '\0'); i++) {}
                stackVM.push(new bool ((a[i] == '\0') && (b[i] != '\0')), _BOOLEAN_);
            } else {
                LOGIC_OPERATION(<)
            }
            break;
        case GRTR:
            if ((lval == _STRING_) && (rval == _STRING_)) {
                char * b = getString(stackVM.pop());
                char * a = getString(stackVM.pop());
                int i = 0;
                for (; (a[i] != '\0') && (b[i] != '\0'); i++) {}
                stackVM.push(new bool (b[i] == '\0'), _BOOLEAN_);
            } else {
                LOGIC_OPERATION(>)
            }
            break;
        case LESSEQ: LOGIC_OPERATION(<=) break;
        case GRTREQ: LOGIC_OPERATION(>=) break;
        case EQ:
            if ((lval == _STRING_) && (rval == _STRING_)) {
                char * b = getString(stackVM.pop());
                char * a = getString(stackVM.pop());
                bool r = true;
                int i = 0;
                for (; (a[i] != '\0') && (b[i] != '\0'); i++) {
                    if (a[i] != b[i]) {
                        r = false;
                        break;
                    }
                }
                r = r && (a[i] == '\0') && (b[i] == '\0');
                stackVM.push(new bool (r), _BOOLEAN_);
            } else {
                LOGIC_OPERATION(==)
            }
            break;
        case NEQ:
            if ((lval == _STRING_) && (rval == _STRING_)) {
                char * b = getString(stackVM.pop());
                char * a = getString(stackVM.pop());
                bool r = true;
                int i = 0;
                for (; (a[i] != '\0') && (b[i] != '\0'); i++) {
                    if (a[i] != b[i]) {
                        r = false;
                        break;
                    }
                }
                r = r && (a[i] == '\0') && (b[i] == '\0');
                stackVM.push(new bool (!r), _BOOLEAN_);
            } else {
                LOGIC_OPERATION(!=)
            }
            break;
        case READ:
            if (rval == _INT_) {
                float temp;
                std::cin >> temp;

                stackVM.updateType(_INT_);
                * static_cast<int *>(stackVM.pop()) = (int)temp;
            } else if (rval == _REAL_) {
                std::cin >> * static_cast<float *>(stackVM.pop());
            } else if (rval == _STRING_) {
                std::string x;
                std::cin >> x;
                char * newString = new char[x.length() + 1];
                memcpy(newString, x.data(), x.length() + 1);
                char * a = static_cast<char *>(stackVM.pop());
                const char * b = newString;
                memcpy(a, &b, sizeof(void*));
                dynamicStrings.push(newString, _STRING_);
            } else {
                std::string x;
                do { std::cin >> x; } 
                while ((x != "True") && (x != "true") && (x != "False") && (x != "false"));
                * static_cast<bool *>(stackVM.pop()) = (x == "True") || (x == "true");
            }
            break;

        case LAND: {
            bool b = * static_cast<bool *>(stackVM.pop());
            bool a = * static_cast<bool *>(stackVM.pop());
            stackVM.push(new bool (a && b), _BOOLEAN_);
            break;
        }
        case LOR: {
            bool b = * static_cast<bool *>(stackVM.pop());
            bool a = * static_cast<bool *>(stackVM.pop());
            stackVM.push(new bool (a || b), _BOOLEAN_);
            break;
        }
        case LNOT: {
            bool a = * static_cast<bool *>(stackVM.pop());
            stackVM.push(new bool (!a), _BOOLEAN_);
            break;
        }
        case CALL: {
            int offset = * static_cast<int *>(stackVM.pop());
            int param  = * static_cast<int *>(stackVM.pop());
            for (int i = 0; i < param; i++) {
                registerVM.push( stackVM.pop() );
            }
            params.push(new int (param), _INT_);
            offsets.push(new int (*eip + 1), _INT_);
            *eip = offset - 1;
            stackVM.lock();
            break;
        }
        case RET: {
            int offset = * static_cast<int *>(offsets.pop());
            int param  = * static_cast<int *>(params.pop());
            while (param--) registerVM.pop();
            *eip = offset - 1;
            break;
        }
        case FORK: {
            pipefd.push_back(new int[2]);
            pipe(pipefd.back());

            pid_t pid = fork();
            threads.push_back(pid);

            if (pid == -1) throw Obstacle(PANIC);
            if (pid == 0) {
                inThread = true;
                int offset = * static_cast<int *>(stackVM.pop());
                *eip = offset - 1;
            } 
            else {
                //int st = 0;
                //waitpid(pid, &st, WNOHANG);
                #ifdef DEBUG
                std::cout << "Запущен новый поток с PID = " << pid << std::endl;
                #endif
            }
            break;
        }
        case LOCK: {
            while (threads.size() != 0) {
                int st, ret = 0;
                pid_t pid = threads.back();
                ret = waitpid(pid, &st, 0);
                if (WIFEXITED(st)) threads.pop_back();
                else throw Obstacle(PANIC);
            }
            updateVars();
            break;
        }
        case UNPACK: {
            int fieldSize = * static_cast<int *>(stackVM.pop());
            for (int i = 0; i < fieldSize * 2; i++) {
                registerVM.push(stackVM.pop());
            }
            for (int i = 0; i < fieldSize; i++) {
                stackVM.push(registerVM.get(i));
                stackVM.push(registerVM.get(i + fieldSize));
            }
            for (int i = 0; i < fieldSize * 2; i++) {
                registerVM.pop();
            }
            break;
        }
        case DEREF: {
            int shift = * static_cast<int *>(stackVM.pop());
            char * basePoint = * static_cast<char **>(stackVM.pop());
            auto x = basePoint + typeSize(lval) * shift;
            //std::cout << "Вычислен адрес " << static_cast<void*>(x) << std::endl;
            stackVM.push(x);
            break;
        }
        case ALLOC: {
            int size = * static_cast<int *>(stackVM.pop());
            void** basePoint = static_cast<void**>(stackVM.pop());
            void * newMem = malloc(size);
            allocated.push_back(newMem);
            *basePoint = newMem;
            //std::cout << "Выдлена память в " << newMem << ", запись в " << basePoint << std::endl;
            break;
        }
        default:
            std::cout << "Неизвестная команда." << std::endl;
            exit(-1);
    }

    return exitStatus;
}

void VirtualMachine::updateVars(void) {
    void * p;
    int n;
    while (pipefd.size() != 0) {
        read(pipefd.back()[0], &p, sizeof(void *));
        if (p == nullptr) {
            delete[] pipefd.back();
            pipefd.pop_back();
            continue;
        }
        read(pipefd.back()[0], &n, sizeof(int));
        read(pipefd.back()[0], p, n);
    }
}

VirtualMachine::~VirtualMachine(void) {
    delete [] base;
    dynamicStrings.burn();
}