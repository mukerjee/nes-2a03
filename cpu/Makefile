.PHONY: clean

CC=g++
LD=g++
CFLAGS += -c -fpic
CPPFLAGS=$(CFLAGS)

LIB=libNesCpu.so
STATICLIB=libNesCpu.a
SOURCES=nes_memory.cc nes_cpu.cc
HEADERS=nes_memory.h nes_cpu.h
OBJS=$(SOURCES:.cc=.o)

all: $(STATICLIB)

%.o: %.cc $(HEADERS)
	$(CC) $(CPPFLAGS) $<  -o $@

$(LIB): $(OBJS) 
	$(LD) -shared -o $@ $(OBJS) $(LDFLAGS)

$(STATICLIB): $(OBJS) 
	ar rcs $@ $(OBJS)

clean:
	-rm -f $(LIB)
	-rm -f $(STATICLIB)
	-rm -f *.o 
