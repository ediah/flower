program {
    int x;
    goto BYTE;
OK:
    write("OK");
    goto END;
ERROR:
    write("ERROR");
    goto END;
BYTE:
    bytecode {
        x, 2, ASSIGN,
        x, 2, EQ, OK, JIT,
        ERROR, JMP
    }
END:
    write(x);
}