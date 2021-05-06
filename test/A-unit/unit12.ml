program {
    int x = +5, a, i = -0;
start:
    write("Введите цифру 5!");
    read(a);
    if (a == x) goto end;
    write("Не та цифра!");
    i = +i - -1; /* Страшная конструкция, нужна для теста унарных + и - */
    goto start;
end:
    write("Правильно, спасибо!");
    write("Вы пытались ", i+1, " раз");
}