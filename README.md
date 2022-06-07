<p align="center">
  <img src="https://github.com/ediah/mlc/blob/a526268661edea2aff21163bf82ed8be3bc5da22/flower.png" alt="Icon" width="500"/>
</p>


<p align="center">
  <a href="https://lgtm.com/projects/g/ediah/mlc/alerts/"><img src="https://img.shields.io/lgtm/alerts/github/ediah/mlc?style=for-the-badge" alt="Total alerts" /></a>
  <a href="https://lgtm.com/projects/g/ediah/mlc/context:cpp"><img src="https://img.shields.io/lgtm/grade/cpp/github/ediah/mlc?style=for-the-badge" alt="Language grade: C/C++" /></a>
  <img src="https://img.shields.io/badge/coverage-75.9%25-yellow?style=for-the-badge" alt="Coverage">
</p>

# Краткое описание
Flower -- строго типизированный язык процедурного программирования. Языком поддерживается многопоточность, указатели заменены на концепт "общей" (или "разделяемой") переменной, 4 простых типа и один составной. Продвинутая работа со структурами: над ними можно проводить арифметические операции.

В репозитории ведётся разработка:

1. Оптимизирующего байт-код компилятора (flc),

2. Виртуальной стековой машины (flvm),

3. Отладчика виртуальной машины (fldbg).

# Порядок работы

1. Скомпилировать написанную программу `source.fl`:
```bash
./flc source.fl
```

2. Запустить бинарный файл на виртуальной машине:
```bash
./flvm out.bin
```

3. В случае ошибок или иных проблем отладить:
```bash
./fldbg out.bin
```

# Документация
Скоро будет доступна в разделе wiki, планируется работа с doxygen.

# Примеры

### Факториал

```c#
def factorial(int n): int {
    if (n > 1) 
        return factorial(n - 1) * n;
    else
        return 1;
}

program {
    int a;
    read(a);
    write(factorial(a));
}
```

### Многопоточность

```c#
def iterate(shared int a): shared int {
    for (int i = 1; i <= 5; i = i + 1) {
        /* write("Итерация #", i, ": a = ", a); */
        a = a / 2;
    }
    return a;
}

program {
    shared int x, y;
    
    read(x);
    read(y);

    thread:1 { x = iterate(x); }
    thread:2 { y = iterate(y); }

    fork(1, 2);
    lock();

    write(x, " ", y);
}
```

Множество других примеров Вы можете найти в папке 'test'.