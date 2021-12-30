struct circle {
    int radius;
    real length;
}

program {
    struct circle A = {
        radius = 5
    }, B;

    A.length = 2 * 3.14 * A.radius;
    B.radius = A.radius * 2;
    B.length = 2 * 3.14 * B.radius;

    write("Длина окружности с радиусом ", A.radius, " равна: ", A.length);
    write("Длина окружности с радиусом ", B.radius, " равна: ", B.length);
}