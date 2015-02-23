#include <iostream>
#include "nes_memory.h"

NesMemory::NesMemory() {
#ifdef DEBUG
	memory_map_ = new std::map<uint16_t, uint8_t>();
#else
	memory_map_ = new std::unordered_map<uint16_t, uint8_t>();
#endif
}

NesMemory::~NesMemory() {
	delete memory_map_;
}

/**
* @brief Set a byte in emulated 6502 RAM.
*
* @param addr Memory address to write.
* @param byte Value to write
*/
void NesMemory::set_byte(const uint16_t addr, const uint8_t byte) {
	(*memory_map_)[addr] = byte;
}

/**
* @brief Get a byte from emulated 6502 RAM.
*
* @param addr Address to read.
*
* @return Value stored at that address; 0 if not set.
*/
uint8_t NesMemory::get_byte(const uint16_t addr) {
	return (*memory_map_)[addr];
}
		
/**
* @brief Print the contents of memory. Addresses may not be in order if DEBUG
*	flag is not set.
*/
void NesMemory::print_memory_contents() {
#ifdef DEBUG
	std::map<uint16_t, uint8_t>::iterator it;
#else
	std::unordered_map<uint16_t, uint8_t>::iterator it;
#endif
	
	for (it = memory_map_->begin(); it != memory_map_->end(); it++) {
		printf("0x%02x\t%u\n", it->first, it->second);
	}
}
