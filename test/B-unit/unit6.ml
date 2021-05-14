def do_something(int x, y): real {
    int a = 1, b = 2;
    return a / b;
}

program {
    /* Переменные x, y, a и b должны быть доступны
     * только из функции do_something. Попытка обратиться
     * к ним из program должно вызвать ошибку.
     */
    
    write(x);
    write(y);
    write(a);
    write(b);
}