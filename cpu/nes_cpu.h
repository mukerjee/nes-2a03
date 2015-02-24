#ifndef NES_CPU_H
#define NES_CPU_H

#include "nes_memory.h"

class NesCpu {

	public:
		NesCpu();
		~NesCpu();



	private:

		// TODO: make map mapping opcode to <op method, addr mode> tuple

		/* TODO: make function that looks up the opcode in the above map, calls
		the appropriate addressing mode function to get the data value the op
		should operate on, and then calls the op.  */
		
		/*************** MEMORY ***************/
		NesMemory *memory_;


		/*************** REGISTERS ***************/

		/* TODO: We need a PC don't we? Depends whose job it is to feed
		instructions to the CPU class (i.e., what is the CPU class'
		interface---does some other class feed it one instruction at a time,
		and an outer "program runner" class tracks which instructions to
		execute, or does the caller of NesCpu just hand us a whole program and
		say "go" */

		uint8_t register_a_;
		uint8_t register_x_;
		uint8_t register_y_;

		bool carry_flag_;
		bool zero_flag_;
		bool interrupt_disable_;
		bool decimal_mode_flag_;
		bool break_command_;
		bool overflow_flag_;
		bool negative_flag_;


		/*************** OP CODES ***************/
		void lda(uint8_t value);


		/*************** ADDRESSING MODES ***************/
		inline uint8_t immediate(uint8_t constant);
		inline uint8_t zero_page(uint8_t address);
		inline uint8_t zero_page_x(uint8_t address);
		inline uint8_t zero_page_y(uint8_t address);
		// TODO: relative  (need to decide who implements PC)
		inline uint8_t absolute(uint16_t address);
		inline uint8_t absolute_x(uint16_t address);
		inline uint8_t absolute_y(uint16_t address);
		inline uint8_t indirect(uint16_t address_location);
		inline uint8_t indexed_indirect(uint16_t address_location);
		inline uint8_t indirect_indexed(uint16_t address_location);

		// helper function for indirect addressing modes
		inline uint16_t calculate_indirect_address(uint16_t address_location);
};

#endif  /* NES_CPU_H */