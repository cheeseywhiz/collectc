SRC:=src
LIB:=lib
TEST:=test
BUILD:=build
OBJ:=$(BUILD)/obj
DIRS:=$(BUILD) $(OBJ)
TEST_PROGRAM:=$(BUILD)/test
auto_link+=-Wl,-rpath=$(BUILD),-rpath-link=$(BUILD)

OBJECTS:=get jsmnutils rand reg path raw random_popper
TEST_OBJS:=jsmntest pathtest randtest regtest random_popper_test rawtest
SRC_HDR:=config collect log $(OBJECTS)
TEST_HDR:=test

CFLAGS+=-Wall -Wextra -std=c99 -fPIC -D_GNU_SOURCE
CFLAGS+=-DJSMN_PARENT_LINKS -I$(LIB)/jsmn
BUILD_COLLECT_CFLAGS:=

TEST_CFLAGS:=-I$(SRC)
TEST_CFLAGS+=-Og -g3 -Wno-unused-variable
BUILD_TEST_CFLAGS:=$(auto_link)

version_programs:=$(CC) $(LD) $(MAKE) curl-config

OBJECTS:=$(addprefix $(OBJ)/,$(addsuffix .o,$(OBJECTS)))
TEST_OBJS:=$(addprefix $(OBJ)/,$(addsuffix .o,$(TEST_OBJS)))
SRC_HDR:=$(addprefix $(SRC)/,$(addsuffix .h,$(SRC_HDR)))
TEST_HDR:=$(addprefix $(TEST)/,$(addsuffix .h,$(TEST_HDR)))

ifeq ($(DEBUG),1)
	CFLAGS+=-Og -g3
	CFLAGS+=-D_COLLECT_DEBUG
	BUILD_COLLECT_CFLAGS:=$(auto_link)
	VFLAGS+=-v --leak-check=full --track-origins=yes --show-leak-kinds=all
	TEST_CMD:=-
else
	CFLAGS+=-O2
	TEST_CMD:=
endif

VALGRIND:=$(shell command -v valgrind 2>/dev/null)

ifdef VALGRIND
	TEST_CMD+=valgrind $(VFLAGS)
endif

TEST_CMD+=$(TEST_PROGRAM)

.PHONY: all
all: builddirs $(BUILD)/collect

$(DIRS):
	mkdir -p $@

.PHONY: builddirs
builddirs: $(DIRS)

.PHONY: reset
reset: clean deps builddirs

.PHONY: cleandeps
cleandeps:
	rm -rf $(LIB)/jsmn/test/test_*
	cd $(LIB)/jsmn && $(MAKE) clean

.PHONY: cleancollect
cleancollect:
	rm -rf $(shell cat .gitignore) /tmp/collectc.* $(LIB)/jsmn/test/test_*

.PHONY: clean
clean: cleandeps cleancollect

.PHONY: jsmn
jsmn:
	cd $(LIB)/$@ && CFLAGS="-fPIC -DJSMN_PARENT_LINKS" $(MAKE)

.PHONY: deps
deps: jsmn

# OBJECTS
$(OBJ)/%.o: $(SRC)/%.c $(SRC_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/libcollect.so: LDLIBS+=-ljsmn -lm $(shell pkg-config --libs --cflags libcurl)
$(BUILD)/libcollect.so: LDFLAGS+=-L$(LIB)/jsmn
$(BUILD)/libcollect.so: $(OBJECTS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/collect: $(SRC)/main.c $(BUILD)/libcollect.so
	$(CC) $(CFLAGS) $(BUILD_COLLECT_CFLAGS) -o $@ $< $(LDFLAGS) -L$(BUILD) $(LDLIBS) -lcollect

# TEST_OBJS
$(OBJ)/%.o: $(TEST)/%.c $(SRC_HDR) $(TEST_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/libtest.so: CFLAGS+=$(TEST_CFLAGS)
$(BUILD)/libtest.so: $(TEST_OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/test: $(TEST)/main.c $(BUILD)/libcollect.so $(BUILD)/libtest.so
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $(BUILD_TEST_CFLAGS) -o $@ $< $(LDFLAGS) -L$(BUILD) $(LDLIBS) -ltest -lcollect

.PHONY: testdeps
testdeps:
	cd $(LIB)/jsmn && $(MAKE) test

.PHONY: testcollect
testcollect:
	$(TEST_CMD)

.PHONY: test
test: builddirs $(BUILD)/test testcollect

.PHONY: $(version_programs)
$(version_programs):
	-$@ --version
	@echo

.PHONY: version
version: $(version_programs)

.PHONY: travisrun
travisrun: version deps all test
	$(BUILD)/collect
	$(BUILD)/collect
	$(BUILD)/collect random

.PHONY: travis
travis: clean
	./.teeexit.sh travis.log $(MAKE) travisrun
