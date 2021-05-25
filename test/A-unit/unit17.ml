def func(int a; real b): real {
    return ( b + a / b ) / 2;
}

def root(int x): real {
    real r, last = 0.0;
    r = x;

    while ( (r - last) != 0 ) {
        last = r;
        r = func(x, r);
    }

    return r;
}

program {
    int x;
    read(x);
    write(root(x));
}