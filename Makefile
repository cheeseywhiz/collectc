SRC=$(PWD)/src
LIB=$(PWD)/lib
TEST=$(PWD)/test
BUILD=$(PWD)/build
OBJ=$(BUILD)/obj

CFLAGS+=-Wall -Wextra -pedantic-errors -O2 -fPIC -fverbose-asm -masm=intel -march=native

all: clean build_dirs collect

clean:
	rm -rf build
	@cd $(LIB)/jsmn && $(MAKE) clean

build_dirs:
	@mkdir -p $(BUILD)
	@mkdir -p $(OBJ)

jsmn:
	$(eval CFLAGS+=-DJSMN_PARENT_LINKS)
	@cd $(LIB)/$@ && CFLAGS="-DJSMN_PARENT_LINKS" $(MAKE)
	$(eval CFLAGS+=-I$(LIB)/$@)
	$(eval LDFLAGS+=-L$(LIB)/$@)
	$(eval LDLIBS+=-ljsmn)

src/%:
	$(eval NEW_OBJ=$(OBJ)/$(@F).so)
	$(CC) $(CFLAGS) -shared $(OBJECTS) $@.c -o $(NEW_OBJ) $(LDFLAGS) $(LDLIBS)
	$(eval OBJECTS+=$(NEW_OBJ))

src/get: src/reg
src/get: LDLIBS+=$(shell pkg-config --libs --cflags libcurl)

src/rand: LDLIBS+=-lm

src/jsmnutils: jsmn src/reg src/rand

lib: src/get src/jsmnutils src/rand src/reg

collect: lib
	$(CC) $(CFLAGS) $(OBJECTS) $(SRC)/main.c -o $(BUILD)/$@ $(LDFLAGS) $(LDLIBS)

setup_test: clean build_dirs
	$(eval CFLAGS+=-g3)
	$(eval CFLAGS+=-I$(SRC))

test: setup_test lib
	$(CC) $(CFLAGS) $(OBJECTS) $(TEST)/$@.c -o $(BUILD)/$@ $(LDFLAGS) $(LDLIBS)
	valgrind $(VALGRIND) $(BUILD)/$@
