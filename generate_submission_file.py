#!/usr/bin/env python3

import re
from pathlib import Path
from typing import Final

PROJECT_PATH: Final = Path('.')
MAIN_FILENAME: Final = PROJECT_PATH / 'main.cpp'
SUBMISSION_FILENAME: Final = PROJECT_PATH / 'main_submit.cpp'

PATTERN: Final = r"^\#include\s+\"(.+\.hpp)\"\s*$"
PROG: Final = re.compile(PATTERN)


def needs_preprocess(program: [str]) -> bool:
    for line in program:
        res = PROG.search(line)
        if res is not None:
            return True
    return False


def preprocess(program: [str]) -> [str]:
    response = []
    for line in program:
        result = PROG.search(line)
        if result:
            filename = PROJECT_PATH / result.group(1)
            with filename.open("r") as f:
                for file_line in f.readlines():
                    response.append(file_line)
        else:
            response.append(line)
    return response


def main():
    with MAIN_FILENAME.open("r") as f:
        program = f.readlines()
    while needs_preprocess(program):
        program = preprocess(program)
    with SUBMISSION_FILENAME.open("w") as f:
        f.writelines(program)


if __name__ == "__main__":
    main()
