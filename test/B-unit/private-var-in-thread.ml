program {
    int i = 0;

    thread:1 { 
        i = i + 1;
    }

    write(i);
}