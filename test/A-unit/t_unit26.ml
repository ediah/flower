struct point {
    real x, y, z;
}

struct object {
    shared struct point pos;
    int t;
}

def step(shared struct point pos): bool {
    bool status = true;
    pos = pos + 1;
    return status;
}

def length(struct point pos): real {
    return 0.0;
}

program {
    struct object Obj = {
        pos = {x = 1.0, y = 2.0, z = 3.0},
        t = 10
    };
    if (step(Obj.pos))
        write(Obj.pos.x, " ", Obj.pos.y, " ", Obj.pos.z, " ", Obj.t);
}