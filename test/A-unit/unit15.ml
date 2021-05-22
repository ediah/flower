def factorial(int n): int {
    if (n > 1) 
        return factorial(n - 1) * n;
    else
        return 1;
}

program {
    int a, b;
    read(a);
    b = factorial(a);
    write(b);
}