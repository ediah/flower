def wrongType(): int {
    return "ERROR";
}

program {
    int x;
    x = wrongType();
    write(x);
}
