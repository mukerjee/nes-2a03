#ifndef NES_CPU_H
#define NES_CPU_H

#include "nes_memory.h"

class NesCpu {

	public:
		NesCpu();
		~NesCpu();

		// TODO: what interface? Let the NSF reader parse how long each instruction is and just offer one function per addressing mode (e.g., immediate(op, arg))?
	
	private:
		NesMemory *memory_;
		uint8_t register_a;
		uint8_t register_x;
		uint8_t register_y;
		// TODO: need a status register? or just some bool flags, one per bit?
};

#endif  /* NES_CPU_H */
