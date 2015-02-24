CPPFLAGS=-Iapu/ -Iapu/channels/ -Iutil/ -std=c++11 -O2
CXX=g++

SRCS=cpu.cc apu/apu.cc apu/channels/channel.cc apu/channels/pulse.cc apu/channels/triangle.cc apu/channels/noise.cc apu/channels/dmc.cc util/apu_mixer.cc util/counter.cc util/resample.cc

OBJS=$(subst .cc,.o,$(SRCS))

all: cpu

cpu: $(OBJS)
	$(CXX) $(LDFLAGS) -o cpu_test $(OBJS) $(LDLIBS)

clean:
	rm $(OBJS)
