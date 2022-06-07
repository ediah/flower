#! /usr/bin/python3
# Генерация зависимостей make

import sys, os

SRC_DIRS = sys.argv[1:]

SRC = []
for srcd in SRC_DIRS:
    for addr, dirs, files in os.walk(srcd):
        files = list(map(lambda x: addr + '/' + x, files))
        cpp = list(filter(lambda x: x[-4:] == '.cpp', files))
        hpp = list(filter(lambda x: x[-4:] == '.hpp', files))
        SRC += cpp + hpp
SRC = list(set(SRC))

DEP = {}
for code in SRC:
    with open(code, "r") as file:
        data = file.read().strip().split('\n')
    inc = []
    for line in data:
        if line[:8] == '#include':
            inc += [line.split()[1]]
    inc = list(filter(lambda x: x[0] != '<' and x[-1] != '>', inc))
    inc = list(set(map(lambda x: './src/' + x[1:-1], inc)))

    if code[:-4] in set(DEP.keys()):
        DEP[code[:-4]] += inc
    else:
        DEP[code[:-4]] = inc

def resolveRecursively(headers, oldDeps):
    ret = []
    for header in headers:
        deps = DEP[header[:-4]]
        newDeps = oldDeps + deps
        if set(oldDeps) == set(newDeps):
            continue
        ret += deps + resolveRecursively(deps, list(newDeps))
    return list(set(ret))

with open("Makefile.dep", "w") as makefile:
    for key in sorted(DEP.keys()):
        if len(DEP[key]) == 0:
            continue

        headers = DEP[key] + resolveRecursively(DEP[key], [])
        key = key[ key.rfind("/") + 1 :]
        line = key + '.o:'
        for header in set(headers):
            line += ' ' + header
        makefile.write(line + '\n')