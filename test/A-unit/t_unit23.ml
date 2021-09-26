def iterate(shared int a): shared int {
    for (int i = 1; i <= 5; i = i + 1) {
        write("Итерация #", i, ": a = ", a);
    }
    return a;
}

program {
    shared int x = 900, y = 800;

    thread:1 { x = iterate(x); }
    thread:2 { y = iterate(y); }

    employ_threads(1, 2);

    write(x, " ", y);
}