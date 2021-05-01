struct fio_t {
    string firstname, lastname;
}

struct person {
    int age;
    struct fio_t fio;
}

program {
    struct person A = {
        age = 20, 
        fio = {firstname = "Вася",
               lastname  = "Пупкин"}
    };

    struct person B = {
        age = 30,
        fio = {firstname = "Иван",
               lastname  = "Иванов"}
    };

    A = B;

    write(A.fio.firstname, " ", A.fio.lastname, 
                          ", ", A.age, " лет.");
}