CPPFLAGS=-Ichannels/ -I./util/ -std=c++11 -O3 -fomit-frame-pointer
CXX=g++

LDFLAGS += -lportaudio

SRCS=apu_test.cc apu.cc channels/channel.cc channels/pulse.cc channels/triangle.cc channels/noise.cc channels/dmc.cc ./util/apu_mixer.cc ./util/counter.cc ./util/audio.cc

OBJS=$(subst .cc,.o,$(SRCS))

EXE=apu_test

all: apu

apu: $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LDLIBS)

clean:
	rm $(OBJS) $(EXE)
