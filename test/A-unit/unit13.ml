struct point {
    int x, y;
}

program {

    for (int i = 0; i < 5; i = i+1) 
        write(i);

    for (real r = 0.0; r < 5; r = r + 1)
        write(r);

    for (string s ="a"; s < "aaaaa"; s = s + "a")
        write(s);

    for (bool b = true; b; b = false)
        write(b);
        
    for (struct point p = {x = 0, y = 0}; 
         p.x * p.x + p.y * p.y < 9;
         p = {x = p.x + 1, y = p.y + 1})
        write(p.x, " ", p.y);
}