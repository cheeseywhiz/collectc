SRC=$(PWD)/src
LIB=$(PWD)/lib
TEST=$(PWD)/test
BUILD=$(PWD)/build
OBJ=$(BUILD)/obj

CC=gcc
CFLAGS+=-Wall -fPIC

all: clean build_dirs collect

clean:
	@rm -rf build
	@cd $(LIB)/jsmn && make clean

build_dirs:
	@mkdir -p $(BUILD)
	@mkdir -p $(OBJ)

jsmn:
	$(eval CFLAGS+=-DJSMN_PARENT_LINKS)
	@cd $(LIB)/$@ && CFLAGS="${CFLAGS}" make
	$(eval CFLAGS+=-I$(LIB)/$@)
	$(eval LDFLAGS+=-L$(LIB)/$@)
	$(eval LDLIBS+=-ljsmn)

src/%:
	$(eval NEW_OBJ=$(OBJ)/$(@F).so)
	$(CC) $(CFLAGS) -shared $(OBJECTS) $@.c -o $(NEW_OBJ) $(LDFLAGS) $(LDLIBS)
	$(eval OBJECTS+=$(NEW_OBJ))

src/get: jsmn src/reg
src/get: LDLIBS+=$(shell pkg-config --libs --cflags libcurl)

src/jsmnutils: jsmn src/reg

collect: src/get src/jsmnutils src/reg
	$(CC) $(CFLAGS) $(OBJECTS) $(SRC)/main.c -o $(BUILD)/$@ $(LDFLAGS) $(LDLIBS)

setup_test: clean build_dirs
	$(eval CFLAGS+=-I$(SRC))

test: setup_test src/get src/reg src/jsmnutils
	$(CC) $(CFLAGS) $(OBJECTS) $(TEST)/$@.c -o $(BUILD)/$@ $(LDFLAGS) $(LDLIBS)
	valgrind $(BUILD)/$@
