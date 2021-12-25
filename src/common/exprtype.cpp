#include <iostream>
#include "common/obstacle.hpp"
#include "common/exprtype.hpp"

type_t expressionType(type_t t1, type_t t2, operation_t o) {
    type_t r = _NONE_;

    #ifdef DEBUG
    std::cout << "Проверка " << typetostr(t1) << " " << typetostr(t2) << std::endl;
    #endif

    switch (o) {
        case PLUS:
            switch (t1) {
                // INT PLUS INT  = INT
                // INT PLUS REAL = REAL
                case _INT_:
                    r = t2;
                    if ((t2 != _INT_) && (t2 != _REAL_) && (t2 != _STRUCT_)) 
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                // REAL PLUS INT  = REAL
                // REAL PLUS REAL = REAL
                case _REAL_:
                    r = _REAL_;
                    if ((t2 != _INT_) && (t2 != _REAL_))
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                // STRING PLUS STRING = STRING
                case _STRING_:
                    r = _STRING_;
                    if (t2 != _STRING_) 
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;
                
                case _STRUCT_:
                    r = _STRUCT_;
                    break;

                // _ PLUS INT  = INT
                // _ PLUS REAL = REAL
                case _NONE_:
                    r = t2;
                    if ((t2 != _INT_) && (t2 != _REAL_))
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                default:
                    throw Obstacle(EXPR_BAD_TYPE);
            }

            break;

        case MINUS:
            switch (t1) {
                // INT MINUS INT  = INT
                // INT MINUS REAL = REAL
                case _INT_:
                    r = t2;
                    if ((t2 != _INT_) && (t2 != _REAL_) && (t2 != _STRUCT_))
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                // REAL MINUS INT  = REAL
                // REAL MINUS REAL = REAL
                case _REAL_:
                    r = _REAL_;
                    if ((t2 != _INT_) && (t2 != _REAL_)) 
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                // _ MINUS INT  = REAL
                // _ MINUS REAL = REAL
                case _NONE_:
                    r = t2;
                    if ((t2 != _INT_) && (t2 != _REAL_)) 
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                default:
                    throw Obstacle(EXPR_BAD_TYPE);
            }

            break;
        
        case MUL: case DIV:
            switch (t1) {
                // INT MUL INT  = INT
                // INT MUL REAL = REAL
                // INT DIV INT  = INT
                // INT DIV REAL = REAL
                case _INT_:
                    r = t2;
                    if ((t2 != _INT_) && (t2 != _REAL_) && (t2 != _STRUCT_))
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                // REAL MUL INT  = REAL
                // REAL MUL REAL = REAL
                // REAL DIV INT  = REAL
                // REAL DIV REAL = REAL
                case _REAL_:
                    r = _REAL_;
                    if ((t2 != _INT_) && (t2 != _REAL_)) 
                        throw Obstacle(EXPR_BAD_TYPE);
                    break;

                default:
                    throw Obstacle(EXPR_BAD_TYPE);
            }

            break;

        // INT MOD INT = INT
        // INT MOD STRUCT = STRUCT
        case MOD:
            if (t1 != _INT_)
                throw Obstacle(EXPR_BAD_TYPE);

            if (t2 == _INT_)
                r = _INT_;
            else if (t2 == _STRUCT_)
                r = _STRUCT_;
            else
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // _ INV INT     = INT
        // _ INV REAL    = REAL
        // _ INV BOOLEAN = BOOLEAN
        case INV:
            r = t2;
            if (t1 != _NONE_)
                throw Obstacle(EXPR_BAD_TYPE);
            if ((t2 != _INT_) && (t2 != _REAL_) && (t2 != _BOOLEAN_)) 
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // INT    LESS INT    = BOOLEAN
        // INT    LESS REAL   = BOOLEAN
        // REAL   LESS INT    = BOOLEAN
        // REAL   LESS REAL   = BOOLEAN
        // INT    GRTR INT    = BOOLEAN
        // INT    GRTR REAL   = BOOLEAN
        // REAL   GRTR INT    = BOOLEAN
        // REAL   GRTR REAL   = BOOLEAN
        // INT    EQ   INT    = BOOLEAN
        // INT    EQ   REAL   = BOOLEAN
        // REAL   EQ   INT    = BOOLEAN
        // REAL   EQ   REAL   = BOOLEAN
        // INT    NEQ  INT    = BOOLEAN
        // INT    NEQ  REAL   = BOOLEAN
        // REAL   NEQ  INT    = BOOLEAN
        // REAL   NEQ  REAL   = BOOLEAN
        // STRING LESS STRING = BOOLEAN
        // STRING GRTR STRING = BOOLEAN
        // STRING EQ   STRING = BOOLEAN
        // STRING NEQ  STRING = BOOLEAN
        case LESS: case GRTR: case EQ: case NEQ:
            r = _BOOLEAN_;
            if (((t1 == _INT_) || (t1 == _REAL_)) && (t2 != _INT_) && (t2 != _REAL_))
                throw Obstacle(EXPR_BAD_TYPE);
            if ((t1 == _STRING_) && (t2 != _STRING_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // INT  LESSEQ INT  = BOOLEAN
        // INT  LESSEQ REAL = BOOLEAN
        // REAL LESSEQ INT  = BOOLEAN
        // REAL LESSEQ REAL = BOOLEAN
        // INT  GRTREQ INT  = BOOLEAN
        // INT  GRTREQ REAL = BOOLEAN
        // REAL GRTREQ INT  = BOOLEAN
        // REAL GRTREQ REAL = BOOLEAN
        case LESSEQ: case GRTREQ:
            r = _BOOLEAN_;
            if (((t1 != _INT_) && (t1 != _REAL_)) || ((t2 != _INT_) && (t2 != _REAL_))) 
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // BOOLEAN LOR  BOOLEAN = BOOLEAN
        // BOOLEAN LAND BOOLEAN = BOOLEAN
        case LOR: case LAND:
            r = _BOOLEAN_;
            if ((t1 != _BOOLEAN_) || (t2 != _BOOLEAN_)) 
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // _ LNOT BOOLEAN = BOOLEAN
        case LNOT:
            r = _BOOLEAN_;
            if ((t1 != _NONE_) || (t2 != _BOOLEAN_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // INT     ASSIGN INT     = INT
        // INT     ASSIGN REAL    = INT
        // REAL    ASSIGN INT     = REAL
        // REAL    ASSIGN REAL    = REAL
        // STRING  ASSIGN STRING  = STRING
        // BOOLEAN ASSIGN BOOLEAN = BOOLEAN
        case ASSIGN:
            r = _NONE_;
            if (((t1 == _INT_) || (t1 == _REAL_)) && (t2 != _INT_) && (t2 != _REAL_))
                throw Obstacle(EXPR_BAD_TYPE);
            if (((t1 == _STRING_) || (t1 == _BOOLEAN_)) && (t1 != t2))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // BOOLEAN JIT LABEL = _
        case JIT:
            r = _NONE_;
            if ((t1 != _BOOLEAN_) || (t2 != _LABEL_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // _ JMP LABEL = _
        case JMP:
            r = _NONE_;
            if ((t1 != _NONE_) || (t2 != _LABEL_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // INT CALL LABEL = _
        // INT RET  LABEL = _
        case CALL: case RET:
            r = _NONE_;
            if ((t1 != _INT_) || (t2 != _LABEL_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // _ READ  INT     = _
        // _ READ  REAL    = _
        // _ READ  STRING  = _
        // _ READ  BOOLEAN = _
        // _ WRITE INT     = _
        // _ WRITE REAL    = _
        // _ WRITE STRING  = _
        // _ WRITE BOOLEAN = _
        case READ: case WRITE:
            r = _NONE_;
            if ((t1 != _NONE_) || (t2 == _NONE_) || (t2 == _STRUCT_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // _ STOP _ = _
        // _ LOCK _ = _
        // _ ENDL _ = _
        case STOP: case LOCK: case ENDL:
            r = _NONE_;
            if ((t1 != _NONE_) || (t2 != _NONE_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        // _ LOAD  INT = _
        // _ SHARE INT = _
        // _ FORK  INT = _
        case LOAD: case SHARE: case FORK:
            r = _NONE_;
            if ((t1 != _NONE_) || (t2 != _INT_))
                throw Obstacle(EXPR_BAD_TYPE);

            break;

        default:
            throw Obstacle(PANIC);
    }

    return r;
}

char * typetostr(type_t t) {
    switch(t) {
        case _NONE_: return "NONE"; break;
        case _INT_:  return "INT";  break;
        case _REAL_: return "REAL"; break;
        case _STRING_: return "STRING"; break;
        case _BOOLEAN_: return "BOOLEAN"; break;
        case _LABEL_: return "LABEL"; break;
        case _STRUCT_: return "STRUCT"; break;
    }
    #ifdef DEBUG
    return "???";
    #else
    throw Obstacle(PANIC);
    #endif
}

bool isNullary(operation_t o) {
    bool ret = (o == NONE) || (o == ENDL) || (o == RET) || (o == STOP);
    ret = ret || (o == LOCK);
    return ret;
}

bool isUnary(operation_t o) {
    bool ret = (o == INV) || (o == LNOT) || (o == LOAD) || (o == READ);
    ret = ret || (o == WRITE) || (o == JMP) || (o == SHARE) || (o == FORK);
    return ret;
}

bool isBinary(operation_t o) {
    bool ret = (o == JIT) || (o == CALL) || (o == PLUS) || (o == MINUS);
    ret = ret || (o == LOR) || (o == MUL) || (o == DIV) || (o == LAND);
    ret = ret || (o == MOD) || (o == LESS) || (o == GRTR) || (o == LESSEQ);
    ret = ret || (o == GRTREQ) || (o == EQ) || (o == NEQ) || (o == ASSIGN);
    return ret;
}

bool isExpr(operation_t o) {
    bool ret = (o == INV) || (o == PLUS) || (o == MINUS) || (o == LOR);
    ret = ret || (o == MUL) || (o == DIV) || (o == LAND) || (o == LNOT);
    ret = ret || (o == MOD) || (o == LESS) || (o == GRTR) || (o == LESSEQ);
    ret = ret || (o == GRTREQ) || (o == EQ) || (o == NEQ) || (o == ASSIGN);
    return ret;
}

int operands(operation_t o) {
    if (isNullary(o))
        return 0;
    if (isUnary(o))
        return 1;
    if (isBinary(o))
        return 2;
    throw Obstacle(PANIC);
}