struct point {
    real x, y, z;
}

struct object {
    shared struct point pos;
    int t;
}

def step(shared struct point pos): bool {
    bool status = true;
    pos = { 
        x = pos.x + 0.1, 
        y = pos.y + 0.1, 
        z = pos.z + 0.1
    };
    return status;
}

program {
    struct object Obj = {
        pos = {x = 1.0, y = 2.0, z = 3.0},
        t = 10
    };
    if (step(Obj.pos))
        write(Obj.pos.x, " ", Obj.pos.y, " ", Obj.pos.z, " ", Obj.t);
}