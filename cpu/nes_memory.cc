#include <iostream>
#include "nes_memory.h"

NesMemory::NesMemory() {
    memory_ = (uint8_t *)malloc(MEM_SIZE * sizeof(uint8_t));
}

NesMemory::~NesMemory() {
	free(memory_);
}

/**
* @brief Set a byte in emulated 6502 RAM.
*
* @param addr Memory address to write.
* @param byte Value to write
*/
void NesMemory::set_byte(const uint16_t addr, const uint8_t byte) {
	memory_[addr] = byte;
}

/**
* @brief Get a byte from emulated 6502 RAM.
*
* @param addr Address to read.
*
* @return Value stored at that address
*/
uint8_t NesMemory::get_byte(const uint16_t addr) {
	return memory_[addr];
}

/**
* @brief Get a word from emulated 6502 RAM.
*
* @param addr Address to read.
*
* @return Value stored at that address
*/
uint16_t NesMemory::get_word(const uint16_t addr) {
    return (memory_[addr+1] << 8) | memory_[addr];
}
		
/**
* @brief Print the contents of memory.
*/
void NesMemory::print_memory_contents() {
    for (int i = 0; i < MEM_SIZE; i++) {
		printf("0x%02x\t%u\n", i, memory_[i]);
	}	
}
