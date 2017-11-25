SRC=$(PWD)/src
LIB=$(PWD)/lib
TEST=$(PWD)/test
BUILD=$(PWD)/build
OBJ=$(BUILD)/obj

CFLAGS+=-Wall -Wextra -pedantic-errors -O2 -fPIC -fverbose-asm -masm=intel -march=native -std=c99 -pedantic

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
	rm -rf build
	@cd $(LIB)/jsmn && $(MAKE) clean

jsmn:
	@cd $(LIB)/$@ && CFLAGS="-fPIC -DJSMN_PARENT_LINKS" $(MAKE)

$(OBJ)/%.so: $(SRC)/%.c
	$(CC) $(CFLAGS) -shared -o $@ $< $(LDFLAGS) $(LDLIBS)

$(OBJ)/get.so: LDLIBS+=$(shell pkg-config --libs --cflags libcurl)

$(OBJ)/rand.so: LDLIBS+=-lm

objects: $(OBJ)/get.so $(OBJ)/jsmnutils.so $(OBJ)/rand.so $(OBJ)/reg.so
	$(eval OBJECTS=$^)

$(PWD)/%:
	mkdir -p $@

lib: jsmn $(OBJ) objects

$(BUILD)/collect: $(SRC)/main.c lib $(BUILD)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $< $(LDFLAGS) $(LDLIBS)

$(BUILD)/test: $(TEST)/test.c lib $(BUILD)
	$(eval CFLAGS+=-Og)
	$(eval CFLAGS+=-g3)
	$(eval CFLAGS+=-I$(SRC))
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $< $(LDFLAGS) $(LDLIBS)

test: $(BUILD)/test
	@cd $(LIB)/jsmn && $(MAKE) test
	$(TEST_CMD)

.PHONY: all clean jsmn objects lib test
