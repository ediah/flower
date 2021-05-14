def mul(int a, b): int {
    /* Переменные x и y должны быть доступны только в program.
     * В функции mul они должны быть недоступны.
     */
    return x * y;
}

program {
    int x = 2, y = 3;
    write( mul(x, y) );
}