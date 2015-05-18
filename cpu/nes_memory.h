#ifndef NES_MEMORY_H
#define NES_MEMORY_H

#define MEM_SIZE 1<<16

class NesMemory {
	
	public:
		NesMemory();
		~NesMemory();
		void set_byte(const uint16_t addr, const uint8_t byte);
		uint8_t get_byte(const uint16_t addr);
        uint16_t get_word(const uint16_t addr);
		void print_memory_contents();

	private:
        uint8_t *memory_;
};


#endif  /* NES_MEMORY_H */
