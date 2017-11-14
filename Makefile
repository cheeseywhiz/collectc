ROOT=$(PWD)
BUILD=$(PWD)/build
SRC=$(PWD)/src
GCC=gcc -Wall
LIBCURL=$(shell pkg-config --libs --cflags libcurl)

all: clean collect.c reg.c

collect.c:
	$(GCC) $(LIBCURL) $(SRC)/main.c $(SRC)/get.c -o $(BUILD)/collect

reg.c:
	$(GCC) $(SRC)/reg.c -o $(BUILD)/reg

clean:
	rm -rf build && mkdir -p build
