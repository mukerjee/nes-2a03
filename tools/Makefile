.PHONY: clean

CC=g++
LD=g++
CFLAGS += -c -fpic -std=c++11
CPPFLAGS=$(CFLAGS)

SOURCES=siddasm2.c
OBJS=$(SOURCES:.cc=.o)
EXE=nsf_disassembler

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $(OBJS)

%.o: %.cc 
	$(CC) $(CPPFLAGS) $<  -o $@

clean:
	-rm -f *.o 
	-rm -f $(EXE)
