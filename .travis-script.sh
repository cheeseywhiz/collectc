#!/bin/bash

run() {
    make clean && make deps && make && make test && LD_LIBRARY_PATH=build ./build/collect
}

main() {
    run | tee build.log
}

main
