struct time {
    int h, m, s;
}

struct point {
    int x, y, z;
    struct time t;
}

program {
    struct point A, B, C;

    A = {
        y = 1, z = 2,
        t = { h = 1, m = 2 }
    };

    B = { x = 10, y = 11, z = 12 };
    B.t = { h = 5, m = 6, s = 7};

    write(A.x, " ", A.y, " ", A.z, " ", A.t.h, " ", A.t.m);
    write(B.x, " ", B.y, " ", B.z, " ", B.t.h, " ", B.t.m);