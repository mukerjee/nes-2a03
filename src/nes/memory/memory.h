#ifndef NES_MEMORY_MEMORY_H_
#define NES_MEMORY_MEMORY_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MEM_SIZE 1<<16

class Memory {	
 public:
    Memory();
    ~Memory();
    void LoadCart(const uint8_t* cart, const uint16_t start_address,
                  const size_t length);
    void LoadBank(const uint8_t* bank, const uint8_t bank_number,
                  const size_t length);
    void BankSwitch(const uint8_t bank_spot, const uint8_t bank_number);
    void SetByte(const uint16_t addr, const uint8_t byte);
    uint8_t GetByte(const uint16_t addr);
    uint16_t GetWord(const uint16_t addr);
    void PrintMemoryContents();

 private:
    uint8_t *memory_;
    uint8_t **banks_;
};


#endif  // NES_MEMORY_MEMORY_H_
