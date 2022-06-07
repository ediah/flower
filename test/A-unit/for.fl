struct point {
    int x, y;
}

program {
    int i;
    real r;
    string s;
    bool b;
    struct point p = {x = 0, y = 10};

    for (i = 0; i < 5; i = i+1) 
        write(i);

    for (r = 0.0; r < 5; r = r + 1)
        write(r);

    for (s = "a"; s < "aaaaa"; s = s + "a")
        write(s);

    for (b = true; b; b = false)
        write(b);
        
    for (p.y = 0; p.x * p.x + p.y * p.y < 9; p = p + 1)
        write(p.x, " ", p.y);
}