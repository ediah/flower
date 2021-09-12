#ifndef PARSER_H
#define PARSER_H

#define MAXIDENT 50

#include <iostream>
#include <fstream>

#include "compiler/cursor.hpp"
#include "common/tables.hpp"
#include "common/poliz.hpp"
#include "common/stack.hpp"
#include "optimizer/optimizer.hpp"

class Parser {
    std::ifstream code; // Код
    std::ofstream bin;  // Бинарник
    Cursor c;           // Курсор
    IdentTable IdTable; // Таблица идентификаторов
    POLIZ poliz;        // ПОЛИЗ

    Stack exits;         // Стек выходов из циклов
    Stack steps;         // Стек входов в циклы
    StructTable StTable; // Таблица структур
    bool ok;             // Произошла ли ошибка во время чтения программы
    bool inFunc;
    Stack retTypes;      // Тип возвращаемых параметров из функций

    // Вспомогательные функции
    static int fastPow(int x, int n);  // Быстрое возведение в степень
    bool readWord(char * word);        // Чтение непрерывной последовательности символов
    void revert(int x);                // Возврат курсора
public:
    Parser(): ok(true), inFunc(false) {
        c.line = 1;
    };

    void load(std::string name); // Загрузка исходного кода
    bool parse(void);            // Разбор программы
    void defs(void);             // Определения
    IdentTable * def(void);      // Определение переменных одного типа
    void defStruct(void);        // Определение структуры
    void defFunction(void);      // Определение функции
    /*
    void defInput(void);
    void defOutput(void);
    */
    
    bool type(void);            // Тип
    IdentTable * variable(void);// Переменная
    char * identificator(void); // Идентификатор
    void constVal(void);        // Инициализатор (константа)
    int constInt(void);         // Целое число
    char * constString(void);   // Строка
    float constReal(void);      // Вещественное число
    bool constBool(void);       // Логическая константа
    void constStruct(IdentTable * fields); // Структура
    void assign(IdentTable * lval);        // Присваивание
    void assignStruct(IdentTable * lval, IdentTable * rval); // Присваивание структур

    void program(void);    // Программа

    void operations(void); // Операции
    void operation(void);  // Операция
    IdentTable * saveLabel(char * label, int addr); // Сохранение метки
    type_t expr(void);     // Выражение ( a or b )
    type_t andExpr(void);  // a and b
    type_t boolExpr(void); // Сравнение
    type_t add(void);      // a +- b
    type_t mul(void);      // a */ b
    type_t constExpr(void);  // Константа или идентификатор
    IdentTable * cycleparam(void); // Циклическое выражение

    void condOp(void);  // if - else
    void forOp(void);   // for
    void whileOp(void); // while
    void breakOp(void); // break
    void writeOp(void); // write
    void gotoOp(void);  // goto
    void readOp(void);  // read
    void continueOp(void); // continue
    void returnOp(void);

    void optimize(bool verbose);
    void finalize(void); // Вывод результата парсера в читаемом виде
    void giveBIN(const char * filename, bool optimize, bool verbose); // Запись бинарника

    ~Parser(void);
};

#endif
