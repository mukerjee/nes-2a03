#include "memory.h"

Memory::Memory() {
    memory_ = (uint8_t *)malloc(MEM_SIZE * sizeof(uint8_t));
}

Memory::~Memory() {
	free(memory_);
}

/**
* @brief Set a byte in emulated 6502 RAM.
*
* @param addr Memory address to write.
* @param byte Value to write
*/
void Memory::SetByte(const uint16_t addr, const uint8_t byte) {
    if (addr < 0x2000) { // mirroring
        uint16_t base_addr = addr % 0x0800;
        memory_[base_addr] = byte;
        memory_[base_addr + 0x0800] = byte;
        memory_[base_addr + 0x1000] = byte;
        memory_[base_addr + 0x1800] = byte;
    } else {
        memory_[addr] = byte;
    }
}

/**
* @brief Get a byte from emulated 6502 RAM.
*
* @param addr Address to read.
*
* @return Value stored at that address
*/
uint8_t Memory::GetByte(const uint16_t addr) {
	return memory_[addr];
}

/**
* @brief Get a word from emulated 6502 RAM.
*
* @param addr Address to read.
*
* @return Value stored at that address
*/
uint16_t Memory::GetWord(const uint16_t addr) {
    return (memory_[addr+1] << 8) | memory_[addr];
}
		
/**
* @brief Print the contents of memory.
*/
void Memory::PrintMemoryContents() {
    for (int i = 0; i < MEM_SIZE; i++) {
		printf("0x%02x\t%u\n", i, memory_[i]);
	}
}
