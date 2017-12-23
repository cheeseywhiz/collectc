#!/bin/bash
make clean && make deps && make test && make && LD_LIBRARY_PATH=build ./build/collect
