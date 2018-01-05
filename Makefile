SRC:=src
LIB:=lib
TEST:=test
BUILD:=build
OBJ:=$(BUILD)/obj
DIRS:=$(BUILD) $(OBJ)
TEST_PROGRAM:=$(BUILD)/test

VERSION:=0.3.2

autolink+=-Wl,-rpath=$(BUILD),-rpath-link=$(BUILD)

ifeq ($(DO_AUTOLINK),1)
	AUTOLINK:=$(autolink)
else
	AUTOLINK:=
endif

OBJECTS:=get jsmnutils rand reg path raw random_popper log
TEST_OBJS:=jsmntest pathtest randtest regtest random_popper_test rawtest
SRC_HDR:=collect $(OBJECTS)
TEST_HDR:=test

COLLECT_FLAGS:=-nrav -ocache

CFLAGS+=-Wall -Wextra -std=c99 -fPIC -D_GNU_SOURCE -DCOLLECT_VERSION=\"$(VERSION)\"
CFLAGS+=-DJSMN_PARENT_LINKS -I$(LIB)/jsmn

TEST_CFLAGS:=-I$(SRC)
TEST_CFLAGS+=-Og -g3 -Wno-unused-variable

version_programs:=$(CC) $(LD) $(MAKE) curl-config

OBJECTS:=$(addprefix $(OBJ)/,$(addsuffix .o,$(OBJECTS)))
TEST_OBJS:=$(addprefix $(OBJ)/,$(addsuffix .o,$(TEST_OBJS)))
SRC_HDR:=$(addprefix $(SRC)/,$(addsuffix .h,$(SRC_HDR)))
TEST_HDR:=$(addprefix $(TEST)/,$(addsuffix .h,$(TEST_HDR)))

ifeq ($(DEBUG),1)
	CFLAGS+=-Og -g3
	CFLAGS+=-D_COLLECT_DEBUG
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
	$(CC) $(CFLAGS) $(AUTOLINK) -o $@ $< $(LDFLAGS) -L$(BUILD) $(LDLIBS) -lcollect

# TEST_OBJS
$(OBJ)/%.o: $(TEST)/%.c $(SRC_HDR) $(TEST_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/libtest.so: CFLAGS+=$(TEST_CFLAGS)
$(BUILD)/libtest.so: $(TEST_OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/test: $(TEST)/main.c $(BUILD)/libcollect.so $(BUILD)/libtest.so
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $(autolink) -o $@ $< $(LDFLAGS) -L$(BUILD) $(LDLIBS) -ltest -lcollect

.PHONY: testdeps
testdeps:
	cd $(LIB)/jsmn && $(MAKE) test

.PHONY: testcollect
testcollect:
	$(TEST_CMD)

.PHONY: runcollect
runcollect:
	$(BUILD)/collect -V
	$(BUILD)/collect -h
	$(BUILD)/collect reddit $(COLLECT_FLAGS)
	$(BUILD)/collect reddit $(COLLECT_FLAGS)
	$(BUILD)/collect random $(COLLECT_FLAGS)

.PHONY: log
log:
	./.teeexit.sh make.log $(LOG)

.PHONY: test
test: builddirs $(BUILD)/test testcollect

.PHONY: $(version_programs)
$(version_programs):
	-$@ --version
	@echo

.PHONY: version
version: $(version_programs)

.PHONY: travisrun
travisrun: version deps all test runcollect

.PHONY: travis
travis: LOG:=$(MAKE) travisrun
travis: clean log
