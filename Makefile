PWD:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SRC:=$(PWD)/src
LIB:=$(PWD)/lib
TEST:=$(PWD)/test
BUILD:=$(PWD)/build
OBJ:=$(BUILD)/obj

CFLAGS+=-Wall -Wextra -std=c99 -fPIC -march=native

ifdef DEBUG
	CFLAGS+=-O3 -g3
	VFLAGS+=-v --leak-check=full --track-origins=yes --show-leak-kinds=all
else
	CFLAGS+=-O2
endif

# global jsmn flags
CFLAGS+=-DJSMN_PARENT_LINKS -I$(LIB)/jsmn
LDFLAGS+=-L$(LIB)/jsmn
LDLIBS+=-ljsmn

VALGRIND:=$(shell command -v valgrind 2>/dev/null)

ifdef VALGRIND
	TEST_CMD=valgrind $(VFLAGS) $(BUILD)/test
else
	TEST_CMD=$(BUILD)/test
endif

all: $(BUILD)/collect

clean:
	rm -rf $(shell cat .gitignore)
	cd $(LIB)/jsmn && $(MAKE) clean

jsmn:
	cd $(LIB)/$@ && CFLAGS="-fPIC -DJSMN_PARENT_LINKS" $(MAKE)

deps: jsmn

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

OBJECTS:=$(OBJ)/get.o $(OBJ)/jsmnutils.o $(OBJ)/rand.o $(OBJ)/reg.o $(OBJ)/path.o $(OBJ)/raw.o

$(BUILD)/libcollect.so: $(OBJ) $(OBJECTS) $(BUILD)
	$(eval LDLIBS+=$(shell pkg-config --libs --cflags libcurl))
	$(eval LDLIBS+=-lm)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)
	$(eval LDFLAGS=-L$(BUILD))
	$(eval LDLIBS=-lcollect)

$(BUILD)/collect: $(SRC)/main.c $(BUILD)/libcollect.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

$(OBJ)/%.o: $(TEST)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

TEST_OBJS:=$(OBJ)/jsmntest.o $(OBJ)/pathtest.o $(OBJ)/randtest.o $(OBJ)/regtest.o

testflags:
	$(eval CFLAGS+=-Og -g3 -I$(SRC) -Wl,-rpath=$(BUILD),-rpath-link=$(BUILD))

$(BUILD)/libtest.so: $(OBJ) testflags $(TEST_OBJS) $(BUILD)
	$(CC) $(CFLAGS) -shared -o $@ $(TEST_OBJS) $(LDFLAGS) $(LDLIBS)
	$(eval LDFLAGS+=-L$(TEST))
	$(eval LDLIBS+=-ltest)

$(BUILD)/test: $(TEST)/test.c $(BUILD)/libcollect.so $(BUILD)/libtest.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

testdeps:
	cd $(LIB)/jsmn && $(MAKE) test

test: $(BUILD)/test
	LD_LIBRARY_PATH=$(BUILD) $(TEST_CMD)

$(PWD)/%:
	mkdir -p $@

.PHONY: clean jsmn deps testdeps all objects testflags test
