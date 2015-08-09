CPPFLAGS=-I../audio_adapter/ -I../../../include/signals_and_slots/		\
-I../../audio_streamer/ -I../../../include/portaudio/ -std=c++11 -O3	\
-fomit-frame-pointer

CXX=g++

LDFLAGS += ../../../include/portaudio/libportaudio.a -framework CoreAudio	\
-framework AudioToolbox -framework AudioUnit -framework CoreServices

SRCS=apu_test.cc apu.cc pulse.cc triangle.cc noise.cc dmc.cc				\
../audio_adapter/audio_adapter.cc ../../audio_streamer/audio_streamer.cc

OBJS=$(subst .cc,.o,$(SRCS))



EXE=apu_test

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LDLIBS)

clean:
	- rm $(OBJS) $(EXE)
	- rm *~
