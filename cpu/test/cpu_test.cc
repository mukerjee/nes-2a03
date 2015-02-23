#include <iostream>
#include "nes_cpu.h"

int main() {
	std::cout << "CPU Tester" << std::endl;

	NesCpu *cpu = new NesCpu();



	delete cpu;
}
