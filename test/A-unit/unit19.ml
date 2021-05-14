def greet(string name): string {
    return "Привет, " + name + "!";
}

program {
    string me, greeting;
    write("Как тебя зовут?");
    read(me);
    greeting = greet(me);
    write(greeting);
}