program {
    int a = 51, b = 6, c;
    string x = "abc", y, z = "abcd";
    c = (a + b) * 2;
    if ((c >= 100) or (x == z)) {
        read (y);
        write (y);
        write (x + y + z, c);
    }
    else
        c = a = 21;
    while (c > 100)
    {
        c = c − 5;
        write (c);
        x = x + "step";
    }
    write (x);
}
