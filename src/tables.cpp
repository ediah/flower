#include <iostream>
#include <cstring>
#include "parser.hpp"
#include "tables.hpp"
#include "obstacle.hpp"
#include "exprtype.hpp"

IdentTable::IdentTable(const IdentTable & templateIT) {
    valType = templateIT.valType;
    def = templateIT.def;
    ord = templateIT.ord;
    offset = templateIT.offset;

    if (templateIT.structName != nullptr) {
        structName = new char[strnlen(templateIT.structName, MAXIDENT) + 1];
        memccpy(structName, templateIT.structName, '\0', strnlen(templateIT.structName, MAXIDENT) + 1);
    } else structName = nullptr;

    if (templateIT.name != nullptr) {
        name = new char[strnlen(templateIT.name, MAXIDENT) + 1];
        memccpy(name, templateIT.name, '\0', strnlen(templateIT.name, MAXIDENT) + 1);
    } else name = nullptr;
    
    if (templateIT.valType == _STRUCT_)
        val = new IdentTable( * (IdentTable *)templateIT.val);
    else val = templateIT.val;

    if (templateIT.next->getType() != _NONE_)
        next = new IdentTable(*templateIT.next);
    else next = new IdentTable;
}

IdentTable & IdentTable::operator=(const IdentTable & templateIT) {
    valType = templateIT.valType;
    def = templateIT.def;
    ord = templateIT.ord;
    offset = templateIT.offset;

    if (templateIT.structName != nullptr) {
        structName = new char[strnlen(templateIT.structName, MAXIDENT) + 1];
        memccpy(structName, templateIT.structName, '\0', strnlen(templateIT.structName, MAXIDENT) + 1);
    } else structName = nullptr;

    if (templateIT.name != nullptr) {
        name = new char[strnlen(templateIT.name, MAXIDENT) + 1];
        memccpy(name, templateIT.name, '\0', strnlen(templateIT.name, MAXIDENT) + 1);
    } else name = nullptr;
    
    if (templateIT.valType == _STRUCT_)
        val = new IdentTable( * (IdentTable *)templateIT.val);
    else val = templateIT.val;

    if (templateIT.next->getType() != _NONE_)
        next = new IdentTable(*templateIT.next);
    else next = new IdentTable;
    
    return *this;
}

// Получаем последний объект списка
IdentTable * IdentTable::last(void) {
    IdentTable * p = this;
    while (p->next != nullptr) p = p->next;
    return p;
}

// Сохраняем идентификатор переменной
void IdentTable::pushId(char* ident) {
    IdentTable * p = this;
    while (p->next != nullptr) {
        if ((p->name != nullptr) && (ident != nullptr) && (strcmp(p->name, ident) == 0)) {
            delete [] ident;
            throw Obstacle(IDENT_DUP);
        }
        p = p->next;
    }
    p->name = ident;
}

// Сохраняем тип переменной
void IdentTable::pushType(type_t t) {
    last()->valType = t;
}

void IdentTable::pushStruct(char * name) {
    last()->structName = name;
}

// Сохраняем значение переменной
void IdentTable::pushVal(void* v) {
    IdentTable * l = last();
    l->val = v;
    l->def = true;
}

// Всё, что могло быть известно, мы узнали. Переключаемся на новый объект.
IdentTable * IdentTable::confirm(void) {
    IdentTable * l = last();

    #ifdef DEBUG
    std::cout << "Создан новый объект: ";
    l->whoami();
    std::cout << std::endl;
    #endif

    IdentTable * newIdent = new IdentTable;
    l->next = newIdent;
    l->next->ord = l->ord + 1;

    return l;
}

// Дублирование типа для выражений вида:
// int a, b, c;
void IdentTable::dupType(void) {
    IdentTable * p = this;
    if (last()->valType == _NONE_) {
        while (p->next->next != nullptr) p = p->next;
        p->next->valType = p->valType;
        if (p->structName != nullptr) {
            p->next->structName = new char[strnlen(p->structName, MAXIDENT) + 1];
            memccpy(p->next->structName, p->structName, '\0', strnlen(p->structName, MAXIDENT) + 1);
        }
        if (p->valType == _STRUCT_) {
            p->next->val = new IdentTable(* (IdentTable *) p->val);
            p = (IdentTable *) p->next->val;
            while (p->next != nullptr) {
                if (p->valType != _STRUCT_)
                    p->val = nullptr;
                p->def = false;
                p = p->next;
            }
        }
    }
}

type_t IdentTable::getType(void) const {
    return valType;
}

void IdentTable::setType(type_t newType) {
    valType = newType;
}

char * IdentTable::getStruct(void) const {
    return structName;
}

void IdentTable::setFunc(void) {
    func = true;
}

bool IdentTable::isFunc(void) const {
    return func;
}

void IdentTable::setOrd(int x) {
    ord = x;
}

void IdentTable::onReg(void) {
    reg = true;
}

bool IdentTable::isReg(void) const {
    return reg;
}

void IdentTable::setParams(int x) {
    params = x;
}

int IdentTable::getParams(void) const {
    return params;
}

IdentTable * IdentTable::getIT(char * name, bool autodel) {
    IdentTable * p = this;

    while ((p->name == nullptr) || (strcmp(p->name, name) != 0)) {
        if (p->next->valType != _NONE_) p = p->next;
        else {
            if (autodel) delete [] name;
            throw Obstacle(IDENT_NOT_DEF);
        }
    }

    delete [] name;

    return p;
}

void IdentTable::whoami(void) {

    std::cout << '{' << typetostr(valType) << ' ';
    if (valType == _STRUCT_) {
        std::cout << structName << ' ';
        if (name != nullptr) std::cout << name;
        else std::cout << "? ";
        if (func) std::cout << "FUNCTION ";
        std::cout << " = {";
        IdentTable * fields = (IdentTable *) val;
        while (fields->next != nullptr) {
            fields->whoami();
            fields = fields->next;
        }
        std::cout << " }";
    } else {
        if (func) std::cout << "FUNCTION ";
        if (name != nullptr)
            std::cout << name << " = ";
        if (def) {
            switch (valType) {
                case _INT_: case _LABEL_:
                    std::cout << * (int*) val; break;
                case _REAL_:
                    std::cout << * (float*) val; break;
                case _STRING_:
                    std::cout << (char*) val; break;
                case _BOOLEAN_:
                    std::cout << * (bool*) val; break;
                default: 
                    std::cout << "[неизвестен]"; break;
            }
        } else std::cout << "(не определён)";
    }
    std::cout << '}';

}

void IdentTable::repr(void) {
    IdentTable * p = this;
    std::cout << "IdentTable:" << std::endl;
    while (p->next != nullptr) {
        std::cout << p << " ";
        p->whoami();
        std::cout << std::endl;
        p = p->next;
    }
}

void IdentTable::setId(char * name) {
    if (this->name != nullptr) delete [] this->name;
    this->name = name;
}

char * IdentTable::getId(void) const {
    return this->name;
}

void * IdentTable::getVal(void) {
    return val;
}

void IdentTable::setVal(void * val) {
    this->val = val;
    def = true;
}

void IdentTable::setOffset(int x) {
    offset = x;
}

int IdentTable::getOffset(void) const {
    return offset;
}

int IdentTable::getOrd(void) const {
    return ord;
}

void IdentTable::writeValToStream(std::ostream & s) {
    if (!def) {
        switch (valType) {
            case _INT_: case _LABEL_:
                val = new int (0); break;
            case _REAL_:
                val = new float (0); break;
            case _STRING_:
                val = new char[1];
                ( (char*)val )[0] = '\0'; 
                break;
            case _BOOLEAN_:
                val = new bool (false); break;
            case _STRUCT_: break;
            default:
                throw Obstacle(PANIC);
        }
        def = true;
    }
    IdentTable * ITp;
    switch (valType) {
        case _INT_: case _LABEL_:
            s.write((char*)val, sizeof(int)); break;
        case _REAL_:
            s.write((char*)val, sizeof(float)); break;
        case _STRING_:
            s.write("\0\0\0\0\0\0\0\0", sizeof(void*));
            s.write((char*)val, sizeof(char) * (strlen((char*)val) + 1));
            break;
        case _BOOLEAN_:
            s.write((char*)val, sizeof(bool)); break;
        case _STRUCT_:
            ITp = (IdentTable*)val;
            while (ITp->next != nullptr) {
                ITp->setOffset((int)s.tellp());
                ITp->writeValToStream(s);
                ITp = ITp->next;
            }
            break;
        default:
            throw Obstacle(PANIC);
    }
}

IdentTable::~IdentTable() {
    if (name != nullptr) delete [] name;

    if ((val != nullptr) && (!func)) {
        switch (valType) {
            case _INT_: case _LABEL_:
                delete (int*) val; break;
            case _REAL_:
                delete (float*) val; break;
            case _BOOLEAN_:
                delete (bool*) val; break;
            case _STRING_: 
                delete [] (char*)val; break;
            case _STRUCT_: 
                delete (IdentTable*)val; break;
            default: break;
        }
    }
    if (structName != nullptr) delete [] structName;

    if (next != nullptr) delete next;
}

StructTable * StructTable::last(void) {
    StructTable * p = this;
    while (p->next != nullptr) p = p->next;
    return p;
}

void StructTable::pushName(char * name) {
    last()->name = name;
}

void StructTable::pushField(type_t type, char * name, char * structName) {
    StructTable * l = last();
    l->fields.pushType(type);
    l->fields.pushId(name);
    if (structName != nullptr) {
        l->fields.pushStruct(structName);
        IdentTable & templateIT = getStruct(structName)->getFields();
        l->fields.pushVal(new IdentTable(templateIT));
    } else {
        if (l->fields.getStruct() != nullptr) {
            std::cout << "Структура не равна нулю\n" << l->fields.getStruct();
            std::cout << std::endl;
        }
    }
    l->fields.confirm();
}

StructTable * StructTable::confirm(void) {
    StructTable * p = last();

    #ifdef DEBUG
    std::cout << "Описана новая структура: " << p->name << '{';
    IdentTable * fields = & p->fields;
    fields->whoami();
    while (fields->next->next != nullptr) {
        std::cout << ", ";
        fields = fields->next;
        fields->whoami();
    }
    std::cout << '}' << std::endl;
    #endif

    p->next = new StructTable;

    return p;
}

StructTable * StructTable::getStruct(char * name) {
    StructTable * p = this;

    while (p != nullptr) {
        if (strcmp(p->name, name) == 0) break;
        p = p->next;
    }

    if (p == nullptr)
        throw Obstacle(STRUCT_UNDEF);

    return p;
}

IdentTable & StructTable::getFields(void) {
    return fields;
}

StructTable::~StructTable(void) {
    if (name != nullptr) delete [] name;
    if (next != nullptr) delete next;
}
