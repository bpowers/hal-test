# if you invoke make as 'make V=1' it will verbosely list what it is doing,
# otherwise it defaults to pretty mode, which makes errors _much_ easier to see
ifneq ($V, 1)
MAKEFLAGS = -s
endif

CC=clang

WARNFLAGS := \
        -Wformat -Wall -Wundef -Wpointer-arith -Wcast-qual \
        -Wwrite-strings -Wsign-compare -Wmissing-noreturn \
        -Wextra -Wstrict-aliasing=2 -Wno-unused-parameter
# clang doesn't know about this yet
#        -Wunsafe-loop-optimizations

CFLAGS += -m32 -std=gnu99 $(WARNFLAGS) -I.

SRC := haltest.c
OBJS := $(patsubst %.c,%.o,$(filter %.c,$(SRC)))

LDFLAGS += /lib/libdbus-1.so.3 /usr/lib/libhal.so.1 -Wl,--no-add-needed

TARGETS := haltest

all: $(TARGETS)

# link the program
haltest: $(OBJS)
	@echo "  LD    $@"
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS) $(LDFLAGS)

# clear out all suffixes
.SUFFIXES:
# list only those we use
.SUFFIXES: .d .c .o .proto .pb-c.c

# calculate C include
%.d: %.c
	@echo "  DEPS  $@"
	support/depend.sh `dirname $*.c` $(CFLAGS) $*.c > $@

%.o: %.c
	@echo "  CC    $@"
	$(CC) $(CFLAGS) -c -o $@ $<

# include the C include dependencies
-include $(OBJS:.o=.d)


clean:
	find . -name "*.o" | xargs rm -f
	rm -f $(TARGETS)

distclean: clean
	find . -name "*.d" | xargs rm -f

.PHONY: clean distclean