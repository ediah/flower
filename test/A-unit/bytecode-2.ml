def func(int a): int {
    bytecode {
        a, 2, MUL, RET
    }
    return -1;
}

program {
    int x;
    bytecode {
        x,
        2, 1, func, CALL,
        ASSIGN
    }
    write(x);
}