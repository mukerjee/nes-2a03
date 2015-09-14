.PHONY: all lib clean

LIB=../../lib
INCLUDE=../../include

CC=g++
LD=g++
CFLAGS += -c -fpic -std=c++11 -O3 -fomit-frame-pointer
CPPFLAGS=$(CFLAGS) -I../nes/ -I../nes/cpu -I../nes/memory/ -I../nes/apu/	\
-I../nes/audio_adapter -I../audio_streamer/ -I$(INCLUDE)/portaudio		\
-I$(INCLUDE)/signals_and_slots/ -I$(INCLUDE)/reader_writer_queue -I$(INCLUDE)

LDFLAGS += $(INCLUDE)/portaudio/libportaudio.a -framework CoreAudio	\
-framework AudioToolbox -framework AudioUnit -framework CoreServices -lCurses

SOURCES= nsf.cc nsf_reader.cc ../audio_streamer/audio_streamer.cc		\
../nes/nes.cc ../nes/apu/apu.cc ../nes/apu/dmc.cc ../nes/apu/noise.cc			\
../nes/apu/pulse.cc ../nes/apu/triangle.cc										\
../nes/audio_adapter/audio_adapter.cc ../nes/cpu/cpu.cc ../nes/memory/memory.cc	\

HEADERS= $(INCLUDE)/nsf.h nsf_reader.h ../audio_streamer/audio_streamer.h ../nes/nes.h	\
../nes/apu/apu.h ../nes/apu/channel.h ../nes/apu/dmc.h ../nes/apu/noise.h		\
../nes/apu/pulse.h ../nes/apu/triangle.h ../nes/audio_adapter/audio_adapter.h	\
../audio_streamer/audio_source.h ../nes/cpu/cpu.h ../nes/memory/memory.h		\
$(INCLUDE)/portaudio/portaudio.h $(INCLUDE)/reader_writer_queue/atomicops.h			\
$(INCLUDE)/reader_writer_queue/readerwriterqueue.h							\
$(INCLUDE)/signals_and_slots/delegate.h										\
$(INCLUDE)/signals_and_slots/gallant_signal.h								\

OBJS=$(SOURCES:.cc=.o)

LIBNSF=$(LIB)/libnsf.a
EXE=nsf_player

all: $(LIBNSF) $(EXE)
lib: $(LIBNSF)

$(LIBNSF): $(OBJS) $(HEADERS)
	ar rcs $@ $(OBJS)

$(EXE): $(LIBNSF) nsf_player.h nsf_player.o
	$(CC) $(LDFLAGS) $(LIBNSF) nsf_player.o -o $(EXE)

%.o: %.cc $(HEADERS)
	$(CC) $(CPPFLAGS) $<  -o $@

clean:
	- rm -f $(OBJS) $(LIBNSF) $(EXE) nsf_player.o
	- rm *~
