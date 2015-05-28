#include "nsfreader.h"
#include "nes_cpu.h"
#include "apu.h"

int main(int argc, char **argv) {
    NSFReader reader = NSFReader(argv[1]);
    APU apu;
    NesCpu cpu = NesCpu(&apu);
    cpu.init_nsf(&reader);
    sleep(10);
}
