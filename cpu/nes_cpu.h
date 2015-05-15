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

		/* TODO: think about how to handle read/modify/write instructions.
		Separate set of adressing mode functions? Or they all return address
		but immediate? Maybe before going any further we need the outer program
		function; writing it might make the answer obvoius.  */
		
		

		/*************** MEMORY ***************/
		NesMemory *memory_;


		/*************** REGISTERS ***************/

		uint8_t register_a_;
		uint8_t register_x_;
		uint8_t register_y_;
        uint8_t register_s_;
        uint16_t register_pc_;

		bool carry_flag_;
		bool zero_flag_;
		bool interrupt_disable_;
		bool decimal_mode_flag_;
		bool break_command_;
		bool overflow_flag_;
		bool negative_flag_;


		/*************** OP CODES ***************/
		void adc(uint8_t value);
		void and(uint8_t value);
		void asl(bool mem, uint16_t address);
        void bcc(uint8_t value);
        void bcs(uint8_t value);
        void beq(uint8_t value);
        void bit(uint8_t value);
        void bmi(uint8_t value);
        void bne(uint8_t value);
        void bpl(uint8_t value);
        void brk();
        void bvc(uint8_t value);
        void bvs(uint8_t value);
        void clc();
        void cld();
        void cli();
        void clv();
        void cmp(uint8_t value);
        void cpx(uint8_t value);
        void cpy(uint8_t value);
        void dec(uint16_t address);
        void dex();
        void dey();
        void eor(uint8_t value);
        void inc(uint16_t address);
        void inx();
        void iny();
        void jmp(uint16_t address);
        void jsr(uint16_t address);
		void lda(uint8_t value);
        void ldx(uint8_t value);
        void ldy(uint8_t value);
        void lsr(bool mem, uint16_t address);
        void nop();
        void ora(uint8_t value);
        void pha();
        void php();
        void pla();
        void plp();
        void rol(bool mem, uint16_t address);
        void ror(bool mem, uint16_t address);
        void rti();
        void rts();
        void sbc(uint8_t value);
        void sec();
        void sed();
        void sei();
        void sta(uint16_t address);
        void stx(uint16_t address);
        void sty(uint16_t address);
        void tax();
        void tay();
        void tsx();
        void txa();
        void txs();
        void tya();

        /*************** HELPER FUNCTIONS ***************/
        void push_to_stack(uint8_t value);
        uint8_t pop_from_stack();
        void set_processor_status(uint8_t value);
        uint8_t get_processor_status();

		/*************** ADDRESSING MODES ***************/
		inline uint8_t immediate(uint8_t constant);
		inline uint8_t zero_page(uint8_t address);
		inline uint8_t zero_page_x(uint8_t address);
		inline uint8_t zero_page_y(uint8_t address);
		inline uint8_t relative(uint8_t offset):
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
