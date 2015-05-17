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


		/*************** EXECUTION ***************/
        void cpu_loop();
        uint8_t run_instruction(uint8_t opcode);


		/*************** INSTRUCTIONS ***************/
		void adc(uint16_t address);
		void AND(uint16_t address);
		void asl(uint16_t address);
		void asl();
        uint8_t bcc(uint16_t address);
        uint8_t bcs(uint16_t address);
        uint8_t beq(uint16_t address);
        void bit(uint16_t address);
        uint8_t bmi(uint16_t address);
        uint8_t bne(uint16_t address);
        uint8_t bpl(uint16_t address);
        void brk();
        uint8_t bvc(uint16_t address);
        uint8_t bvs(uint16_t address);
        void clc();
        void cld();
        void cli();
        void clv();
        void cmp(uint16_t address);
        void cpx(uint16_t address);
        void cpy(uint16_t address);
        void dec(uint16_t address);
        void dex();
        void dey();
        void eor(uint16_t address);
        void inc(uint16_t address);
        void inx();
        void iny();
        void jmp(uint16_t address);
        void jsr(uint16_t address);
		void lda(uint16_t address);
        void ldx(uint16_t address);
        void ldy(uint16_t address);
        void lsr(uint16_t address);
        void lsr();
        void nop();
        void ora(uint16_t address);
        void pha();
        void php();
        void pla();
        void plp();
        void rol(uint16_t address);
        void rol();
        void ror(uint16_t address);
        void ror();
        void rti();
        void rts();
        void sbc(uint16_t address);
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
		inline uint16_t immediate();
		inline uint16_t zero_page();
		inline uint16_t zero_page_x();
		inline uint16_t zero_page_y();
		inline uint16_t relative();
		inline uint16_t absolute();
		inline uint16_t absolute_x();
		inline uint16_t absolute_y();
		inline uint16_t indirect();
		inline uint16_t indexed_indirect();
		inline uint16_t indirect_indexed();

		/*************** TESTING ***************/
		void print_state();
};

#endif  /* NES_CPU_H */
