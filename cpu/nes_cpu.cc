#include <iostream>
#include "nes_cpu.h"

NesCpu::NesCpu() {
	memory_ = new NesMemory();

	// TESTING
	std::cout << "CPU testing Memory" << std::endl;

	memory_->set_byte(0x00, 32);
	memory_->set_byte(0x05, 27);

	memory_->print_memory_contents();
}

NesCpu::~NesCpu() {
	delete memory_;
}
