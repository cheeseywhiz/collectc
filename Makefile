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
	@cd $(LIB)/$@ && make
	$(eval CFLAGS+=-I$(LIB)/$@)
	$(eval LDFLAGS+=-L$(LIB)/$@)
	$(eval LDLIBS+=-ljsmn)

reg:
	$(eval NEW_OBJ=$(OBJ)/$@.so)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -shared $(OBJECTS) $(SRC)/$@.c -o $(NEW_OBJ)
	$(eval OBJECTS+=$(NEW_OBJ))

get: reg
	$(eval NEW_OBJ=$(OBJ)/$@.so)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(shell pkg-config --libs --cflags libcurl) -shared $(OBJECTS) $(SRC)/$@.c -o $(NEW_OBJ)
	$(eval OBJECTS+=$(NEW_OBJ))

objects: reg get

collect: get
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(OBJECTS) $(SRC)/main.c -o $(BUILD)/collect

test: setup_test jsontest.c

setup_test: clean build_dirs
	$(eval CFLAGS+=-I$(SRC))

jsontest.c: setup_test jsmn
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(OBJECTS) $(TEST)/jsontest.c -o $(BUILD)/jsontest
