#!/bin/bash

# Remove files from previous executions.
rm -f cppcheck.txt clang-tidy.txt

# Runs cppcheck
echo ""
echo "Running cppcheck..."
cppcheck --template "{file}({line}): {severity} ({id}): {message}" --enable=all --suppress=missingIncludeSystem --force --std=c++11 --inline-suppr config.hpp zuniq.hpp main.cpp 2>cppcheck.txt
if [ -s cppcheck.txt ]; then
  cat cppcheck.txt
  exit 1
fi

# Runs clang-tidy
echo ""
echo "Running clang-tidy..."
if ! clang-tidy config.hpp zuniq.hpp main.cpp tests.cpp tests_runner.cpp -- -std=c++17 2>clang-tidy.txt; then
  echo "clang-tidy check failed!"
  exit 1
fi

if grep -q "warning: " clang-tidy.txt || grep -q "error: " clang-tidy.txt; then
  echo "You must pass the static analysis checks before submitting a pull request"
  echo ""
  grep --color -E '^|warning: |error: ' clang-tidy.txt
  exit 1
else
  echo -e "\033[1;32m\xE2\x9C\x93 static analysis passed.\033[0m $1"
  exit 0
fi
