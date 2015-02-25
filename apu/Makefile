CPPFLAGS=-Ichannels/ -I../util/ -std=c++11 -O2
CXX=g++

SRCS=apu_test.cc apu.cc channels/channel.cc channels/pulse.cc channels/triangle.cc channels/noise.cc channels/dmc.cc ../util/apu_mixer.cc ../util/counter.cc ../util/resample.cc

OBJS=$(subst .cc,.o,$(SRCS))

EXE=apu_test

all: cpu

cpu: $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LDLIBS)

clean:
	rm $(OBJS) $(EXE)
