SRC=$(PWD)/src
LIB=$(PWD)/lib
TEST=$(PWD)/test
BUILD=$(PWD)/build
OBJ=$(BUILD)/obj

CFLAGS+=-Wall -Wextra -O2 -fPIC -fverbose-asm -masm=intel -march=native

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

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

objects: $(OBJ)/get.o $(OBJ)/jsmnutils.o $(OBJ)/rand.o $(OBJ)/reg.o
	$(eval OBJECTS=$^)
	$(eval LDLIBS+=$(shell pkg-config --libs --cflags libcurl))
	$(eval LDLIBS+=-lm)

$(PWD)/%:
	mkdir -p $@

$(BUILD)/libcollect.a: jsmn $(OBJ) objects $(BUILD)
	$(AR) rcsv $@ $(OBJECTS)
	$(eval LDFLAGS+=-L$(BUILD))
	$(eval LDLIBS+=-lcollect)

$(BUILD)/collect: $(SRC)/main.c $(BUILD)/libcollect.a
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $< $(LDFLAGS) $(LDLIBS)

$(BUILD)/test: $(TEST)/test.c $(BUILD)/libcollect.a
	$(eval CFLAGS+=-Og)
	$(eval CFLAGS+=-g3)
	$(eval CFLAGS+=-I$(SRC))
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $< $(LDFLAGS) $(LDLIBS)

test: $(BUILD)/test
	@cd $(LIB)/jsmn && $(MAKE) test
	$(TEST_CMD)

.PHONY: all clean jsmn objects test
