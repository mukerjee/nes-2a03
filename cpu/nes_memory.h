#ifndef NES_MEMORY_H
#define NES_MEMORY_H

//#define DEBUG   // TODO: move this to make file
#define MEM_SIZE 2*16

#include <stdint.h>
#include <map>
#include <unordered_map>

class NesMemory {
	
	public:
		NesMemory();
		~NesMemory();
		void set_byte(const uint16_t addr, const uint8_t byte);
		uint8_t get_byte(const uint16_t addr);
        uint16_t get_word(const uint16_t addr);
		void print_memory_contents();

	private:

		// Use a hash table to represent memory: mem addr -> value
		// ordred_map is slower than unordered map, but since it keeps keys in
		// order we use it if we want to be able to print the memory contents
		// for debugging.
#ifdef DEBUG
		std::map<uint16_t, uint8_t> *memory_map_;
#elseif HASH
		std::unordered_map<uint16_t, uint8_t> *memory_map_;
#else
        uint8_t **memory_map_;
#endif
};


#endif  /* NES_MEMORY_H */
