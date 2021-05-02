struct typeA {
    bool x;
}

struct typeB {
    bool x;
}

struct typeC {
    real y;
}

program {
    struct typeA a = {x = true};
    struct typeB b = {x = false};
    struct typeC c = {y = 3.14};

    /* Поля одинаковые, но имена структур разные*/
    a = b;
    /* Поля различаются тоже */
    a = c;

}