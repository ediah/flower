program {
    string ans;

    write("Вы: Доктор, у меня недосып!");
    write("Доктор: Так давайте я Вам досыплю!");
    write("*сып-сып*");
    
    while(true) {
        write("Доктор: Ещё? (Да/Нет)");
        read(ans);

        if (ans == "Нет")
            break;
        else if (ans == "Да")
            write("*сып-сып*");
        else
            write("Доктор: Я Вас не понял!");
    }

    write("Доктор: Приходите ещё!");
}
