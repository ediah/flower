program {
    int a = 5;

    while (a > 0) {
        if (a >= 5)
            write(a, " конфеток осталось...");
        else if (a > 1)
            write(a, " конфетки осталось...");
        else
            write(a, " конфетка осталась...");
        a = a - 1;
    }

    write("Конфеток не осталось!");
}
