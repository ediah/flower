include "test/A-unit/inc/sqrt.mli";

struct point {
    int x, y;
}

def find_point(real length): struct point {
    struct point p;
    real t;

    t = length / sqrt(2.0);
    p = {x = t, y = t};

    return p;
}

program {
    struct point p;
    real l;
    read(l);
    p = find_point(l);
    write(p.x, " ", p.y);
}