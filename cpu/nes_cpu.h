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

		/* TODO: right now I'm storing bytes in memory as uint8_t, but if I
		pull them out to do math on them, I cast them to int8_t. Is that ok? */

		/* TODO: think about how to handle read/modify/write instructions.
		Separate set of adressing mode functions? Or they all return address
		but immediate? Maybe before going any further we need the outer program
		function; writing it might make the answer obvoius.  */
		
		

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


		/*************** EXECUTION ***************/



		/*************** INSTRUCTIONS ***************/
		/* Group 1: operate on accumulator */
		void adc(uint8_t value);
		void lda(uint8_t value);
		void AND(uint8_t value);

		/* Group 2: operate on accumulator or memory (read/modify/write) */
		// TODO: the execution function needs to take the return value and assign it either to memory or accumulator
		uint8_t asl(uint8_t value);

		/* Group 3 */


		/*************** ADDRESSING MODES ***************/
		// TODO: make these return address; get value in execution wrapper function?
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
		

		/*************** TESTING ***************/
		void print_state();
};

#endif  /* NES_CPU_H */
