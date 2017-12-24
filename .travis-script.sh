#!/bin/bash
make clean && make deps && make && make test && LD_LIBRARY_PATH=build ./build/collect
