# Toolchain settings
TARGET  := 
CC      := $(TARGET)gcc
CXX     := $(TARGET)g++
AS      := $(TARGET)yasm
LD      := $(TARGET)ld
GDB     := $(TARGET)gdb

BUILD   ?= debug

# User Settings
AINCDIR     =
INCDIR      = . ..
LIBS        = mini
FLAGS       =
ASFLAGS     = -f elf64 -DPIC
CFLAGS      = -std=c11 -Wall -fno-stack-protector -nostdlib
CXXFLAGS    = -std=c++17
LDFLAGS     = -m elf_x86_64 --dynamic-linker /lib64/ld-linux-x86-64.so.2 -L. -L..
DEFFLAGS    = -DUSEMINI
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

alarm1: $(OBJDIR)/start.o $(OBJDIR)/alarm1.o
	$(LD) $(FLAGS) $(ZLDFLAGS) -o $@ $^ $(foreach l, $(LIBS), -l$l)

alarm2: $(OBJDIR)/start.o $(OBJDIR)/alarm2.o
	$(LD) $(FLAGS) $(ZLDFLAGS) -o $@ $^ $(foreach l, $(LIBS), -l$l)

alarm3: $(OBJDIR)/start.o $(OBJDIR)/alarm3.o
	$(LD) $(FLAGS) $(ZLDFLAGS) -o $@ $^ $(foreach l, $(LIBS), -l$l)

jmp1: $(OBJDIR)/start.o $(OBJDIR)/jmp1.o
	$(LD) $(FLAGS) $(ZLDFLAGS) -o $@ $^ $(foreach l, $(LIBS), -l$l)

ifneq ($(DEPS),)
  sinclude $(DEPS)
endif

clean:
	rm -rf obj
	rm -f *.o $(BINNAME)

distclean: clean
	rm -rf dep
	rm -f *.d

