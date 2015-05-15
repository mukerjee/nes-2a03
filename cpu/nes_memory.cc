#include <iostream>
#include "nes_memory.h"

NesMemory::NesMemory() {
#ifdef DEBUG
	memory_map_ = new std::map<uint16_t, uint8_t>();
#elseif HASH
	memory_map_ = new std::unordered_map<uint16_t, uint8_t>();
#else
    memory_map_ = malloc(MEM_SIZE * sizeof(uint8_t));
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
* @return Value stored at that address
*/
uint8_t NesMemory::get_byte(const uint16_t addr) {
	return (*memory_map_)[addr];
}

/**
* @brief Get a word from emulated 6502 RAM.
*
* @param addr Address to read.
*
* @return Value stored at that address
*/
uint16_t NesMemory::get_byte(const uint16_t addr) {
    return ((*memory_map_)[addr+1] << 8) | (*memory_map_)[addr];
}
		
/**
* @brief Print the contents of memory. Addresses may not be in order if DEBUG
*	flag is not set.
*/
void NesMemory::print_memory_contents() {
#ifdef DEBUG
	std::map<uint16_t, uint8_t>::iterator it;
	for (it = memory_map_->begin(); it != memory_map_->end(); it++) {
		printf("0x%02x\t%u\n", it->first, it->second);
	}
#elseif HASH
	std::unordered_map<uint16_t, uint8_t>::iterator it;
	for (it = memory_map_->begin(); it != memory_map_->end(); it++) {
		printf("0x%02x\t%u\n", it->first, it->second);
	}
#else
    for (int i = 0; i < MEM_SIZE; i++) {
		printf("0x%02x\t%u\n", i, memory_map_[i]);
	}    
#endif
	
}
