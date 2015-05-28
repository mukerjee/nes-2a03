.PHONY: clean

CC=g++
LD=g++
CFLAGS += -c -fpic -std=c++11 -O3 -fomit-frame-pointer
CPPFLAGS=$(CFLAGS) -I../apu/ -I../apu/util/ -I../apu/channels/ -I../cpu/ -I./

LDFLAGS += -lportaudio

LIB=libNSF.so
STATICLIB=libNSF.a
SOURCES=nsfreader.cc nsfplayer.cc ../cpu/nes_cpu.cc ../cpu/nes_memory.cc ../apu/apu.cc ../apu/channels/channel.cc ../apu/channels/pulse.cc ../apu/channels/triangle.cc ../apu/channels/noise.cc ../apu/channels/dmc.cc ../apu/util/apu_mixer.cc ../apu/util/counter.cc ../apu/util/audio.cc
HEADERS=nsfreader.h ../cpu/nes_cpu.h ../cpu/nes_memory.h ../apu/apu.h ../apu/util/audio.h
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
