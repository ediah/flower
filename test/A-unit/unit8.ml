struct point {
    int x, y;
    string name;
}

program {
    struct point A, B;

    A.x = 0;
    A.y = 1;
    A.name = "A";
    B = {x = 2, name = "B", y = 3};

    write("Точки:");
    write(A.name, ": ", A.x, " ", A.y);
    write(B.name, ": ", B.x, " ", B.y);
}