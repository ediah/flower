#!/usr/bin/python3

import sys

REPORT = sys.argv[1]

with open(REPORT, 'r') as f:
    data = f.read()

percent = float(data.strip().split()[1][:-1])

with open('README.md', 'r') as f:
    readme = f.read()

start = readme.find('badge/coverage-') + len('badge/coverage-')
end = readme[start:].find('?')

color = 'red'
if percent >= 75:
    color = 'yellow'
if percent >= 90:
    color = 'brightgreen'

msg = str(percent) + '%25-' + color

readme = readme[:start] + msg + readme[start + end:]

with open('README.md', 'w') as f:
    f.write(readme)