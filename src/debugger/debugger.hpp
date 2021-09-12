#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include "runtime/vmachine.hpp"

class Debugger: public VirtualMachine {
public:
    virtual void run(void) override;
};

#endif