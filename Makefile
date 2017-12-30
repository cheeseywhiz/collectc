PWD:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SRC:=$(PWD)/src
LIB:=$(PWD)/lib
TEST:=$(PWD)/test
BUILD:=$(PWD)/build
OBJ:=$(BUILD)/obj

OBJECTS:=get.o jsmnutils.o rand.o reg.o path.o raw.o random_popper.o
TEST_OBJS:=jsmntest.o pathtest.o randtest.o regtest.o random_popper_test.o

OBJECTS:=$(addprefix $(OBJ)/,$(OBJECTS))
TEST_OBJS:=$(addprefix $(OBJ)/,$(TEST_OBJS))

CFLAGS+=-Wall -Wextra -std=c99 -fPIC -march=native -D_GNU_SOURCE
CFLAGS+=-DJSMN_PARENT_LINKS -I$(LIB)/jsmn

ifeq ($(DEBUG),1)
	CFLAGS+=-O3 -g3
	VFLAGS+=-v --leak-check=full --track-origins=yes --show-leak-kinds=all
	TEST_CMD:=-
else
	CFLAGS+=-O2
endif

TEST_CFLAGS:=-I$(SRC) -Wl,-rpath=$(BUILD),-rpath-link=$(BUILD)
TEST_CFLAGS+=-Og -g3 -Wno-unused-variable

VALGRIND:=$(shell command -v valgrind 2>/dev/null)

ifdef VALGRIND
	TEST_CMD+=valgrind $(VFLAGS)
endif

TEST_CMD+=$(BUILD)/test

all: $(BUILD)/collect

cleandeps:
	rm -rf $(LIB)/jsmn/test/test_*
	cd $(LIB)/jsmn && $(MAKE) clean

clean: cleandeps
	rm -rf $(shell cat .gitignore) /tmp/collectc.* $(LIB)/jsmn/test/test_*

jsmn:
	cd $(LIB)/$@ && CFLAGS="-fPIC -DJSMN_PARENT_LINKS" $(MAKE)

deps: jsmn

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/libcollect.so: LDLIBS+=-ljsmn -lm $(shell pkg-config --libs --cflags libcurl)
$(BUILD)/libcollect.so: LDFLAGS+=-L$(LIB)/jsmn
$(BUILD)/libcollect.so: $(OBJ)/ $(OBJECTS) $(BUILD)/
	$(CC) $(CFLAGS) -shared -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)

$(BUILD)/collect: $(SRC)/main.c $(BUILD)/libcollect.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -L$(BUILD) $(LDLIBS) -lcollect

$(OBJ)/%.o: $(TEST)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/libtest.so: CFLAGS+=$(TEST_CFLAGS)
$(BUILD)/libtest.so: $(OBJ)/ $(TEST_OBJS) $(BUILD)/
	$(CC) $(CFLAGS) -shared -o $@ $(TEST_OBJS) $(LDFLAGS) $(LDLIBS)

$(BUILD)/test: $(TEST)/main.c $(BUILD)/libcollect.so $(BUILD)/libtest.so
	$(CC) $(CFLAGS) $(TEST_CFLAGS) -o $@ $< $(LDFLAGS) -L$(BUILD) $(LDLIBS) -ltest -lcollect

testdeps:
	cd $(LIB)/jsmn && $(MAKE) test

test: $(BUILD)/test
	$(TEST_CMD)

version:
	$(MAKE) --version

travisrun: deps all test
	$(BUILD)/collect
	$(BUILD)/collect
	$(BUILD)/collect random

travis: version clean
	$(MAKE) travisrun 2>&1 | tee $(PWD)/build.log

$(PWD)/%/:
	mkdir -p $@

.PHONY: version cleandeps clean jsmn deps testdeps all test travisrun travis
