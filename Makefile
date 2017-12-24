PWD:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SRC:=$(PWD)/src
LIB:=$(PWD)/lib
TEST:=$(PWD)/test
BUILD:=$(PWD)/build
OBJ:=$(BUILD)/obj

CFLAGS+=-Wall -Wextra -std=c99 -fPIC -march=native -O2

# global jsmn flags
CFLAGS+=-DJSMN_PARENT_LINKS
CFLAGS+=-I$(LIB)/jsmn
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
	rm -rf $(BUILD) $(OBJ) $(TEST)/*.o $(TEST)/*.so vgcore.* *.jpg *.png
	cd $(LIB)/jsmn && $(MAKE) clean

jsmn:
	cd $(LIB)/$@ && CFLAGS="-fPIC -DJSMN_PARENT_LINKS" $(MAKE)

deps: jsmn

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TEST)/%.o: $(TEST)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PWD)/%:
	mkdir -p $@

OBJECTS:=$(OBJ)/get.o $(OBJ)/jsmnutils.o $(OBJ)/rand.o $(OBJ)/reg.o $(OBJ)/path.o

$(BUILD)/libcollect.so: $(OBJ) $(OBJECTS) $(BUILD)
	$(eval LDLIBS+=$(shell pkg-config --libs --cflags libcurl))
	$(eval LDLIBS+=-lm)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)
	$(eval LDFLAGS=-L$(BUILD))
	$(eval LDLIBS=-lcollect)

$(BUILD)/collect: $(SRC)/main.c $(BUILD)/libcollect.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

TEST_OBJS:=$(TEST)/jsmntest.o $(TEST)/pathtest.o $(TEST)/randtest.o $(TEST)/regtest.o

testflags:
	$(eval CFLAGS+=-Og -g3 -I$(SRC) -Wl,-rpath=$(BUILD),-rpath-link=$(BUILD))

$(TEST)/libtest.so: $(TEST) testflags $(TEST_OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $(TEST_OBJS) $(LDFLAGS) $(LDLIBS)
	$(eval LDFLAGS+=-L$(TEST))
	$(eval LDLIBS+=-ltest)

$(BUILD)/test: $(TEST)/test.c $(BUILD)/libcollect.so $(TEST)/libtest.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

testdeps:
	cd $(LIB)/jsmn && $(MAKE) test

test: $(BUILD)/test
	LD_LIBRARY_PATH=test $(TEST_CMD)

.PHONY: all clean jsmn deps objects testflags testdeps test
