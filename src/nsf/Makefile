.PHONY: clean

CC=g++
LD=g++
CFLAGS += -c -fpic -std=c++11 -O3 -fomit-frame-pointer
CPPFLAGS=$(CFLAGS) -I../nes/ -I../nes/cpu -I../nes/memory/ -I../nes/apu/	\
-I../nes/audio_adapter -I../audio_streamer/ -I../../include/portaudio		\
-I../../include/signals_and_slots/ -I../../include/reader_writer_queue

LDFLAGS += ../../include/portaudio/libportaudio.a -framework CoreAudio	\
-framework AudioToolbox -framework AudioUnit -framework CoreServices

SOURCES= nsf_player.cc nsf_reader.cc ../audio_streamer/audio_streamer.cc		\
../nes/nes.cc ../nes/apu/apu.cc ../nes/apu/dmc.cc ../nes/apu/noise.cc			\
../nes/apu/pulse.cc ../nes/apu/triangle.cc										\
../nes/audio_adapter/audio_adapter.cc ../nes/cpu/cpu.cc ../nes/memory/memory.cc

HEADERS= nsf_reader.h ../audio_streamer/audio_streamer.h ../nes/nes.h			\
../nes/apu/apu.h ../nes/apu/channel.h ../nes/apu/dmc.h ../nes/apu/noise.h		\
../nes/apu/pulse.h ../nes/apu/triangle.h ../nes/audio_adapter/audio_adapter.h	\
../nes/audio_adapter/audio_source.h ../nes/cpu/cpu.h ../nes/memory/memory.h		\
../../include/portaudio/portaudio.h ../../include/reader_writer_queue/atomicops.h			\
../../include/reader_writer_queue/readerwriterqueue.h							\
../../include/signals_and_slots/delegate.h										\
../../include/signals_and_slots/gallant_signal.h

OBJS=$(SOURCES:.cc=.o)

EXE=nsf_player

all: $(EXE)

$(EXE): $(OBJS) $(HEADERS)
	$(CC) $(LDFLAGS) -o $(EXE) $(OBJS)

%.o: %.cc $(HEADERS)
	$(CC) $(CPPFLAGS) $<  -o $@

clean:
	- rm -f $(OBJS) $(EXE)
	- rm *~
