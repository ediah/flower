def fib(int n): int {
    if (n > 1)
        return fib(n - 1) + fib(n - 2);
    else
        return 1;
}

program {
    write( fib(46) );
}
