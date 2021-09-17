def func(real x): real {
    return 3 * x * x + 2 * x - 5;
}

program {
    real x, y, e = 0.001;
    real a = 0.0, b = 10.0, s = 0.0;

    x = a;
    y = func(x);

    while (x < b) {
        x = x + e;
        s = s + e * (func(x) + y) / 2;
        y = func(x);
    }

    write(s);
}