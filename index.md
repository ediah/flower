<p align="center">
  <img src="https://raw.githubusercontent.com/ediah/mlc/b42af024b5c50ee507205f0f1affdcd0929b1d56/logo.png" alt="Icon" width="500"/>
</p>

## Язык, с которым можно получить "отл"!

<p align="center">
  <a href="https://lgtm.com/projects/g/ediah/mlc/alerts/"><img src="https://img.shields.io/lgtm/alerts/github/ediah/mlc?style=for-the-badge" alt="Total alerts" /></a>
  <a href="https://lgtm.com/projects/g/ediah/mlc/context:cpp"><img src="https://img.shields.io/lgtm/grade/cpp/github/ediah/mlc?style=for-the-badge" alt="Language grade: C/C++" /></a>
</p>

# Особенности реализации

1. Разбор кода методом рекурсивного спуска (`parser.cpp`)
2. Основные операции: **if - else**, **for**, **while**, **break**, **continue**, **goto**, **read**, **write**
3. Основные простые типы: **int**, **real**, **string**, **bool**
4. Возможность описания пользовательского составного типа: **struct**
5. Поддержка функций
6. Вычисление выражений с любыми простыми типами и полями структур простого типа
7. При обнаружении ошибки выводится соответсвующее сообщение и продолжается чтение до конца
8. Компиляция проводится для виртуальной стековой машины (`vmachine.cpp`)

Пример использования:

```bash 
./mlc -c -r -i test/A-unit/unit10.ml
``` 

# Пример

### Привет, %username%!

```py
program {
    string username;
    read(username);
    write("Привет, " + username + "!");
}
```

### Факториал

```py
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
