<p align="center">
  <img src="https://raw.githubusercontent.com/ediah/mlc/b42af024b5c50ee507205f0f1affdcd0929b1d56/logo.png" alt="Icon" width="500"/>
</p>


<p align="center">
  <a href="https://lgtm.com/projects/g/ediah/mlc/alerts/"><img src="https://img.shields.io/lgtm/alerts/github/ediah/mlc?style=for-the-badge" alt="Total alerts" /></a>
  <a href="https://lgtm.com/projects/g/ediah/mlc/context:cpp"><img src="https://img.shields.io/lgtm/grade/cpp/github/ediah/mlc?style=for-the-badge" alt="Language grade: C/C++" /></a>
  <img src="https://img.shields.io/badge/coverage-76.1%25-yellow?style=for-the-badge" alt="Coverage">
</p>

# Компилятор модельного языка программирования
Рождённый на втором курсе факультета ВМК МГУ в качестве зачётной работы, он послужил для меня отличной практикой, благодаря которой я многому научился. Теперь я продолжаю этот проект в качестве хобби. Надеюсь, Вы тоже сможете извлечь из него пользу!

# Краткое описание
Model Language (ml) -- строго типизированный язык процедурного программирования.

Model Langage Compiler (mlc) -- оптимизирующий байт-код компилятор и виртуальная стековая машина.

# Особенности реализации

1. Основные операции: **if - else**, **for**, **while**, **break**, **continue**, **goto**, **read**, **write**
2. Основные простые типы: **int**, **real**, **string**, **bool**
3. Возможность описания пользовательского составного типа: **struct**
4. Поддержка функций
5. При обнаружении ошибки чтение не прекращается, а продолжается до конца

```bash 
$> ./mlc -h
Компилятор Модельного Языка Программирования v1.2.0 (Календула)
Флаги командной строки:
        -c      Компиляция
        -r      Выполнение
        -d      Отладка
        -O      Оптимизация
        -s      Не печатать ПОЛИЗ
        -v      Выводить сообщения оптимизатора
        -i      Указать входной файл
        -o      Указать выходной файл
``` 

# Примеры

### Привет, мир!

```py
program {
    write("Привет, мир!");
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

Множество других примеров Вы можете найти в папке 'test'.