# Toolchain settings
TARGET  := 
CC      := $(TARGET)gcc
CXX     := $(TARGET)g++
AS      := $(TARGET)yasm
LD      := $(TARGET)ld
GDB     := $(TARGET)gdb

BUILD   ?= debug

# User Settings
BINNAME     = libmini.so
AINCDIR     = 
INCDIR      = 
LIBS        =
FLAGS       =
ASFLAGS     = -f elf64 -DPIC -DYASM -D__x86_64__
CFLAGS      = -std=c11 -Wall -fno-stack-protector -fno-tree-loop-distribute-patterns -fPIC -nostdlib
CXXFLAGS    = -std=c++17
LDFLAGS     = -shared
DEFFLAGS    = 
DBGFLAGS    = -g -DDEBUG
LDSCRIPT    = 

# Compile Flags
ZFLAGS      = 
ZASFLAGS    = $(foreach d, $(AINCDIR), -I$d) $(ASFLAGS)
ZCFLAGS     = $(OPTIFLAGS) $(DEFFLAGS) $(foreach d, $(INCDIR), -I$d) $(CFLAGS)
ZCXXFLAGS   = $(OPTIFLAGS) $(DEFFLAGS) $(foreach d, $(INCDIR), -I$d) $(CXXFLAGS)
ZLDFLAGS    = $(LDFLAGS)

OPTIFLAGS   = 
ifeq ($(BUILD),debug)
	OBJDIR = obj/Debug
	DEPDIR = dep/Debug
	ZFLAGS += $(DBGFLAGS)
	OPTIFLAGS = -O0
else ifeq ($(BUILD),release)
	OBJDIR = obj/Release
	DEPDIR = dep/Release
	OPTIFLAGS = -O2
	ZFLAGS += -DNDEBUG
else
	OBJDIR = $(error "Unknown build type")
endif

ifneq ($(LDSCRIPT),)
	ZLDFLAGS += -T $(LDSCRIPT)
endif

SRCS.s      = $(wildcard *.asm)
SRCS.c      = $(wildcard *.c)
SRCS.cpp    = $(wildcard *.cpp)
OBJFILES    = $(SRCS.c:.c=.o) $(SRCS.cpp:.cpp=.o) $(SRCS.s:.asm=.o)
OBJS        = $(addprefix $(OBJDIR)/,$(OBJFILES))
DEPFILES    = $(SRCS.c:.c=.d) $(SRCS.cpp:.cpp=.d)
DEPS        = $(addprefix $(DEPDIR)/,$(DEPFILES))

.PHONY: clean distclean gdb

all: $(BINNAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(DEPDIR):
	mkdir -p $(DEPDIR)

$(OBJDIR)/%.o: %.asm | $(OBJDIR)
	$(AS) $(FLAGS) $(ZASFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(FLAGS) $(ZFLAGS) $(ZCFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(FLAGS) $(ZFLAGS) $(ZCXXFLAGS) -c -o $@ $<

$(DEPDIR)/%.d: %.c | $(DEPDIR)
	@echo -n "$(OBJDIR)/" > $@
	@$(CC) -MM -MP $(FLAGS) $(ZCFLAGS) $< >> $@

$(DEPDIR)/%.d: %.cpp | $(DEPDIR)
	@echo -n "$(OBJDIR)/" > $@
	@$(CXX) -MM -MP $(FLAGS) $(ZCXXFLAGS) $< >> $@

$(BINNAME): $(OBJS)
	$(LD) $(FLAGS) $(ZLDFLAGS) -o $@ $(OBJS) $(foreach l, $(LIBS), -l$l)

tests: $(BINNAME)
	$(MAKE) -C tests all

alarm1: $(BINNAME)
	$(MAKE) -C tests alarm1

alarm2: $(BINNAME)
	$(MAKE) -C tests alarm2

alarm3: $(BINNAME)
	$(MAKE) -C tests alarm3

jmp1: $(BINNAME)
	$(MAKE) -C tests jmp1

ifneq ($(DEPS),)
  sinclude $(DEPS)
endif

gdb: $(BINNAME)
	$(GDB) -se $(BINNAME)

clean:
	rm -rf obj
	rm -f *.o $(BINNAME)
	$(MAKE) -C tests clean

distclean: clean
	rm -rf dep
	rm -f *.d
	$(MAKE) -C tests distclean

