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
	@cd $(LIB)/$@ && CFLAGS=-DJSMN_PARENT_LINKS make
	$(eval CFLAGS+=-I$(LIB)/$@)
	$(eval LDFLAGS+=-L$(LIB)/$@)
	$(eval LDLIBS+=-ljsmn)

src/%:
	$(eval NEW_OBJ=$(OBJ)/$(@F).so)
	$(CC) $(CFLAGS) -shared $(OBJECTS) $(SRC)/$(@F).c -o $(NEW_OBJ) $(LDFLAGS) $(LDLIBS)
	$(eval OBJECTS+=$(NEW_OBJ))

src/reg: src/stringutil

src/get: src/reg
src/get: LDLIBS+=$(shell pkg-config --libs --cflags libcurl)

src/jsmnutils: src/reg

collect: src/get
	$(CC) $(CFLAGS) $(OBJECTS) $(SRC)/main.c -o $(BUILD)/$@ $(LDFLAGS) $(LDLIBS)

test: setup_test jsontest

setup_test: clean build_dirs
	$(eval CFLAGS+=-I$(SRC))

jsontest: setup_test jsmn src/get src/jsmnutils
	$(CC) $(CFLAGS) $(OBJECTS) $(TEST)/$@.c -o $(BUILD)/$@ $(LDFLAGS) $(LDLIBS)
