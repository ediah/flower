#ifndef PARSER_H
#define PARSER_H

#include "common/exprtype.hpp"
#define MAXIDENT 50
#define MAXTHREADS 4

#include <iostream>
#include <fstream>

#include "compiler/cursor.hpp"
#include "common/tables.hpp"
#include "common/poliz.hpp"
#include "runtime/stack.hpp"
#include "optimizer/optimizer.hpp"

#define NEW_IDENT(IT, type, id, val, fs) { \
    IdTable.pushType(type); \
    IdTable.pushVal(val); \
    IdTable.pushId(id); \
    IdentTable * IT = IdTable.confirm(); \
    for (int i = 0; i < *fs; i++) \
        poliz.pushVal(IT); \
}

#define BYTECODE_OP_BIN(op) { \
    type_t left, right; \
    if (types.size() != 0) { \
        right = types.back(); \
        types.pop_back(); \
    } else right = _NONE_; \
    if (types.size() != 0) { \
        left = types.back(); \
        types.pop_back(); \
    } else left = _NONE_; \
    poliz.pushOp(left, right, op); \
    type_t restype = expressionType(left, right, op); \
    if (restype != _NONE_) types.push_back(restype); \
}

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
    bool inFunc;         // Читает ли в данный момент тело функции
    bool inThread;       // Читает ли в данный момент определение потока
    std::vector<std::pair<type_t, char*>> retTypes;  // Тип возвращаемых параметров из функций
    std::vector<std::ifstream*> fileQueue; // Файлы в очереди к обработке
    IdentTable * threads[MAXTHREADS];

    // Вспомогательные функции
    static int fastPow(int x, int n);  // Быстрое возведение в степень
    bool readWord(char * word);        // Чтение непрерывной последовательности символов
    void revert(int x);                // Возврат курсора
public:
    Parser(): ok(true), inFunc(false), inThread(false) {
        c.line = 1;
        for (int i = 0; i < MAXTHREADS; i++)
                threads[i] = nullptr;
    };

    void load(std::string name); // Загрузка исходного кода
    bool parse(void);            // Разбор программы
    void include(void);
    void defs(void);             // Определения
    IdentTable * def(void);      // Определение переменных одного типа
    void defStruct(void);        // Определение структуры
    void defFunction(void);      // Определение функции
    
    bool typeModificator(void); // Модификатор типа
    bool type(void);            // Тип
    IdentTable * variable(void);// Переменная
    char * identificator(void); // Идентификатор
    void constVal(void);        // Инициализатор (константа)
    int constInt(void);         // Целое число
    char * constString(void);   // Строка
    float constReal(void);      // Вещественное число
    bool constBool(void);       // Логическая константа
    void constStruct(IdentTable * fields); // Структура
    IdentTable * getFieldInStruct(void);   // Получить элемент структуры
    void callIdent(IdentTable * val);      // Вызов объекта
    void assign(IdentTable * lval);        // Присваивание
    void assignStruct(IdentTable * lval, IdentTable * rval); // Присваивание структур
    int unrollStruct(IdentTable * lval, int ord = -1);
    void handleStruct(type_t lval, type_t rval, operation_t op, int * fieldSize, char * structName);
    void program(void);    // Программа

    void operations(void); // Операции
    void operation(void);  // Операция
    IdentTable * saveLabel(char * label, int addr); // Сохранение метки
    type_t expr     (int * fieldSize = nullptr, char * structName = nullptr); // Выражение ( a or b )
    type_t andExpr  (int * fieldSize = nullptr, char * structName = nullptr); // a and b
    type_t boolExpr (int * fieldSize = nullptr, char * structName = nullptr); // Сравнение
    type_t add      (int * fieldSize = nullptr, char * structName = nullptr); // a +- b
    type_t mul      (int * fieldSize = nullptr, char * structName = nullptr); // a */ b
    type_t constExpr(int * fieldSize = nullptr, char * structName = nullptr); // Константа или идентификатор
    void repack(int fieldSize);
    IdentTable * cycleparam(void); // Циклическое выражение

    void condOp(void);  // if - else
    void forOp(void);   // for
    void whileOp(void); // while
    void breakOp(void); // break
    void writeOp(void); // write
    void gotoOp(void);  // goto
    void readOp(void);  // read
    void continueOp(void); // continue
    void bytecodeOp(void); // bytecode
    void returnOp(void);
    void threadOp(void);
    void forkOp(void);
    void lockOp(void);

    void optimize(bool verbose);
    void finalize(void); // Вывод результата парсера в читаемом виде
    void giveBIN(const char * filename, bool optimize, bool silent, bool verbose); // Запись бинарника

    ~Parser(void);
};

#endif
