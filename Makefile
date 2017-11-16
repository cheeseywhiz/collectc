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
	$(CC) $(CFLAGS) -shared $(OBJECTS) $(SRC)/$@.c -o $(NEW_OBJ) $(LDFLAGS) $(LDLIBS)
	$(eval OBJECTS+=$(NEW_OBJ))

get: reg
	$(eval NEW_OBJ=$(OBJ)/$@.so)
	$(CC) $(CFLAGS) -shared $(OBJECTS) $(SRC)/$@.c -o $(NEW_OBJ) $(shell pkg-config --libs --cflags libcurl) $(LDFLAGS) $(LDLIBS)
	$(eval OBJECTS+=$(NEW_OBJ))

objects: reg get

collect: get
	$(CC) $(CFLAGS) $(OBJECTS) $(SRC)/main.c -o $(BUILD)/collect $(LDFLAGS) $(LDLIBS)

test: setup_test jsontest.c

setup_test: clean build_dirs
	$(eval CFLAGS+=-I$(SRC))

jsontest.c: setup_test jsmn get
	$(CC) $(CFLAGS) $(OBJECTS) $(TEST)/jsontest.c -o $(BUILD)/jsontest $(LDFLAGS) $(LDLIBS)
