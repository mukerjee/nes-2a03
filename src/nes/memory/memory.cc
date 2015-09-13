#include "memory.h"

Memory::Memory() {
    memory_ = (uint8_t *)calloc(MEM_SIZE, sizeof(uint8_t));
    banks_ = (uint8_t **)calloc(256, sizeof(uint8_t *));
    for (int i = 0; i < 256; i++) {
        banks_[i] = (uint8_t *)malloc(4096 * sizeof(uint8_t));
    }
}

Memory::~Memory() {
	free(memory_);
    for(int i = 0; i < 256; i++) {
        free(banks_[i]);
    }
    free(banks_);
}

/**
* @brief Load a cartridge into emulated 6502 RAM.
*
* @param cart Block of memory representing the cartridge.
* @param start_address The 6502 RAM address where the cartridge should
*                      be placed.
* @param length Size of the cartridge in bytes.
*/
void Memory::LoadCart(const uint8_t* cart, const uint16_t start_address,
                      const size_t length) {
    memcpy(&memory_[start_address], cart, length);
}

/**
* @ brief Loads a single bank into an NSF-style memory mapper.
*
* @param bank Block of memory representing the bank.
* @param bank_nubmer Which of the 256 banks to store this data.
* @param length Size of the bank in bytes (should be at most 4K).
*/
void Memory::LoadBank(const uint8_t* bank, const uint8_t bank_number,
                      const size_t length) {
    memcpy(banks_[bank_number], bank, length);
}

/**
* @brief Switches banks using an NSF-style memory mapper. Assumes 8 4K bank spots
*        starting at 0x8000 in the 6502's memory.
*
* @param bank_spot The space in 6502 memory to load the bank into (0 - 7).
* @param bank_number The bank to load into the bank_spot.
*/
void Memory::BankSwitch(const uint8_t bank_spot, const uint8_t bank_number) {
    uint16_t start_address = 0x8000 + (bank_spot * 4096);
    memcpy(&memory_[start_address], banks_[bank_number], 4096);
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
