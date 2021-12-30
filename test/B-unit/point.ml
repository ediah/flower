struct point {
    int x, y;
}

def length(struct point p): real {
    return root( p.x * p.x + p.y * p.y);
}

program {
    struct point p = {x = 3, y = 4};
    write( length( {x = 3, y = 4} ) );
    write( length( p ) );
}