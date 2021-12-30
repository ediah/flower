pow(real x; int a): real {
    real r = 1;
    while (a > 0) {
        r = r * x;
        a = a - 1;
    }

    return r;
}

program {
    write( pow(2.0, 4) );
}