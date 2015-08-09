#ifndef NES_MEMORY_MEMORY_H_
#define NES_MEMORY_MEMORY_H_

#define MEM_SIZE 1<<16

class Memory {	
 public:
    Memory();
    ~Memory();
    void SetByte(const uint16_t addr, const uint8_t byte);
    uint8_t GetByte(const uint16_t addr);
    uint16_t GetWord(const uint16_t addr);
    void PrintMemoryContents();

 private:
    uint8_t *memory_;
};


#endif  // NES_MEMORY_MEMORY_H_
