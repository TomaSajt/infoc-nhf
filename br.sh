#!/bin/sh
cmake -S . -B build/ && cmake --build build/ && ./build/infoc-nhf
