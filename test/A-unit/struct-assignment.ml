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
        t = { h = 3, m = 4 }
    };

    B = { x = 10, y = 11, z = 12 };
    B.t = { h = 5, m = 6, s = 7};

    C = A;
    C.t = B.t;

    write(A.x, " ", A.y, " ", A.z, " ", A.t.h, " ", A.t.m, " ", A.t.s);
    write(B.x, " ", B.y, " ", B.z, " ", B.t.h, " ", B.t.m, " ", B.t.s);
    write(C.x, " ", C.y, " ", C.z, " ", C.t.h, " ", C.t.m, " ", C.t.s);
}