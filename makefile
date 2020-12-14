CFLAGS += -O2
CPPFLAGS += -I. -Iinclude/ -Iacvector/src

BUILDDIR = build

DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

LIBSRCS = $(wildcard src/lib/*.c)
BINSRCS = $(wildcard src/bin/*.c)
SRCS = $(LIBSRCS) $(BINSRCS)

DEPFILES := $(SRCS:%.c=%.d)

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

all: lib bin

%.o : %.c %.d makefile
	$(COMPILE.c) -o $@ $<

$(DEPFILES):
include $(wildcard $(DEPFILES))

lib: build/lib/libstorylines.a

bin: $(BUILDDIR)/bin/storylines-minisat $(BUILDDIR)/bin/storylines-cadical

build/lib/libstorylines.a: $(patsubst src/lib/%.c,src/lib/%.o,$(LIBSRCS)) acvector/src/acvector.o
	mkdir -p $(BUILDDIR)/lib
	$(AR) rcs $@ $^

$(BUILDDIR)/bin/storylines-minisat: minisat/build/release/lib/libminisat.a $(BUILDDIR)/lib/libstorylines.a $(patsubst src/bin/%.c,src/bin/%.o,$(BINSRCS))
	mkdir -p $(BUILDDIR)/bin
	$(CXX) -L$(BUILDDIR)/lib -Lminisat/build/release/lib -o $@ $(patsubst src/bin/%.c,src/bin/%.o,$(BINSRCS)) -lstorylines -lminisat

$(BUILDDIR)/bin/storylines-cadical: cadical/build/libcadical.a $(BUILDDIR)/lib/libstorylines.a $(patsubst src/bin/%.c,src/bin/%.o,$(BINSRCS))
	mkdir -p $(BUILDDIR)/bin
	$(CXX) -L$(BUILDDIR)/lib -Lcadical/build -o $@ $(patsubst src/bin/%.c,src/bin/%.o,$(BINSRCS)) -lstorylines -lcadical

acvector/src/acvector.o: acvector/src/acvector.c
	$(MAKE) -C acvector src/acvector.o

minisat/build/release/lib/libminisat.a:
	$(MAKE) -C minisat

cadical/build/libcadical.a:
	cd cadical && ./configure
	$(MAKE) -C cadical

clean:
	$(RM) -r $(wildcard src/bin/*.d) $(wildcard src/lib/*.d) $(wildcard src/bin/*.o) $(wildcard src/lib/*.o) $(BUILDDIR) $(DEPDIR)

.PHONY: all clean lib bin
