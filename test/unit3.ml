program {
    int n = 10;
    real x, r = 1.0, p;

    read(x);

    for (int i = 0; p - r != 0; i = i+1) {
        p = r;
        r = (r + x / r) / 2;
    }

    write("Квадратный корень: ", r);
}
