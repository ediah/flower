#include <iostream>
#include "lexer.hpp"
#include "obstacle.hpp"
#define C_IS_ALPHA ((c >= 'a') && (c <= 'z') || (c >= 'A') && (c <= 'Z'))
#define C_IS_NUM ((c >= '0') && (c <= '9'))

// Быстрый алгоритм возведения x в степень n
int Lexer::fastPow(int x, int n) {
    int r = 1;
    while (n != 0) {
        if (n & 1 == 1) r *= x;
        x *= x;
        n >>= 1;
    }
    return r;
}

void Lexer::load(const char * name) {
    code.open(name);
}

void Lexer::parse(void) {
    char word[8] = "program";
    code >> c;

    try {
        for (int i = 0; i < 6; i++) {
            if (c != word[i]) throw Obstacle(PROG_START);
            code >>= c;
        }
        if (c != word[6]) throw Obstacle(PROG_START);

        code >> c;
        if (c != '{') throw Obstacle(PROG_OPENBR);

        defs();
        operations();

        if (c != '}') throw Obstacle(PROG_CLOSEBR);
        poliz.pushOp(_NONE_, _NONE_, STOP);
    }
    catch (Obstacle & o) {
        c.where();
        o.describe();
        exit(-1);
    }

}

void Lexer::defs(void) {
    while (true) {
        try { type(); }
        catch(Obstacle o) { break; }

        do {
            variable();
        } while (c == ',');

        if (c != ';') throw Obstacle(DEF_END);
    }
}

void Lexer::type(void) {
    code >> c;

    switch(c.symbol()) {
        case 'i':
            if (readWord("nt"))
                IdTable.pushType(_INT_);
            else throw Obstacle(TYPE_UNKNOWN);
            break;
        case 's':
            if (readWord("tring"))
                IdTable.pushType(_STRING_);
            else throw Obstacle(TYPE_UNKNOWN);
            break;
        case 'r':
            if (readWord("eal"))
                IdTable.pushType(_REAL_);
            else throw Obstacle(TYPE_UNKNOWN);
            break;
        default:
            throw Obstacle(TYPE_UNKNOWN);
    }
}

bool Lexer::readWord(char * word) {
    bool r = true;
    int i;
    for (i = 0; (word[i] != '\0') && r; i++) {
        code >>= c;
        if (c != word[i]) r = false;
    }
    if (!r) revert(i);
    return r;
}

void Lexer::variable(void) {
    IdTable.dupType();
    char * name = identificator();
    IdTable.pushId(name);
    if (c == '=') constVal();
    IdTable.confirm();
}

char * Lexer::identificator(void) {
    char * ident = new char[MAXIDENT];
    int i = 0;
    code >> c;
    //std::cout << c.symbol() << std::endl;
    if (!C_IS_ALPHA) throw Obstacle(BAD_IDENT);
    do {
        ident[i++] = c.symbol();
        code >>= c;
    } while (C_IS_ALPHA);
    ident[i] = '\0';
    //std::cout << "IDENT " << ident << std::endl;
    if (c.symbol() == ' ') code >> c;
    return ident;
}

void Lexer::constVal(void) {
    type_t tval = IdTable.last()->getType();

    try {
        switch(tval) {
            case _INT_:
                IdTable.pushVal( new int (constInt()) );
                break;
            case _STRING_:
                IdTable.pushVal( constString() );
                break;
            case _REAL_:
                IdTable.pushVal( new float (constReal()) );
                break;
        }
    }
    catch (Obstacle & o) {
        c.where();
        o.describe();
        exit(-1);
    }
}

int Lexer::constInt(void) {
    int x = 0, sign = 1;
    code >> c;
    if (c == '-') {
        sign = -1;
        code >> c;
    } else if (c == '+') code >> c;

    if (!C_IS_NUM) throw Obstacle(BAD_NUM);

    do {
        x = 10 * x + ( c.symbol() - '0');
        code >>= c;
    } while (C_IS_NUM);
    x = x * sign;

    //std::cout << "Прочитано число " << x << std::endl;

    return x;
}

char * Lexer::constString(void) {
    code >> c;
    if (c != '\"') throw Obstacle(BAD_STRING);

    int start = code.tellg();
    do { code >>= c; }
    while ((!code.eof()) && (c != '\"'));

    if (code.eof()) throw Obstacle(BAD_STRING);

    int len = (int)code.tellg() - start - 1;
    code.seekg(start);

    char *x = new char[len + 1];

    for (int i = 0; i < len; i++) {
        code >>= c;
        x[i] = c.symbol();
    }

    x[len] = '\0';

    code >>= c;
    code >> c;

    //std::cout << "Прочитана строка \"" << x << '\"'<< std::endl;

    return x;
}

float Lexer::constReal(void) {
    int intPart = constInt();

    if (c != '.') throw Obstacle(BAD_NUM);
    code >>= c;

    if (!C_IS_NUM) throw Obstacle(BAD_NUM);

    int x = 1;
    float floatPart = 0;
    do {
        floatPart += (float)( c.symbol() - '0') / fastPow(10, x++);
        code >>= c;
    } while (C_IS_NUM);

    if (intPart < 0) floatPart *= -1;

    return intPart + floatPart;
}

void Lexer::operations(void) {
    /*
    if (<выражение>) <оператор> else <оператор>
    | for ([выражение]; [выражение]; [выражение]) <оператор>
    | while (<выражение>) <оператор>
    | break;
    | goto <идентификатор> ;
    | read (<идентификатор>);
    | write (
    */

    while (true) {
        try { operation(); }
        catch(Obstacle o) { break; }
    }
}

void Lexer::operation(void) {

    code.putback(c.symbol());

    if (readWord("if")) condOp();
    else if (readWord("for")) forOp();
    else if (readWord("while")) whileOp();
    else if (readWord("break")) breakOp();
    else if (readWord("write")) writeOp();
    else if (readWord("goto")) gotoOp();
    else if (readWord("read")) readOp();
    else if (readWord("{")) {
        operations();
        code >> c;
        if (c != '}')
            throw Obstacle(OP_CLOSEBR);
    } else {
        char * name = identificator();
        if (c == ':') {
            // Помеченный оператор
            saveLabel(name); // TODO: сохранить метку
            operation();
        } else if (c == '=') {
            std::cout << name << std::endl;
            IdentTable * lval = IdTable.getIT(name);
            type_t lvtype = lval->getType();
            poliz.pushVal(lval);
            type_t exop = expr();
            expressionType(lvtype, exop, ASSIGN);
            poliz.pushOp(lvtype, exop, ASSIGN);
            code >> c;
        } else throw Obstacle(BAD_OPERATOR);
    }
}

void Lexer::saveLabel(char * label) {

}

type_t Lexer::expr(void) {
    type_t r = _NONE_;
    try {
        r = add();

        code >> c;
        if ( (c == '=') || (c == '<') || (c == '>') || (c == '!')) {
            type_t rval = add();
            operation_t op;
            char p = c.symbol();
            code >>= c;
            if (c == '=') {
                switch(p) {
                    case '<': op = LESSEQ; break;
                    case '>': op = GRTREQ; break;
                    case '!': op = NEQ;    break;
                }
            } else {
                switch(p) {
                    case '<': op = LESS; break;
                    case '>': op = GRTR; break;
                    case '=': op = EQ;   break;
                }
            }
            poliz.pushOp(r, rval, op);
            r = expressionType(r, rval, op);

        }
    }
    catch(Obstacle & o) {
        c.where();
        o.describe();
        exit(-1);
    }
    return r;
}

type_t Lexer::add(void) {
    bool exit = false;
    type_t r = mul();
    operation_t op;

    while (true) {
        code >> c;

        switch (c.symbol()) {
            case '+': op = PLUS; break;
            case '-': op = MINUS; break;
            case 'o':
                code >>= c;
                if (c == 'r') {
                    op = LOR;
                } else {
                    revert(2);
                    exit = true;
                }
                break;
            default:
                revert(1);
                exit = true;
        }
        if (exit) break;
        type_t rval = mul();
        poliz.pushOp(r, rval, op);
        r = expressionType(r, rval, op);
    }

    return r;
}

type_t Lexer::mul(void) {
    bool exit = false;
    type_t r = constExpr();
    operation_t op;

    while (true) {
        switch (c.symbol()) {
            case '*': op = MUL; break;
            case '/': op = DIV; break;
            case 'a':
                if (readWord("nd")) {
                    op = LAND;
                    break;
                }
            default:
                revert(1);
                exit = true;
        }
        if (exit) break;
        type_t rval = constExpr();
        poliz.pushOp(r, rval, op);
        r = expressionType(r, rval, op);

    }

    return r;
}

type_t Lexer::constExpr(void) {
    type_t r;

    code >> c;
    code.putback(c.symbol());

    if (readWord("true")) {
        r = _BOOLEAN_;
        IdTable.pushId(nullptr);
        IdTable.pushType(_BOOLEAN_);
        IdTable.pushVal(new bool (true));
        IdentTable * val = IdTable.confirm();
        poliz.pushVal(val);
    } else if (readWord("false")) {
        r = _BOOLEAN_;
        IdTable.pushId(nullptr);
        IdTable.pushType(_BOOLEAN_);
        IdTable.pushVal(new bool (false));
        IdentTable * val = IdTable.confirm();
        poliz.pushVal(val);
    } else if (readWord("not")) {
        type_t val = constExpr();
        r = expressionType(_NONE_, val, LNOT);
        poliz.pushOp(_NONE_, val, LNOT);
    } else if (readWord("(")) {
        r = expr();
        if (c != ')') throw Obstacle(EXPR_CLOSEBR);
        code >> c;
    } else {
        int start = code.tellg();
        try {
            r = _INT_;
            int x = constInt();
            IdTable.pushId(nullptr);
            IdTable.pushType(_INT_);
            IdTable.pushVal(new int (x));
            IdentTable * val = IdTable.confirm();
            poliz.pushVal(val);
        } catch(...) {
            code.seekg(start);
            try {
                char * name = identificator();
                IdentTable * val = IdTable.getIT(name);
                r = val->getType();
                poliz.pushVal(val);
            } catch (Obstacle & o) {
                c.where();
                o.describe();
                exit(-1);
            }
        }
    }
    return r;
}

void Lexer::condOp(void) {

}
void Lexer::forOp(void) {

}
void Lexer::whileOp(void) {

}
void Lexer::breakOp(void) {

}

void Lexer::gotoOp(void) {

}

void Lexer::readOp(void) {
    code >> c;

    if (c != '(')
        throw Obstacle(BAD_PARAMS_OPBR);

    char * operand = identificator();

    if (c != ')')
        throw Obstacle(BAD_PARAMS_CLBR);

    code >> c;

    if (c != ';')
        throw Obstacle(CLOSED_BOOK);

    code >> c;

    // TODO: запись в ПОЛИЗ -- ЧТЕНИЕ
    std::cout << "READ " << operand << std::endl;
}

void Lexer::writeOp(void) {
    try {
        code >> c;

        if (c != '(')
            throw Obstacle(BAD_PARAMS_OPBR);

        do {
            type_t exop = expr();
            poliz.pushOp(_NONE_, exop, WRITE);
        } while (c == ',');

        if (c != ')')
            throw Obstacle(BAD_PARAMS_CLBR);

        code >> c;

        if (c != ';')
            throw Obstacle(CLOSED_BOOK);

        code >> c;
    }
    catch (Obstacle & o) {
        c.where();
        o.describe();
        exit(-1);
    }
}

void Lexer::finalize(void) {
    IdTable.repr();
    poliz.repr();
    std::cout << std::endl;
}

void Lexer::giveBIN(char * filename) {
    bin.open(filename, std::ios_base::binary | std::ios_base::out);
    int x = 0;
    bin.write((char*)&x, sizeof(int)); // Сюда запишем адрес начала команд

    IdentTable * ITp = &IdTable;
    while (ITp->next != nullptr) {
        ITp->setOffset((int)bin.tellp());
        //std::cout << ITp->getOffset();
        ITp->writeValToStream(bin);
        ITp = ITp->next;
    }

    int progStart = bin.tellp();
    int b = poliz.getSize();
    op_t * prog = poliz.getProg();
    bool * execBit = poliz.getEB();
    for (int i = 0; i < b; i++) {
        if (execBit[i]) {
            int tempint1 = (int)prog[i];
            bin.write((char*)&tempint1, sizeof(int));
        } else {
            int tempint2 = ((IdentTable *)prog[i])->getOffset();
            bin.write((char*)&tempint2, sizeof(int));
        }
        bin.write((char*)&execBit[i], sizeof(bool));
    }
    bin.seekp(0, std::ios_base::beg);
    bin.write((char*)&progStart, sizeof(int));

    bin.close();
}

void Lexer::revert(int x) {
    code.seekg((int)code.tellg() - x);
}
