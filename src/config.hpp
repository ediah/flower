#ifndef CONFIG_H
#define CONFIG_H

//! Максимальное число элементов в ПОЛИЗе
#ifndef MAXCMD
    #define MAXCMD 1024
#endif
//! Максимальное число элементов в стеке
#ifndef MAXSTACK
    #define MAXSTACK 1024
#endif
//! Максимальная длина идентификатора
#ifndef MAXIDENT
    #define MAXIDENT 50
#endif
//! Максимальное количество потоков
#ifndef MAXTHREADS
    #define MAXTHREADS 4
#endif
//! Максимальный размер окна отладчика
#ifndef MAXWINDOW
    #define MAXWINDOW 10
#endif

#endif