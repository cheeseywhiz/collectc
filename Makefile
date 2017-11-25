SRC=$(PWD)/src
LIB=$(PWD)/lib
TEST=$(PWD)/test
BUILD=$(PWD)/build
OBJ=$(PWD)/obj

CFLAGS+=-Wall -Wextra -std=c99 -O2 -fPIC -fverbose-asm -masm=intel -march=native

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
	rm -rf $(BUILD) $(OBJ)
	cd $(LIB)/jsmn && $(MAKE) clean

jsmn:
	cd $(LIB)/$@ && CFLAGS="-fPIC -DJSMN_PARENT_LINKS" $(MAKE)

deps: jsmn

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(PWD)/%:
	mkdir -p $@

OBJECTS:=$(OBJ)/get.o $(OBJ)/jsmnutils.o $(OBJ)/rand.o $(OBJ)/reg.o

$(BUILD)/libcollect.so: $(OBJ) $(OBJECTS) $(BUILD)
	$(eval LDLIBS+=$(shell pkg-config --libs --cflags libcurl))
	$(eval LDLIBS+=-lm)
	$(CC) $(CFLAGS) -shared -Wl,-soname=libcollect.so -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)
	$(eval LDFLAGS=-L$(BUILD))
	$(eval LDLIBS=-lcollect)

$(BUILD)/collect: $(SRC)/main.c $(BUILD)/libcollect.so
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

$(BUILD)/test: $(TEST)/test.c $(BUILD)/libcollect.so
	$(CC) $(CFLAGS) -Og -g3 -I$(SRC) -Wl,-rpath=$(BUILD),-rpath-link=$(BUILD) -o $@ $< $(LDFLAGS) $(LDLIBS)

test: $(BUILD)/test
	cd $(LIB)/jsmn && $(MAKE) test
	$(TEST_CMD)

.PHONY: all clean jsmn deps objects test
