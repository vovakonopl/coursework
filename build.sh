#!/bin/bash

## flags for compiler
FLAGS="-Wall -Iinclude"

## name of the .exe file
OUT_NAME="app"

## search for all .cpp files
FILES=$(find src -name "*.cpp")

## compilation command
g++ $FLAGS $FILES -o $OUT_NAME

echo "Compilation finished! Use \"./$OUT_NAME\" to run the program."
