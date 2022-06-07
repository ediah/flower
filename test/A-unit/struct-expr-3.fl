struct bits {
    bool a, b, c, d;
}

program {
    struct bits x = {a = false, b = false, c = true, d = true};
    struct bits y = {a = false, b = true, c = false, d = true};
    struct bits m, n;

    m = x or y;
    n = x and y;

    write(m.a, " ", m.b, " ", m.c, " ", m.d);
    write(n.a, " ", n.b, " ", n.c, " ", n.d);
}