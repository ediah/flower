#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include <cassert>
#include "common/exprtype.hpp"
#include "common/tables.hpp"
#include "common/obstacle.hpp"
#include "compiler/parser.hpp"
#include "config.hpp"
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
    code >> c;

    IdTable.pushType(_LABEL_);
    IdentTable * progOffset = IdTable.confirm();
    poliz.pushVal(progOffset);
    poliz.pushOp(_NONE_, _LABEL_, JMP);
    try {

        do {
            if (fileQueue.size() != 0) {
                code.swap(*fileQueue.back());
                fileQueue.back()->close();
                delete fileQueue.back();
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
    int paramsSum = 0;
    for (int i = 0; i < paramsNum; i++) {
        if (p->isShared()) {
            int nparamFact = 0;
            if (p->getType() == _STRUCT_) {
                IdentTable * fields = reinterpret_cast<IdentTable*>(p->getVal());
                while (fields->next != nullptr) {
                    nparamFact++;
                    fields = fields->next;
                }
            } else nparamFact = 1;

            paramsSum += nparamFact;
        }
        if (p->getType() == _STRUCT_) {
            IdentTable * fields = reinterpret_cast<IdentTable*>(p->getVal());
            while (fields->next != nullptr) {
                fields->setReg(true);
                fields = fields->next;
            }
        } else p->setReg(true);
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
            code >> c;
            char * stName = identificator();
            thisFunc->setType(_STRUCT_);
            thisFunc->setStruct(stName);
            //IdTable.pushStruct(stName);
        } else throw Obstacle(NO_TYPE);
    } else throw Obstacle(PROCEDURE);

    retTypes.push_back(std::make_pair(thisFunc->getType(), thisFunc->getStruct()));

    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '{') throw Obstacle(PROG_OPENBR);
    code >> c;

    defs();
    operations();

    retTypes.pop_back();
    
    if (c != '}') throw Obstacle(PROG_CLOSEBR); // никогда не будет исполнено
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
    
    int fieldSize = 1;
    char * stName = retTypes.back().second;
    if (retTypes.back().first == _STRUCT_) {
        fieldSize = StTable.getStruct(stName)->getFields().last()->getOrd();
    }
    type_t retFact = expr(&fieldSize, stName);

    if (retFact != retTypes.back().first) 
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
    int vars = 0;
    try {
        do {
            code >> c;
            if (r == nullptr) {
                r = variable();
                r->setMainTable(&IdTable);
            } else variable()->setMainTable(&IdTable);
            if ((c == ' ') || (c == '\n')) code >> c;
            vars++;
        } while (c == ',');

        if (r->isArray()) {
            IdentTable * p = r;
            while (vars--) {
                poliz.pushVal(p);
                int fs = 1;
                NEW_IDENT(allocSize, _INT_, nullptr, 
                    new int (p->getArray() * IdTable.secLast()->typeSize()), &fs)
                poliz.pushOp(p->getType(), _INT_, ALLOC);
                p = p->next;
            }
        }

    } catch(Obstacle & o) {
        IdTable.last()->setId(nullptr);
        ok = false;
        c.where();
        o.describe();
        c.cite(code);
        c.line++;
        revert(1); // исправление двоеточия
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

bool Parser::typeArray(void) {
    bool r = true;

    if (c != '[') return false;
    code >> c;

    if (c == '*') {
        // Динамический массив
        IdTable.last()->setArray();
    } else {
        // Статический массив
        int size = constInt();
        if (size <= 0) throw Obstacle(BAD_SIZE);
        IdTable.last()->setArray(size);
    }
    
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != ']') throw Obstacle(ARRAY_CLOSEBR);

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

    if ((c == ' ') || (c == '\n')) code >> c;
    bool arr = typeArray();
    if (r && !arr) revert(1);

    if (tmod && (!r))
        throw Obstacle(MODIF_WITHOUT_TYPE);

    return r;
}

/* Перед выполнением:
 * aab bba aca
 *     ^~~ <= курсор на первой букве слова
 * После выполнения:
 * aab bba aca
 *        ^ <= курсор за последней буквой слова (если true)
 * Если false, курсор на начальной позиции
 */ 
bool Parser::readWord(const char * word) {
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

std::vector<int> Parser::assign(IdentTable * lval, bool idle, int ford) {
    type_t lvtype = lval->getType();
    std::vector<int> pi;

    if ((lvtype == _STRUCT_) && (c == '{')) {
        IdentTable * fields = static_cast<IdentTable *>(lval->getVal());
        int size = fields->getSize();
        int start = poliz.getSize() - size;
        int end = poliz.getSize();
        lval->updateFieldOrds();
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
            if ((val->getType() != _STRUCT_) || (c != '{'))
                pi.push_back(ford + val->getFieldOrd());

            std::vector<int> pi2 = assign(val, idle, ford + val->getFieldOrd());
            pi.insert(pi.end(), pi2.begin(), pi2.end());
        } while (c == ',');
        if (c != '}') throw Obstacle(BAD_EXPR);
        code >> c;

        if ((ford == 0) && !lval->isArray())
            poliz.permutate(start, end, pi.size(), pi.data());
    } else {
        int fieldSize = unrollStruct(lval, -1, idle);
        char * strName = nullptr;

        std::vector<type_t> fieldTypes;
        if (lvtype == _STRUCT_) {
            strName = lval->getStruct();
            fieldTypes = StTable.getTypes(lval->getStruct());
        }

        /* Мы точно знаем количество полей -- fieldSize.
         * Однако для предотвращения перепаковки resolveIdentificator
         * устанавливает fieldSize = 0. Из-за этого пропадают
         * инструкции ASSIGN, а UNPACK получает аргументом 0.
         */
        int dummyFS = fieldSize;
        type_t exop = expr(&dummyFS, strName);
        expressionType(lvtype, exop, ASSIGN);
        
        if (lval->isArray()) {
            int fs = 1;
            NEW_IDENT(nfields, _INT_, nullptr, new int (fieldSize), &fs)
            poliz.pushOp(_NONE_, _INT_, UNPACK);
        }

        if (lvtype == _STRUCT_) {
            for (int i = 0; i < fieldSize; i++) {
                type_t t = fieldTypes[i];
                // В expr => constExpr проверяется корректность типа
                poliz.pushOp(t, t, ASSIGN);
            }
        } else {
            poliz.pushOp(lvtype, exop, ASSIGN);
        }

        if (exop == _STRUCT_)
            repack(fieldSize);
        
        
        if ((c == ' ') || (c == '\n')) code >> c;
    }

    return pi;
}

int Parser::unrollStruct(IdentTable * lval, int ord, bool idle) {
    int fieldSize = 0;
    if (lval->getType() == _STRUCT_){
        IdentTable * fields = static_cast<IdentTable *>(lval->getVal());
        while (fields->next != nullptr) {
            int newOrd = -1;
            if (fields->isReg())
                newOrd = fields->getOrd();
            fieldSize += unrollStruct(fields, newOrd, idle);
            fields = fields->next;
        }
        #ifdef DEBUG
        std::cout << "РАЗМЕР " << fieldSize << std::endl;
        #endif
    } else {
        fieldSize = 1;
        if (!idle)
            poliz.pushVal(lval);
    }
    return fieldSize;
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
    } while (C_IS_ALPHA && (i < MAXIDENT - 1));
    if (i >=  MAXIDENT - 1)
        throw Obstacle(TOO_BIG_IDENT);
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
    else if (readWord("writeln")) writelnOp();
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
        int chpoint = c.pos;
        char * name = identificator();
        if ((c == ' ') || (c == '\n')) code >> c;
        if (c == ':') {
            code >> c;
            saveLabel(name, poliz.getSize());
            operation();
        } else {
            delete [] name;
            revert(c.pos - chpoint);
            IdentTable * lval = resolveIdentificator();
            if (c == '=') {
                code >> c;
                std::vector<int> pi = assign(lval, true);
                if (lval->isArray())
                    poliz.combine(pi);
                if (c != ';') throw Obstacle(SEMICOLON);
                code >> c;
            } else throw Obstacle(BAD_OPERATOR);
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
    inThread = true;

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

    operations();
    
    poliz.pushOp(_NONE_, _NONE_, STOP);

    if (c != '}')
        throw Obstacle(PROG_CLOSEBR);
    
    progOffset->setVal(new int (poliz.getSize()));

    code >> c;
    inThread = false;
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
        delete static_cast<int *>(existinglab->getVal());
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

type_t Parser::expr(int * fieldSize, char * structName) {
    type_t r = _NONE_;

    assert(fieldSize != nullptr);

    r = andExpr(fieldSize, structName);

    while (true) {
        if (readWord("or")) {
            code >> c;
            type_t rval = andExpr(fieldSize, structName);

            if ((r == _STRUCT_) || (rval == _STRUCT_)) {
            if (*fieldSize == 0) {
                int rp = StTable.getStruct(structName)->getFields().getSize();
                int fs = 1;
                NEW_IDENT(repN, _INT_, nullptr, new int (rp), &fs)
                poliz.pushOp(_NONE_, _INT_, UNPACK);
            }
            handleStruct(r, rval, LOR, fieldSize, structName);
            repack(*fieldSize);
        } else
            poliz.pushOp(r, rval, LOR);

            r = expressionType(r, rval, LOR);
        } else break;
    }

    return r;
}

type_t Parser::andExpr(int * fieldSize, char * structName) {
    type_t r = _NONE_;
    
    r = boolExpr(fieldSize, structName);

    while (true) {
        if (readWord("and")) {
            code >> c;
            type_t rval = boolExpr(fieldSize, structName);
            
            if ((r == _STRUCT_) || (rval == _STRUCT_)) {
            if (*fieldSize == 0) {
                int rp = StTable.getStruct(structName)->getFields().getSize();
                int fs = 1;
                NEW_IDENT(repN, _INT_, nullptr, new int (rp), &fs)
                poliz.pushOp(_NONE_, _INT_, UNPACK);
            }
            handleStruct(r, rval, LAND, fieldSize, structName);
            repack(*fieldSize);
        } else
            poliz.pushOp(r, rval, LAND);

            r = expressionType(r, rval, LAND);
        } else break;
    }
    return r;
}

type_t Parser::boolExpr(int * fieldSize, char * structName) {
    type_t r = _NONE_;

    r = add(fieldSize, structName);
    
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
        type_t rval = add(fieldSize, structName);
        
        if ((r == _STRUCT_) || (rval == _STRUCT_)) {
            if (*fieldSize == 0) {
                int rp = StTable.getStruct(structName)->getFields().getSize();
                int fs = 1;
                NEW_IDENT(repN, _INT_, nullptr, new int (rp), &fs)
                poliz.pushOp(_NONE_, _INT_, UNPACK);
            }
            handleStruct(r, rval, op, fieldSize, structName);
            repack(*fieldSize);
        } else
            poliz.pushOp(r, rval, op);

        r = expressionType(r, rval, op);
    }

    return r;
}

type_t Parser::add(int * fieldSize, char * structName) {
    bool exit = false;
    bool inverse = false;
    if (c == '+') code >> c;
    if (c == '-') {
        inverse = true;
        code >> c;
    }
    type_t r = mul(fieldSize, structName);
    operation_t op;

    while (true) {

        switch (c.symbol()) {
            case '+': op = PLUS; break;
            case '-': op = MINUS; break;
            default: exit = true;
        }
        if (exit) break;
        code >> c;
        type_t rval = mul(fieldSize, structName);
        
        if ((r == _STRUCT_) || (rval == _STRUCT_)) {
            if (*fieldSize == 0) {
                int rp = StTable.getStruct(structName)->getFields().getSize();
                int fs = 1;
                NEW_IDENT(repN, _INT_, nullptr, new int (rp), &fs)
                poliz.pushOp(_NONE_, _INT_, UNPACK);
            }
            handleStruct(r, rval, op, fieldSize, structName);
            repack(*fieldSize);
        } else
            poliz.pushOp(r, rval, op);

        r = expressionType(r, rval, op);
    }

    if (inverse) {
        if (r == _STRUCT_) {
            handleStruct(_NONE_, r, INV, fieldSize, structName);
            repack(*fieldSize);
            r = expressionType(_NONE_, r, INV);
        }
    }

    return r;
}
//FIXME: fieldSize лишний??
void Parser::handleStruct(
    type_t lval, type_t rval, operation_t op, int * fieldSize, char * structName) {
   
   /* Если в rval лежит простой тип, то его можно просто скопировать
    * для всех остальных полей в lval. В случае, если rval -- структура,
    * то нужно аккуратно обойти все её поля. Причём constExpr гарантирует,
    * что структуры lval и rval идентичны, поэтому мы можем взять имя
    * lval (т.е. structName) для определения структуры rval.
    */
    assert((lval == _STRUCT_) || (rval == _STRUCT_));

    std::vector<type_t> types = StTable.getTypes(structName);
    int newFieldSize = types.size();
    type_t ltype, rtype;
    for (int i = newFieldSize - 1; i >= 0; i--) {
        if (lval == _STRUCT_) ltype = types[i];
        else ltype = lval;
        if (rval == _STRUCT_) rtype = types[i];
        else rtype = rval;

        if ((ltype == _STRUCT_) || (rtype == _STRUCT_))
            handleStruct(ltype, rtype, op, &newFieldSize, structName);
        else
            poliz.pushOp(ltype, rtype, op);
    }
}

type_t Parser::mul(int * fieldSize, char * structName) {
    bool exit = false;
    type_t r = constExpr(fieldSize, structName);
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
        type_t rval = constExpr(fieldSize, structName);
        
        if ((r == _STRUCT_) || (rval == _STRUCT_)) {
            if (*fieldSize == 0) {
                int rp = StTable.getStruct(structName)->getFields().getSize();
                int fs = 1;
                NEW_IDENT(repN, _INT_, nullptr, new int (rp), &fs)
                poliz.pushOp(_NONE_, _INT_, UNPACK);
            }
            handleStruct(r, rval, op, fieldSize, structName);
            repack(*fieldSize);
        } else
            poliz.pushOp(r, rval, op);

        r = expressionType(r, rval, op);

    }

    return r;
}

void Parser::repack(int fieldSize) {
    operation_t op = poliz.rgetOpcode(fieldSize);
    if (op == UNPACK) // Костыль??
        return;

    int POLIZsteps[2 * fieldSize];
    for (int i = 0; i < fieldSize * 2; i++)
        POLIZsteps[i] = 1;

    POLIZ buff[2], opBuff[2];

    /* Каков смысл steps? В нём хранится количество элементов ПОЛИЗА,
     * которое нужно достать для каждого поля. Вначале идёт левая,
     * потом правая ветка.
     */
    int temp_iter = fieldSize;
    int i;
    while (((i = poliz.getSize()) > 0) && poliz.getEB()[i - 1] && temp_iter) {
        pslot oper = poliz.getProg()[i - 1];
        opBuff[1].push(oper, poliz.getEB()[i - 1]);
        poliz.pop();
        temp_iter--;
    }
    for (int buffIter = 1; buffIter >= 0; buffIter--) {
        for (int nfield = fieldSize; nfield > 0; nfield--) {
            int stepIdx = buffIter + (nfield - 1) * 2;
            int fields = POLIZsteps[stepIdx];
            POLIZsteps[stepIdx] = 0;
            while (fields) {
                i = poliz.getSize();
                bool ebit = poliz.getEB()[i - 1];
                pslot oper = poliz.getProg()[i - 1];
                if (ebit) {
                    int nops = operands(oper.opcode);
                    fields += nops;
                }
                
                buff[buffIter].push(oper, ebit);
                POLIZsteps[stepIdx] += 1;
                
                poliz.pop();
                fields--;
            }
        }
    }

    for (int step = 0; step < fieldSize * 2; step++) {
        while (POLIZsteps[step]) {
            int  bi   = buff[step % 2].getSize();
            bool ebit = buff[step % 2].getEB()[bi - 1];
            pslot oper   = buff[step % 2].getProg()[bi - 1];
            buff[step % 2].pop();
            poliz.push(oper, ebit);
            POLIZsteps[step] -= 1;
        }
        int  bi   = opBuff[step % 2].getSize();
        if (bi > 0) {
            bool ebit = opBuff[step % 2].getEB()[bi - 1];
            pslot oper   = opBuff[step % 2].getProg()[bi - 1];
            opBuff[step % 2].pop();
            poliz.push(oper, ebit);
        }
    }
}

int Parser::getFieldShift(IdentTable * val) {
    int shift = 0;
    char * name = val->getStruct();
    if (name == nullptr) return 0;
    IdentTable *f = &StTable.getStruct(name)->getFields();
    while (std::strcmp(f->getId(), val->getId()) != 0) {
        shift += f->typeSize();
        f = f->next;
    }
    return shift;
}

IdentTable * Parser::resolveIdentificator(int * fieldSize, char * structName, IdentTable * baseVal) {
    type_t r;

    IdentTable * val = getFieldInStruct(baseVal);
    r = val->getType();
    
    if (c == '(') { // Вызов функции
        if (! val->isFunc())
            throw Obstacle(NOT_CALLABLE);
        callIdent(val);
        if (val->getStruct() != nullptr) {
            IdentTable fields = StTable.getStruct(val->getStruct())->getFields();
            int size = fields.secLast()->getOrd() + 1;
            if (fieldSize != nullptr) {
                *fieldSize = size;
            }
            // GET FROM STACK {size, fieldNum}
            
        }
    } else {
        if (val->isFunc())
            throw Obstacle(CALLABLE);

        if (c == '[') { // Массив
            if (!val->isArray()) throw Obstacle(NOT_ARRAY);
            poliz.pushVal(val);

            code >> c;

            int fs = 1;
            type_t index = expr(&fs);
            if (index != _INT_) throw Obstacle(BAD_INDEX);
            NEW_IDENT(stSize, _INT_, nullptr, new int (val->typeSize()), &fs)
            poliz.pushOp(_INT_, _INT_, MUL);
            poliz.pushOp(r, _INT_, DEREF);

            if (c != ']') throw Obstacle(ARRAY_CLOSEBR);
            code >> c;

            /* Это означает, что машине надо получить поле, которое 
             * вычисляется по смещению относительно начала структуры.
             */
            baseVal = val;
            while (c == '.') {
                code >> c;
                char * name = identificator();
                IdentTable * fields = static_cast<IdentTable*>(baseVal->getVal());
                baseVal = fields->getIT(name);

                int fs = 1;
                int fshift = getFieldShift(baseVal);
                if (fshift != 0) {
                    NEW_IDENT(shift, _INT_, nullptr, new int (fshift), &fs)
                    poliz.pushOp(r, _INT_, DEREF);
                }
            }

            // Теперь возможно опять () или []
            val = resolveIdentificator(fieldSize, structName, baseVal);

        } else {
            if (r == _STRUCT_) {
                if ((structName != nullptr) && (strcmp(val->getStruct(), structName) != 0))
                    throw Obstacle(EXPR_BAD_TYPE);

                if (baseVal != nullptr) {
                    /* Распаковка элементов из памяти
                     * 1) адрес первого элемента уже есть на стеке
                     * 2) надо его скопировать и прибавить (DEREF)
                     * размер первого поля (получим адрес второго)
                     * 3) и так далее
                     * В результате на стеке будут в нужном порядке
                     * лежать адреса каждого из полей.
                     */
                    IdentTable * fields = static_cast<IdentTable *>(val->getVal());

                    if (fieldSize != nullptr)
                        *fieldSize = 0;
                        //*fieldSize = fields->secLast()->getOrd() + 1;
                    
                    int fs = 1;
                    int fshift = 0;
                    //TODO: Структура в структуре -- нужно
                    // раскрывать рекурсивно.
                    while(fields->next->next != nullptr) {
                        poliz.pushOp(_NONE_, _NONE_, COPY);
                        fshift += fields->typeSize();
                        NEW_IDENT(shift, _INT_, nullptr, new int (fshift), &fs)
                        poliz.pushOp(r, _INT_, DEREF);
                        fields = fields->next;
                    }

                } else {
                    int fields = unrollStruct(val);
                    if (fieldSize != nullptr) {
                        *fieldSize = fields;
                    }
                }
            } else if (baseVal == nullptr) poliz.pushVal(val);
        }
    }

    return val;
}

type_t Parser::constExpr(int * fieldSize, char * structName) {
    type_t r;

    if (readWord("true")) {
        r = _BOOLEAN_;
        NEW_IDENT(val, _BOOLEAN_, nullptr, new bool (true), fieldSize)
    } else if (readWord("false")) {
        r = _BOOLEAN_;
        NEW_IDENT(val, _BOOLEAN_, nullptr, new bool (false), fieldSize)
    } else if (readWord("not")) {
        type_t val = constExpr(fieldSize, structName);
        r = expressionType(_NONE_, val, LNOT);
        for (int i = 0; i < *fieldSize; i++)
            poliz.pushOp(_NONE_, val, LNOT);
    } else if (c == '(') {
        code >> c;
        r = expr(fieldSize, structName);
        if (c != ')') throw Obstacle(EXPR_CLOSEBR);
        code >>= c;
    } else {
        
        if (c == '\"') {
            r = _STRING_;
            char * x = constString();
            NEW_IDENT(val, _STRING_, nullptr, x, fieldSize)
        } else {
            int start = code.tellg();
            try {
                r = _REAL_;
                float x = constReal();
                NEW_IDENT(val, _REAL_, nullptr, new float (x), fieldSize)
            } catch (Obstacle & o) {
                code.seekg(start - 1);
                code >>= c;
                if (o.r != BAD_INT) {
                    r = _INT_;
                    int x = constInt();
                    NEW_IDENT(val, _INT_, nullptr, new int (x), fieldSize)
                } else {
                    r = resolveIdentificator(fieldSize, structName)->getType();
                }
            }
        }
    }

    if ((c == ' ') || (c == '\n')) code >> c;

    return r;
}

IdentTable * Parser::getFieldInStruct(IdentTable * baseVal) {
    if (baseVal == nullptr) {
        char * name = identificator();
        baseVal = IdTable.getIT(name);
        if ((c == ' ') || (c == '\n')) code >> c;
    }

    IdentTable * val = baseVal;

    while (c == '.') { // Добираемся до поля структуры
        code >> c;
        char * name = identificator();
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
    int paramCountFact = 0;
    while (c != ')') {
        if (fields == nullptr)
            throw Obstacle(TOO_MUCH_PARAMS);
        if (c == ',') code >> c;

        int fieldSize = 1;
        char * structName = nullptr;

        if (fields->getType() == _STRUCT_) {
            fieldSize = fields->last()->getOrd();
            structName = fields->getStruct();
        }

        type_t exprType = expr(&fieldSize, structName);
        if (fields->getType() != exprType)
            throw Obstacle(EXPR_BAD_TYPE);
        fields = fields->next;
        paramCount += fieldSize;
        paramCountFact++;
    }

    code >> c;
    if (paramCountFact != val->getParams())
        throw Obstacle(BAD_PARAMS_COUNT);
    int fieldSize = 1;
    NEW_IDENT(params, _INT_, nullptr, new int (paramCount), &fieldSize)
    NEW_IDENT(callable, _LABEL_, nullptr, new int (val->getOffset()), &fieldSize)
    poliz.pushOp(_INT_, _LABEL_, CALL);
    
    if (val->getType() == _STRUCT_) {
        int fnum = StTable.getStruct(val->getStruct())->getFields().last()->getOrd();
        IdTable.pushType(_INT_);
        IdTable.pushVal(new int (fnum));
        poliz.pushVal(IdTable.confirm());
        poliz.pushOp(_NONE_, _INT_, UNPACK);
    }
}

void Parser::condOp(void) {
    if ((c == ' ') || (c == '\n')) code >> c;
    if (c != '(')
        throw Obstacle(BAD_PARAMS_OPBR);
    code >> c;

    int fieldSize = 1;
    type_t r = expr(&fieldSize);

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
    char * name;

    if (type()) {
        // переменная не описана
        code >> c;
        name = identificator();
        IdTable.pushId(name);
        lval = IdTable.confirm();
    } else {
        // переменная описана
        name = identificator();
        lval = IdTable.getIT(name);
    }

    if ((c == ' ') || (c == '\n')) code >> c;

    while (c == '.') {
        code >> c;
        name = identificator();
        IdentTable * fields = static_cast<IdentTable *>(lval->getVal());
        lval = fields->getIT(name);
        if ((c == ' ') || (c == '\n')) code >> c;
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
    cp = cycleparam(); // начальное выражение
    /*
    try{ cp = cycleparam(); } // начальное выражение
    catch(Obstacle & o){
        o.describe();
    }
    */
    
    if (c != ';')
        throw Obstacle(SEMICOLON);

    IdTable.pushType(_LABEL_);
    IdTable.pushVal( new int (poliz.getSize()) );
    IdentTable * condexpr = IdTable.confirm();
    code >> c;

    int fieldSize = 1;
    type_t e2 = expr(&fieldSize); // условие продолжения

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

    if ((cp->next->getId() == nullptr)) {
        cp->setId(nullptr); // Эта переменная вне цикла не определена.
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
                    types.push_back(_LABEL_);
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
            int fieldSize = 1;
            int p = c.pos;
            try {
                types.push_back(constExpr(&fieldSize));
            } catch (Obstacle & o) {
                revert(c.pos - p - 1);
                IdentTable * ident = IdTable.getIT(identificator());
                if (ident->isFunc()) {
                    IdTable.pushType(_LABEL_);
                    IdTable.pushVal( new int (ident->getOffset()) );
                    IdentTable * label = IdTable.confirm();
                    poliz.pushVal(label);
                    types.push_back(_LABEL_);
                } else {
                    poliz.pushVal(static_cast<IdentTable *>(ident->getVal()));
                    types.push_back(ident->getType());
                }
            }
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

    int fieldSize = 1;
    type_t e2 = expr(&fieldSize); // условие продолжения

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
        int fieldSize = 1;
        type_t exop = expr(&fieldSize);
        poliz.pushOp(_NONE_, exop, WRITE);
    } while (c == ',');

    if (c != ')')
        throw Obstacle(BAD_PARAMS_CLBR);

    code >> c;

    if (c != ';')
        throw Obstacle(SEMICOLON);

    code >> c;
}

void Parser::writelnOp(void) {
    writeOp();
    poliz.pushOp(_NONE_, _NONE_, ENDL);
}

void Parser::giveBIN(const char * filename, bool optimize, bool printPoliz, bool verbose) {
    bin.open(filename, std::ios_base::binary | std::ios_base::out);
    int x = 0;
    bin.write((char*)&x, sizeof(int)); // Сюда запишем адрес начала команд

    IdentTable * ITp = &IdTable;

    if (optimize) {
        Optimizer opt(&IdTable, &poliz, verbose);
        ITp = opt.optimize();
    }

    if (printPoliz) {
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
    pslot * prog = poliz.getProg();
    bool * execBit = poliz.getEB();

    for (int i = 0; i < b; i++) {
        if (execBit[i]) {
            bin.write((const char *)(prog + i), PSLOT_PROG);
        } else {
            int offset =  poliz.getVal(i)->getOffset();
            bin.write((char*)&offset, sizeof(int));
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
    c.pos -= x + 1;
}