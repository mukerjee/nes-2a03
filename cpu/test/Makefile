CC=g++
LD=g++
LIB=../libNesCpu.so
STATICLIB=../libNesCpu.a

INCLUDE += ..
CFLAGS += -I$(INCLUDE)

all: cpu_test

%: %.cc $(STATICLIB)
	$(CC) $(CFLAGS) $< -o $@ $(STATICLIB)

clean:
	-rm -f cpu_test
	-rm -f *.o 
