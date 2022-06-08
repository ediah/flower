#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include <map>
#include "runtime/vmachine.hpp"

class Debugger: public VirtualMachine {
    std::map<std::string, std::vector<std::string>> synonims;
    std::vector<std::string> lastcmd = {"help"};

    bool markMsg = false;

    static void printData(type_t type, void * data);
public:
    Debugger(): VirtualMachine() {
        synonims["quit"] = {"quit", "q"};
        synonims["step"] = {"step", "s"};
        synonims["help"] = {"help", "h"};
        synonims["inspect"] = {"inspect", "stack", "ins", "i"};
        synonims["memory"] = {"memory", "mem", "m"};
        synonims["registry"] = {"registry", "reg", "r"};
    };

    virtual void run(void) override;

    bool step(int * eip, int n);
    void help(std::vector<std::string> argv);
    bool prompt(int * eip);
    void inspect(std::vector<std::string> argv, const Stack * stack);
    void memory(void) const;
    bool compare(const std::string & str, std::string ref);
};

#endif