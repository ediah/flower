program {
    int a;
    bool b;
    read(a);

    b = a % 2 == 0;

    if (b)
        write("Чётное число!");
    else
        write("Нечётное число!");

    write("a % 2 == 0 = ", b);

}
