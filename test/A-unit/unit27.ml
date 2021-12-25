struct vector {
    real x, y, z;
}

program {
    struct vector a = {x = 1.0, y = 2.0, z = 3.0}, b, c;
    b = 2 * a;
    write(b.x, " ", b.y, " ", b.z);
}