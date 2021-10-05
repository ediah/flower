#include <iostream>
#include <cstring>
#include <vector>
#include "compiler/parser.hpp"
#include "common/obstacle.hpp"
#define C_IS_ALPHA ((c >= 'a') && (c <= 'z') || (c >= 'A') && (c <= 'Z') || (c == '_'))
#define C_IS_NUM ((c >= '0') && (c <= '9'))

// Быстрый алгоритм возведения x в степень n
int Parser::fastPow(int x, int n) {
    int r = 1;
    while (n != 0) {
        if ( (n & 1) == 1) r *= x;
        x *= x;
        n >>= 1;
    }
    return r;
}

void Parser::load(std::string name) {
    code.open(name);
    if (!code.is_open()) {
        std::cout << "Файл " << name << " не найден!\n";
        exit(-1);
    }
}

Parser::~Parser(void) {
    code.close();
}

bool Parser::parse(void) {
    bool programExists = false;
    code >> c;

    IdTable.pushType(_LABEL_);
    IdentTable * progOffset = IdTable.confirm();
    poliz.pushVal(progOffset);
    poliz.pushOp(_NONE_, _LABEL_, JMP);
    try {

        do {
            if (fileQueue.size() != 0) {
                code.swap(*fileQueue.back());
                fileQueue.pop_back();
                code >> c;
            }

            while (!code.eof()) {
                if (readWord("program")) {
                    progOffset->setVal(new int (poliz.getSize()));
                    program();
                } else if (readWord("struct"))
                    defStruct();
                else if (readWord("def"))
                    defFunction();
                else if (readWord("include"))
                    include();
                else throw Obstacle(WRONG_SCOPE);
                code >> c;
            }
        } while (fileQueue.size() != 0);
        
       
    }
    catch(Obstacle & o) {
        ok = false;
        c.where();
        o.describe();
        c.cite(code);
        c.line++;
    }
    return ok;
}

void Parser::include() {
    std::string incfile;
    
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c == '\"') {
        do {
            code >>= c;
            incfile.push_back(c.symbol());
        } while ((c != '\"') && (!code.eof()));
        incfile.pop_back();
        code >> c;
    } else {
        do {
            incfile.push_back(c.symbol());
            code >>= c;
        } while ((c != ';') && (!code.eof()));
    }

    #ifdef DEBUG
    std::cout << "Переключаюсь на файл \"" << incfile << "\"\n";
    #endif

    std::ifstream * mainFile = new std::ifstream;
    mainFile->swap(code);
    fileQueue.push_back(mainFile);
    load(incfile);
    if ((c == ' ') || (c == '\n')) code >> c;
}

void Parser::defFunction(void) {
    inFunc = true;
    IdentTable * thisFunc = IdTable.confirm();
    thisFunc->setFunc();
    thisFunc->setOffset( poliz.getSize() );
    if ((c == ' ') || (c == '\n')) code >> c;
    char * name = identificator();
    thisFunc->setId(name);
    if (c != '(') throw Obstacle(FUNC_OPENBR);
    code >> c;

    IdentTable * formalParams = nullptr;
    if (c != ')') {
        IdTable.last()->setOrd(0);
        type();
        formalParams = def();

        while (c == ';') {
            code >> c;
            type();
            def();
        }

        if (c != ')') throw Obstacle(FUNC_CLOSEBR);
    }
    
    int paramsNum;
    if (formalParams != nullptr) 
        paramsNum = IdTable.last()->getOrd() - formalParams->getOrd();
    else paramsNum = 0;

    thisFunc->setParams(paramsNum);

    if (paramsNum != 0)
        thisFunc->setVal(formalParams);
    
    IdentTable * p = formalParams;
    for (int i = 0; i < paramsNum; i++) {
        if (p->isShared()) {
            IdTable.pushVal(new int (i));
            IdTable.pushType(_INT_);
            poliz.pushVal(IdTable.confirm());
            poliz.pushOp(_NONE_, _INT_, SHARE);
        }
        p->onReg();
        p = p->next;
    }

    code >> c;
    
    if (c == ':') {
        code >> c;

        if (readWord("shared")) {
            thisFunc->setShared();
            code >> c;
        }

        if (readWord("int"))
            thisFunc->setType(_INT_);
        else if (readWord("string"))
            thisFunc->setType(_STRING_);
        else if (readWord("real"))
            thisFunc->setType(_REAL_);
        else if (readWord("bool"))
            thisFunc->setType(_BOOLEAN_);
        else if (readWord("struct")) {
            thisFunc->setType(_STRUCT_);
            code >> c;
            char * stName = identificator();
            IdTable.pushStruct(stName);
        } else throw Obstacle(NO_TYPE);
    } else throw Obstacle(PROCEDURE);

    retTypes.push(new type_t (thisFunc->getType()));

    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '{') throw Obstacle(PROG_OPENBR);
    code >> c;

    defs();
    operations();

    delete static_cast<type_t*>(retTypes.pop());
    
    if (c != '}') throw Obstacle(PROG_CLOSEBR);
    if (inFunc) throw Obstacle(NO_RETURN);

    while (formalParams != nullptr) {
        formalParams->fade();
        formalParams = formalParams->next;
    }
    inFunc = false;
}

void Parser::returnOp(void) {
    inFunc = false;
    code >> c;
    type_t retFact = expr();
    if (retFact != * static_cast<type_t *>(retTypes.top())) 
        throw Obstacle(RETURN_TYPE_MISMATCH);
    if (c != ';') throw Obstacle(SEMICOLON);
    code >> c;
    poliz.pushOp(_INT_, _LABEL_, RET);
}

void Parser::program(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '{') throw Obstacle(PROG_OPENBR);
    code >> c;

    defs();
    operations();

    if (c != '}') throw Obstacle(PROG_CLOSEBR);
    poliz.pushOp(_NONE_, _NONE_, STOP);
}

void Parser::defStruct(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    char * name = identificator();
    StTable.pushName(name);
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '{')
        throw Obstacle(BAD_STRUCT);
    code >> c;
    do {        
        type_t field_type;
        char * struct_name = nullptr;
        char * field_name = nullptr;
        bool shared = false;

        if (readWord("shared")) {
            if (c != ' ') throw Obstacle(TYPE_UNKNOWN);
            shared = true;
            code >> c;
        }

        if (readWord("int"))
            field_type = _INT_;
        else if (readWord("string"))
            field_type = _STRING_;
        else if (readWord("real"))
            field_type = _REAL_;
        else if (readWord("bool"))
            field_type = _BOOLEAN_;
        else if (readWord("struct")) {
            if (c != ' ') throw Obstacle(TYPE_UNKNOWN);
            code >> c;
            struct_name = identificator();
            field_type = _STRUCT_;
        } else throw Obstacle(TYPE_UNKNOWN);

        if (c != ' ') throw Obstacle(TYPE_UNKNOWN);

        do {
            code >> c;
            field_name = identificator();
            StTable.pushField(field_type, field_name, struct_name, shared);
            if ((c == ' ') || (c == '\n')) code >> c;
        } while (c == ',');

        if (c != ';') throw Obstacle(SEMICOLON);
        code >> c;
    } while (c != '}');
    StTable.confirm();
}

void Parser::defs(void) {
    while (type()) {
        def();
        if (c != ';') throw Obstacle(SEMICOLON);
        code >> c;
    }
}

IdentTable * Parser::def(void) {
    IdentTable * r = nullptr;
    try {
        do {
            code >> c;
            if (r == nullptr) r = variable();
            else variable();
            if ((c == ' ') || (c == '\n')) code >> c;
        } while (c == ',');
    } catch(Obstacle & o) {
        IdTable.last()->setId(nullptr);
        ok = false;
        c.where();
        o.describe();
        c.cite(code);
        c.line++;
    }
    return r;
}

bool Parser::typeModificator(void) {
    bool r = true;

    if (readWord("shared")) {
        IdTable.last()->setShared();
        if (c != ' ') throw Obstacle(BAD_TYPE);
        code >> c;
    }
    else r = false;
    
    return r;
}

bool Parser::type(void) {
    bool r = true;
    bool tmod = typeModificator();

    if (readWord("int"))
        IdTable.pushType(_INT_);
    else if (readWord("string"))
        IdTable.pushType(_STRING_);
    else if (readWord("real"))
        IdTable.pushType(_REAL_);
    else if (readWord("bool"))
        IdTable.pushType(_BOOLEAN_);
    else if (readWord("struct")) {
        code >> c;
        char * stName = identificator();
        IdTable.pushStruct(stName);
        IdTable.pushType(_STRUCT_);
        IdentTable & templateFields = StTable.getStruct(stName)->getFields();
        IdTable.pushVal(new IdentTable(templateFields));
    } else r = false;

    if (tmod && (!r))
        throw Obstacle(MODIF_WITHOUT_TYPE);

    return r && (c == ' ');
}

/* Перед выполнением:
 * aab bba aca
 *     ^~~ <= курсор на первой букве слова
 * После выполнения:
 * aab bba aca
 *        ^ <= курсор за последней буквой слова (если true)
 * Если false, курсор на начальной позиции
 */ 
bool Parser::readWord(char * word) {
    bool r = true;
    int i;
    for (i = 0; word[i] != '\0'; i++) {
        if (c != word[i]) { r = false; break; }
        code >>= c;
    }
    // Каждое слово должно отделяться пробелом или знаком препинания
    if (C_IS_NUM || C_IS_ALPHA) r = false;
    if (!r) revert(i);
    return r;
}

void Parser::assignStruct(IdentTable * lval, IdentTable * rval) {
    while (lval->next != nullptr) {
        if (lval->getType() == _STRUCT_)
            assignStruct(static_cast<IdentTable *>(lval->getVal()), 
                         static_cast<IdentTable *>(rval->getVal()));
        else {
            poliz.pushVal(lval);
            poliz.pushVal(rval);
            poliz.pushOp(lval->getType(), rval->getType(), ASSIGN);
        }
        lval = lval->next;
        rval = rval->next;
    }
}

void Parser::assign(IdentTable * lval) {
    type_t lvtype = lval->getType();

    if (lvtype == _STRUCT_) {
        if (c == '{') {
            IdentTable * fields = static_cast<IdentTable *>(lval->getVal());
            do {
                code >> c;
                char * fieldName = identificator();
                IdentTable * val = fields->getIT(fieldName);

                if ((c == ' ') || (c == '\n')) code >> c;

                while (c == '.') {
                    code >> c;
                    fieldName = identificator();
                    IdentTable * innerFields = static_cast<IdentTable *>(val->getVal());
                    val = innerFields->getIT(fieldName);
                    if ((c == ' ') || (c == '\n')) code >> c;
                }

                if (c != '=') throw Obstacle(BAD_EXPR);
                code >> c;
                assign(val);
            } while (c == ',');
            if (c != '}') throw Obstacle(BAD_EXPR);
            code >> c;
        } else {
            char * rvalName = identificator();
            IdentTable * rval = IdTable.getIT(rvalName);
            type_t rvtype = rval->getType();
            if ((rvtype == _STRUCT_) && (strcmp(lval->getStruct(), rval->getStruct()) == 0)) {
                // структуры идентичны, можно приравнивать 
                assignStruct(static_cast<IdentTable *>(lval->getVal()), 
                             static_cast<IdentTable *>(rval->getVal()));
                
            } else throw Obstacle(EXPR_BAD_TYPE);
        }
    } else {
        poliz.pushVal(lval);
        type_t exop = expr();
        expressionType(lvtype, exop, ASSIGN);
        poliz.pushOp(lvtype, exop, ASSIGN);
        if ((c == ' ') || (c == '\n')) code >> c;
    }
}

IdentTable * Parser::variable(void) {
    IdTable.dupType();
    char * name = identificator();
    IdTable.pushId(name);
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c == '=') constVal();
    return IdTable.confirm();
}

char * Parser::identificator(void) {
    if (!C_IS_ALPHA) throw Obstacle(BAD_IDENT);
    char * ident = new char[MAXIDENT];
    int i = 0;
    do {
        ident[i++] = c.symbol();
        code >>= c;
    } while (C_IS_ALPHA);
    ident[i] = '\0';
    return ident;
}

void Parser::constStruct(IdentTable * fields) {

    if (c != '{') throw Obstacle(BAD_EXPR);

    do {
        code >> c;
        char * fieldName = identificator();
        IdentTable * val = fields->getIT(fieldName);
        type_t tval = val->getType();
        if ((c == ' ') || (c == '\n')) code >> c;
        if (c != '=') throw Obstacle(BAD_EXPR);
        code >> c;

        switch (tval) {
            case _INT_:
                val->setVal( new int (constInt()) );
                break;
            case _STRING_:
                val->setVal( constString() );
                break;
            case _REAL_:
                val->setVal( new float (constReal()) );
                break;
            case _BOOLEAN_:
                val->setVal( new bool (constBool()) );
                break;
            case _STRUCT_:
                constStruct(static_cast<IdentTable *>(val->getVal()));
                break;
            default:
                throw Obstacle(PANIC);
        }
        if ((c == ' ') || (c == '\n')) code >> c;
    } while (c == ',');
    if (c != '}') throw Obstacle(BAD_EXPR);
    code >> c;
}

void Parser::constVal(void) {
    IdentTable * val = IdTable.last();
    type_t tval = val->getType();

    code >> c;

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
        case _BOOLEAN_:
            IdTable.pushVal( new bool (constBool()) );
            break;
        case _STRUCT_:
            constStruct(static_cast<IdentTable *>(val->getVal()));
            break;
        default:
            throw Obstacle(PANIC);
    }
}

int Parser::constInt(void) {
    int x = 0, sign = 1;
    if (c == '-') {
        sign = -1;
        code >> c;
    } else if (c == '+') code >> c;

    if (!C_IS_NUM) throw Obstacle(BAD_INT);

    do {
        x = 10 * x + ( c.symbol() - '0');
        code >>= c;
    } while (C_IS_NUM);
    x = x * sign;

    return x;
}

char * Parser::constString(void) {
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

    code >>= c; // кавычка
    code >>= c;

    return x;
}

float Parser::constReal(void) {
    int intPart = constInt();

    if (c != '.') throw Obstacle(BAD_REAL);
    code >>= c;

    if (!C_IS_NUM) throw Obstacle(BAD_REAL);

    int x = 1;
    float floatPart = 0;
    do {
        floatPart += (float)( c.symbol() - '0') / fastPow(10, x++);
        code >>= c;
    } while (C_IS_NUM);

    if (intPart < 0) floatPart *= -1;

    return intPart + floatPart;
}

bool Parser::constBool(void) {
    bool r;
    if (readWord("true")) r = true;
    else if (readWord("false")) r = false;
    else throw Obstacle(BAD_BOOL);

    return r;
}

void Parser::operations(void) {
    while (c != '}') {
        try { operation(); }
        catch(Obstacle & o) {
            ok = false;
            c.where();
            o.describe();
            c.cite(code);
            c.line++;
            code >> c;
        }
    }
}

void Parser::operation(void) {

    if (c.symbol() == '}') return;
    else if (readWord("if")) condOp();
    else if (readWord("for")) forOp();
    else if (readWord("while")) whileOp();
    else if (readWord("break")) breakOp();
    else if (readWord("continue")) continueOp();
    else if (readWord("write")) writeOp();
    else if (readWord("goto")) gotoOp();
    else if (readWord("read")) readOp();
    else if (readWord("bytecode")) bytecodeOp();
    else if (readWord("return")) returnOp();
    else if (readWord("thread")) threadOp();
    else if (readWord("fork")) forkOp();
    else if (readWord("lock")) lockOp();
    else if (readWord("{")) {
        code >> c;
        operations();
        if (c != '}')
            throw Obstacle(OP_CLOSEBR);
        code >> c;
    } else {
        char * name = identificator();
        if ((c == ' ') || (c == '\n')) code >> c;

        if (c == ':') {
            code >> c;
            saveLabel(name, poliz.getSize());
            operation();
        } else {
            IdentTable * lval = IdTable.getIT(name);
            while (c == '.') {
                code >> c;
                name = identificator();
                IdentTable * fields = static_cast<IdentTable *>(lval->getVal());
                lval = fields->getIT(name);
                if ((c == ' ') || (c == '\n')) code >> c;
            }
            if (c != '=') throw Obstacle(BAD_OPERATOR);
            code >> c;
            assign(lval);
            if (c != ';') throw Obstacle(SEMICOLON);
            code >> c;
        }
    }

}

void Parser::threadOp(void) {
    if (inThread)
        throw Obstacle(NESTED_THREADS);
    
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != ':')
        throw Obstacle(NEED_THREAD_NUMBER);
    code >> c;

    IdTable.pushType(_LABEL_);
    IdentTable * progOffset = IdTable.confirm();
    poliz.pushVal(progOffset);
    poliz.pushOp(_NONE_, _LABEL_, JMP);

    int threadNum = constInt();
    IdTable.pushType(_LABEL_);
    IdTable.pushVal( new int (poliz.getSize()) );
    IdTable.pushId(nullptr);
    threads[threadNum] = IdTable.confirm();
    
    if ((c == ' ') || (c == '\n')) code >> c;

    if (c != '{')
        throw Obstacle(PROG_OPENBR);
    code >> c;

    inThread = true;
    operations();
    inThread = false;
    poliz.pushOp(_NONE_, _NONE_, STOP);

    if (c != '}')
        throw Obstacle(PROG_CLOSEBR);
    
    progOffset->setVal(new int (poliz.getSize()));

    code >> c;
}

void Parser::forkOp(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(FUNC_OPENBR);

    do {
        code >> c;
        int threadNum = constInt();
        poliz.pushVal(threads[threadNum]);
        poliz.pushOp(_NONE_, _INT_, FORK);
        if ((c == ' ') || (c == '\n')) code >> c;
    } while (c == ',');

    if (c != ')')
        throw Obstacle(FUNC_CLOSEBR);
    code >> c;
    if (c != ';')
        throw Obstacle(SEMICOLON);

    code >> c;
}

void Parser::lockOp(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(FUNC_OPENBR);
    code >> c;
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != ')')
        throw Obstacle(FUNC_CLOSEBR);
    code >> c;
    if (c != ';')
        throw Obstacle(SEMICOLON);

    poliz.pushOp(_NONE_, _NONE_, LOCK);

    code >> c;
}

IdentTable * Parser::saveLabel(char * label, int addr) {
    IdentTable * existinglab;
    try {
        existinglab = IdTable.getIT(label, false);
        if (existinglab->getType() != _LABEL_)
            throw Obstacle(LABEL_OR_IDENT);
        delete (int *) existinglab->getVal();
        existinglab->setVal(new int (addr));
    }
    catch(Obstacle & o) {
        if (o.r == LABEL_OR_IDENT) {
            ok = false;
            c.where();
            o.describe();
            c.cite(code);
            c.line++;
            code >> c;
        } else {
            IdTable.pushType(_LABEL_);
            IdTable.pushVal( new int (addr) );
            IdTable.pushId(label);
            existinglab = IdTable.confirm();
        }
    }
    return existinglab;
}

type_t Parser::expr(void) {
    type_t r = _NONE_;

    r = andExpr();

    while (true) {
        if (readWord("or")) {
            code >> c;
            type_t rval = andExpr();
            poliz.pushOp(r, rval, LOR);
            r = expressionType(r, rval, LOR);
        } else break;
    }

    return r;
}

type_t Parser::andExpr(void) {
    type_t r = _NONE_;
    
    r = boolExpr();

    while (true) {
        if (readWord("and")) {
            code >> c;
            type_t rval = boolExpr();
            poliz.pushOp(r, rval, LAND);
            r = expressionType(r, rval, LAND);
        } else break;
    }
    return r;
}

type_t Parser::boolExpr(void) {
    type_t r = _NONE_;

    r = add();
    
    if ( (c == '=') || (c == '<') || (c == '>') || (c == '!')) {
        operation_t op = NONE;
        char p = c.symbol();
        code >>= c;
        if (c == '=') {
            switch(p) {
                case '<': op = LESSEQ; break;
                case '>': op = GRTREQ; break;
                case '!': op = NEQ;    break;
                case '=': op = EQ;     break;
            }
            code >>= c;
        } else {
            switch(p) {
                case '<': op = LESS; break;
                case '>': op = GRTR; break;
                default: throw Obstacle(BAD_EXPR);
            }
        }
        if ((c == ' ') || (c == '\n')) code >> c;
        type_t rval = add();
        poliz.pushOp(r, rval, op);
        r = expressionType(r, rval, op);
    }

    return r;
}

type_t Parser::add(void) {
    bool exit = false;
    bool inverse = false;
    if (c == '+') code >> c;
    if (c == '-') {
        inverse = true;
        code >> c;
    }
    type_t r = mul();
    operation_t op;

    while (true) {

        switch (c.symbol()) {
            case '+': op = PLUS; break;
            case '-': op = MINUS; break;
            default: exit = true;
        }
        if (exit) break;
        code >> c;
        type_t rval = mul();
        poliz.pushOp(r, rval, op);
        r = expressionType(r, rval, op);
    }

    if (inverse) {
        poliz.pushOp(_NONE_, r, INV);
        r = expressionType(_NONE_, r, INV);
    }

    return r;
}

type_t Parser::mul(void) {
    bool exit = false;
    //int * fieldSize = new int (0);
    type_t r = constExpr();
    operation_t op;

    while (true) {

        switch (c.symbol()) {
            case '*': op = MUL; break;
            case '/': op = DIV; break;
            case '%': op = MOD; break;
            default: exit = true;
        }
        if (exit) break;
        code >> c;
        type_t rval = constExpr();
        poliz.pushOp(r, rval, op);
        r = expressionType(r, rval, op);

    }

    return r;
}

type_t Parser::constExpr(int * fieldSize) {
    type_t r;

    if (readWord("true")) {
        r = _BOOLEAN_;
        NEW_IDENT(val, _BOOLEAN_, nullptr, new bool (true))
    } else if (readWord("false")) {
        r = _BOOLEAN_;
        NEW_IDENT(val, _BOOLEAN_, nullptr, new bool (false))
    } else if (readWord("not")) {
        type_t val = constExpr();
        r = expressionType(_NONE_, val, LNOT);
        poliz.pushOp(_NONE_, val, LNOT);
    } else if (c == '(') {
        code >> c;
        r = expr();
        if (c != ')') throw Obstacle(EXPR_CLOSEBR);
        code >>= c;
    } else {
        
        if (c == '\"') {
            r = _STRING_;
            char * x = constString();
            NEW_IDENT(val, _STRING_, nullptr, x)
        } else {
            int start = code.tellg();
            try {
                r = _REAL_;
                float x = constReal();
                NEW_IDENT(val, _REAL_, nullptr, new float (x))
            } catch (Obstacle & o) {
                code.seekg(start - 1);
                code >>= c;
                if (o.r != BAD_INT) {
                    r = _INT_;
                    int x = constInt();
                    NEW_IDENT(val, _INT_, nullptr, new int (x))
                } else {
                    IdentTable * val = getFieldInStruct();
                    r = val->getType();

                    if (r == _STRUCT_) {
                        throw Obstacle(STRUCT_IN_EXPR);
                        /*
                        *fieldSize = 0;
                        IdentTable * fields = static_cast<IdentTable *>(val->getVal());
                        while (fields->next != nullptr) {
                            poliz.pushVal(fields);
                            fields = fields->next;
                            *fieldSize = *fieldSize + 1;
                        }
                        */
                    }

                    if (c == '(') { // Вызов функции
                        if (! val->isFunc())
                            throw Obstacle(NOT_CALLABLE);
                        callIdent(val);
                    } else {
                        if (val->isFunc())
                            throw Obstacle(CALLABLE);
                        poliz.pushVal(val);
                    }
                }
            }
        }
    }

    if ((c == ' ') || (c == '\n')) code >> c;

    return r;
}

IdentTable * Parser::getFieldInStruct(void) {
    char * name = identificator();
    IdentTable * val = IdTable.getIT(name);
    if ((c == ' ') || (c == '\n')) code >> c;

    while (c == '.') { // Добираемся до поля структуры
        code >> c;
        name = identificator();
        IdentTable * fields = static_cast<IdentTable *>(val->getVal());
        val = fields->getIT(name);
        if ((c == ' ') || (c == '\n')) code >> c;
    }
    if (!val->isShared() && inThread)
        throw Obstacle(PRIVATE_VAR_IN_THREAD);
    
    return val;
}

void Parser::callIdent(IdentTable * val) {
    code >> c;
    IdentTable * fields = static_cast<IdentTable *>(val->getVal());
    int paramCount = 0;
    while (c != ')') {
        if (fields == nullptr)
            throw Obstacle(TOO_MUCH_PARAMS);
        if (c == ',') code >> c;
        type_t exprType = expr();
        if (fields->getType() != exprType)
            throw Obstacle(EXPR_BAD_TYPE);
        fields = fields->next;
        paramCount++;
    }

    if (c != ')') throw Obstacle(FUNC_CLOSEBR);
    code >> c;
    if (paramCount != val->getParams())
        throw Obstacle(BAD_PARAMS_COUNT);
    NEW_IDENT(params, _INT_, nullptr, new int (paramCount))
    NEW_IDENT(callable, _LABEL_, nullptr, new int (val->getOffset()))
    poliz.pushOp(_INT_, _LABEL_, CALL);
}

void Parser::condOp(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(BAD_PARAMS_OPBR);
    code >> c;

    type_t r = expr();

    if (r != _BOOLEAN_)
        throw Obstacle(BAD_IF);

    if (c != ')')
        throw Obstacle(BAD_PARAMS_CLBR);

    code >> c;

    IdTable.pushType(_LABEL_);
    IdentTable * elsecase = IdTable.confirm();
    IdTable.pushType(_LABEL_);
    IdentTable * endif = IdTable.confirm();

    poliz.pushOp(_NONE_, _BOOLEAN_, LNOT);
    poliz.pushVal(elsecase);
    poliz.pushOp(_BOOLEAN_, _LABEL_, JIT);

    operation();

    poliz.pushVal(endif);
    poliz.pushOp(_NONE_, _LABEL_, JMP);

    elsecase->setVal( new int (poliz.getSize()) );

    if (readWord("else")) {
        code >> c;
        operation();
    }

    endif->setVal( new int (poliz.getSize()) );
}

IdentTable * Parser::cycleparam(void) {
    IdentTable * lval;
    if (type()) {
        // переменная не описана
        code >> c;
        char * name = identificator();
        IdTable.pushId(name);
        lval = IdTable.confirm();
        if ((c == ' ') || (c == '\n')) code >> c;
    } else {
        // переменная описана
        char * name = identificator();
        lval = IdTable.getIT(name);

        if ((c == ' ') || (c == '\n')) code >> c;

        while (c == '.') {
            code >> c;
            name = identificator();
            IdentTable * fields = static_cast<IdentTable *>(lval->getVal());
            lval = fields->getIT(name);
            if ((c == ' ') || (c == '\n')) code >> c;
        }
    }

    if (c != '=') throw Obstacle(BAD_EXPR);
    code >> c;
    assign(lval);

    return lval;
}

void Parser::forOp(void) {
    IdTable.pushType(_LABEL_);
    IdentTable * exit = IdTable.confirm();
    exits.push(exit);
    IdTable.pushType(_LABEL_);
    IdentTable * body = IdTable.confirm();

    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(BAD_PARAMS_OPBR);
    code >> c;

    IdentTable * cp = nullptr;
    try{ cp = cycleparam(); } // начальное выражение
    catch(...){}

    if (c != ';')
        throw Obstacle(SEMICOLON);

    IdTable.pushType(_LABEL_);
    IdTable.pushVal( new int (poliz.getSize()) );
    IdentTable * condexpr = IdTable.confirm();
    code >> c;
    type_t e2 = expr(); // условие продолжения

    if (e2 != _BOOLEAN_) throw Obstacle(BAD_IF);
    poliz.pushVal(body);
    poliz.pushOp(_BOOLEAN_, _LABEL_, JIT);
    // Здесь машина будет в случае невыполнения условия
    poliz.pushVal(exit);
    poliz.pushOp(_NONE_, _LABEL_, JMP);

    if (c != ';')
        throw Obstacle(SEMICOLON);

    IdTable.pushType(_LABEL_);
    IdTable.pushVal( new int (poliz.getSize()) );
    IdentTable * cyclexpr = IdTable.confirm();
    steps.push(cyclexpr);

    code >> c;

    char * name = identificator();
    IdentTable * cycleLval = IdTable.getIT(name);
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '=') throw Obstacle(BAD_EXPR);
    code >> c;
    assign(cycleLval);

    if (c != ')')
        throw Obstacle(BAD_PARAMS_CLBR);

    poliz.pushVal(condexpr);
    poliz.pushOp(_NONE_, _LABEL_, JMP);

    code >> c;
    body->setVal(new int (poliz.getSize()) );
    operation();  // тело цикла

    // Проверяем условие, если правда, то проходим через цикл. выражение
    // и возвращаемся в тело. Иначе выходим из него.
    poliz.pushVal(cyclexpr);
    poliz.pushOp(_NONE_, _LABEL_, JMP);

    exit->setVal(new int (poliz.getSize()) );

    exits.pop();
    steps.pop();
    while ((cp != nullptr) && (cp->getId() != nullptr)) {
        cp->setId(nullptr); // Эта переменная вне цикла не определена.
        cp = cp->next;
    }
}

void Parser::bytecodeOp(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '{')
        throw Obstacle(PROG_OPENBR);

    std::vector<type_t> types;

    do {
        bool couldRead = true;
        code >> c;
        switch (c.symbol()) {
            case 'A':
                if (readWord("ASSIGN")) {
                    BYTECODE_OP_BIN(ASSIGN)
                } else couldRead = false;
                break;
            case 'C':
                if (readWord("CALL")) {
                    BYTECODE_OP_BIN(CALL)
                } else couldRead = false;
                break;
            case 'D':
                if (readWord("DIV")) {
                    BYTECODE_OP_BIN(DIV)
                } else couldRead = false;
                break;
            case 'E':
                if (readWord("ENDL")) {
                    BYTECODE_OP_BIN(ENDL)
                } else if (readWord("EQ")) {
                    BYTECODE_OP_BIN(EQ)
                } else couldRead = false;
                break;
            case 'G':
                if (readWord("GRTR")) {
                    BYTECODE_OP_BIN(GRTR)
                } else if (readWord("GRTREQ")) {
                    BYTECODE_OP_BIN(GRTREQ)
                } else couldRead = false;
                break;
            case 'I':
                if (readWord("INV")) {
                    BYTECODE_OP_BIN(INV)
                } else couldRead = false;
                break;
            case 'J':
                if (readWord("JIT")) {
                    BYTECODE_OP_BIN(JIT)
                } else if (readWord("JMP")) {
                    BYTECODE_OP_BIN(JMP)
                } else couldRead = false;
                break;
            case 'L':
                if (readWord("LAND")) {
                    BYTECODE_OP_BIN(LAND)
                } else if (readWord("LESS")) {
                    BYTECODE_OP_BIN(LESS)
                } else if (readWord("LESSEQ")) {
                    BYTECODE_OP_BIN(LESSEQ)
                } else if (readWord("LNOT")) {
                    BYTECODE_OP_BIN(LNOT)
                } else if (readWord("LOAD")) {
                    BYTECODE_OP_BIN(LOAD)
                } else if (readWord("LOR")) {
                    BYTECODE_OP_BIN(LOR)
                } else couldRead = false;
                break;
            case 'M':
                if (readWord("MINUS")) {
                    BYTECODE_OP_BIN(MINUS)
                } else if (readWord("MOD")) {
                    BYTECODE_OP_BIN(MOD)
                } else if (readWord("MUL")) {
                    BYTECODE_OP_BIN(MUL)
                } else couldRead = false;
                break;
            case 'N':
                if (readWord("NEQ")) {
                    BYTECODE_OP_BIN(NEQ)
                } else couldRead = false;
                break;
            case 'P':
                if (readWord("PLUS")) {
                    BYTECODE_OP_BIN(PLUS)
                } else couldRead = false;
                break;
            case 'R':
                if (readWord("READ")) {
                    BYTECODE_OP_BIN(READ)
                } else if (readWord("RET")) {
                    BYTECODE_OP_BIN(RET)
                } else couldRead = false;
                break;
            case 'S':
                if (readWord("STOP")) {
                    BYTECODE_OP_BIN(STOP)
                } else couldRead = false;
                break;
            case 'W':
                if (readWord("WRITE")) {
                    BYTECODE_OP_BIN(WRITE)
                } else couldRead = false;
                break;
            
            default:
                couldRead = false;
                break;
        }
        if (!couldRead) {
            types.push_back(constExpr());
        }

        if ((c == ' ') || (c == '\n')) code >> c;
    } while (c == ',');

    if (c != '}')
        throw Obstacle(PROG_CLOSEBR);
    
    code >> c;
}

void Parser::whileOp(void) {
    IdTable.pushType(_LABEL_);
    IdentTable * exit = IdTable.confirm();
    exits.push(exit);

    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(BAD_PARAMS_OPBR);

    IdTable.pushType(_LABEL_);
    IdTable.pushVal( new int (poliz.getSize()) );
    IdentTable * condexpr = IdTable.confirm();
    steps.push(condexpr);
    code >> c;
    type_t e2 = expr(); // условие продолжения

    if (e2 != _BOOLEAN_) throw Obstacle(BAD_IF);
    poliz.pushOp(_NONE_, _BOOLEAN_, LNOT);
    poliz.pushVal(exit);
    poliz.pushOp(_BOOLEAN_, _LABEL_, JIT);
    
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != ')')
        throw Obstacle(BAD_PARAMS_CLBR);

    code >> c;
    operation();  // тело цикла

    poliz.pushVal(condexpr);
    poliz.pushOp(_NONE_, _LABEL_, JMP);

    exit->setVal(new int (poliz.getSize()) );
    exits.pop();
    steps.pop();
}
void Parser::breakOp(void) {
    if (exits.isEmpty())
        throw Obstacle(BREAK_OUTSIDE_CYCLE);
    poliz.pushVal(static_cast<IdentTable *>(exits.top()));
    poliz.pushOp(_NONE_, _LABEL_, JMP);
    if (c != ';')
        throw Obstacle(SEMICOLON);
    code >> c;
}

void Parser::continueOp(void) {
    if (exits.isEmpty())
        throw Obstacle(CONTINUE_OUTSIDE_CYCLE);
    poliz.pushVal(static_cast<IdentTable *>(steps.top()));
    poliz.pushOp(_NONE_, _LABEL_, JMP);
    if (c != ';')
        throw Obstacle(SEMICOLON);
    code >> c;
}

void Parser::gotoOp(void) {
    code >> c;
    char * label = identificator();
    IdentTable * labval;
    try {
        labval = IdTable.getIT(label, false);
    }
    catch(Obstacle & o) {
        // До метки ещё не дошли, но это не повод расстраиваться!
        labval = saveLabel(label, 0);
    }
    
    if (labval->getType() != _LABEL_)
        throw Obstacle(BAD_LABEL);

    code >> c;

    poliz.pushVal(labval);
    poliz.pushOp(_NONE_, _LABEL_, JMP);
}

void Parser::readOp(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(BAD_PARAMS_OPBR);

    code >> c;
    char * operand = identificator();
    IdentTable * it = IdTable.getIT(operand);
    poliz.pushVal(it);
    poliz.pushOp(_NONE_, it->getType(), READ);

    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != ')')
        throw Obstacle(BAD_PARAMS_CLBR);

    code >> c;

    if (c != ';')
        throw Obstacle(SEMICOLON);

    code >> c;
}

void Parser::writeOp(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(BAD_PARAMS_OPBR);

    do {
        code >> c;
        type_t exop = expr();
        poliz.pushOp(_NONE_, exop, WRITE);
    } while (c == ',');

    poliz.pushOp(_NONE_, _NONE_, ENDL);

    if (c != ')')
        throw Obstacle(BAD_PARAMS_CLBR);

    code >> c;

    if (c != ';')
        throw Obstacle(SEMICOLON);

    code >> c;
}

void Parser::finalize(void) {
    IdTable.repr();
    poliz.repr();
    std::cout << std::endl;
}

void Parser::giveBIN(const char * filename, bool optimize, bool silent, bool verbose) {
    bin.open(filename, std::ios_base::binary | std::ios_base::out);
    int x = 0;
    bin.write((char*)&x, sizeof(int)); // Сюда запишем адрес начала команд

    IdentTable * ITp = &IdTable;

    if (optimize) {
        Optimizer opt(&IdTable, &poliz);
        ITp = opt.optimize(verbose);
    }

    if (!silent) {
        ITp->repr();
        poliz.repr();
        std::cout << std::endl;
    }

    while (ITp->next != nullptr) {
        ITp->setOffset((int)bin.tellp());
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
            int tempint2 = IT_FROM_POLIZ(poliz, i)->getOffset();
            bin.write((char*)&tempint2, sizeof(int));
        }
        bin.write((char*)&execBit[i], sizeof(bool));
    }
    bin.seekp(0, std::ios_base::beg);
    bin.write((char*)&progStart, sizeof(int));

    bin.close();
}

void Parser::revert(int x) {
    code.seekg((int)code.tellg() - x - 1);
    code >>= c;
}