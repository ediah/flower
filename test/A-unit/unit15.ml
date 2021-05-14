def factorial(int n): int {
    if (n > 1) 
        return factorial(n - 1) * n;
    else
        return 1;
}

program {
    write( factorial(5) ); /* 120 */
}