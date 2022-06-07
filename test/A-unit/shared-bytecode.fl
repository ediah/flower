/* 
   Пример использования общих переменных и байт-кода.
   Обратите внимание на то, что в функции program
   не происходит изменения переменной a, но она доступна
   для изменения в функции bytefun. В C тот же эффект
   имеют указатели.
*/

def anotherFun(shared int c): int {
    c = c + c;
    c = c / 4;
    return c;
}

def bytefun(shared int c): int {
    int b;

    bytecode {
        b, READ,               /* read(b);   */
        c, 2, b, MUL, ASSIGN   /* a = 2 * b; */
    }

    return anotherFun(c);
}

program {
    shared int a;
    int b;

    b = bytefun(a);

    write(a == b);
}