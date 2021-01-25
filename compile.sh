#!/bin/bash
# author: Flavio Regis de Arruda
FILE=main_submit.cpp

rm $FILE

./generate_submission_file.py

if [ -f "$FILE" ]; then
  echo "$FILE generated. Let's compile it"
  g++-7 -Wall -pipe -O2 -march=native -g --std=c++17 -lm main_submit.cpp
  if [ $? -eq 0 ]; then
    echo "Compilation finished sucessfully."
  else
    echo "Error compiling $FILE."
  fi
else
  echo "$FILE not found! Probably not generated."
fi
