.PHONY: clean

CC=g++
LD=g++
CFLAGS += -c -fpic
CPPFLAGS=$(CFLAGS)

LIB=libNSF.so
STATICLIB=libNSF.a
SOURCES=nsfreader.cc nsfplayer.cc ../cpu/nes_cpu.cc ../cpu/nes_memory.cc
HEADERS=nsfreader.h ../cpu/nes_cpu.h ../cpu/nes_memory.h
OBJS=$(SOURCES:.cc=.o)

all: $(STATICLIB) nsf_test

nsf_test: $(OBJS)
	$(CC) $(LDFLAGS) -o nsf_test $(OBJS)

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
