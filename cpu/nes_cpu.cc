#include <iostream>
#include "nes_cpu.h"


/*************** CONSTRUCTOR/DESTRUCTOR ***************/
NesCpu::NesCpu() {
	memory_ = new NesMemory();

	// TESTING
	std::cout << "Testing CPU" << std::endl;

	lda(120);
	print_state();
	adc(4);
	print_state();
	adc(4);
	print_state();
	adc(4);
	print_state();
	adc(150);
	print_state();
}

NesCpu::~NesCpu() {
	delete memory_;
}
		

/*************** OP CODES ***************/
// Descriptions from: http://www.obelisk.demon.co.uk/6502/reference.html
		
/**
* @brief This instruction adds the contents of a memory location to the
*	accumulator together with the carry bit. If overflow occurs the carry bit is
*	set, this enables multiple byte addition to be performed.
*
* @param value
*/
void NesCpu::adc(uint8_t value) {
	// for arithmetic, pretend everything is signed
	int8_t original_a = (int8_t)register_a_;
	int8_t new_a = (int8_t)register_a_;
	new_a += (int8_t)value;
	if (carry_flag_) new_a++;
	register_a_ = (uint8_t)new_a;

	carry_flag_ = ((uint8_t)new_a < (uint8_t)original_a);
	zero_flag_ = (value == 0); // TODO: ???
	overflow_flag_ = (new_a < original_a);
	negative_flag_ = ((value & 0x80) == 0x80);  // bit 7 set?
}

/**
* @brief Loads a byte of memory into the accumulator setting the zero and
*	negative flags as appropriate.
*
* @param value
*/
void NesCpu::lda(uint8_t value) {
	register_a_ = value;

	zero_flag_ = (value == 0);  // TODO: ???
	negative_flag_ = ((value & 0x80) == 0x80);  // bit 7 set?
}
		

		
/*************** ADDRESSING MODES ***************/
// Descriptions from: http://www.obelisk.demon.co.uk/6502/addressing.html

/**
* @brief Immediate addressing allows the programmer to directly specify an 8
*	bit constant within the instruction. It is indicated by a '#' symbol followed
*	by an numeric expression.
*
* @param constant
*
* @return 
*/
uint8_t NesCpu::immediate(uint8_t constant) {
	return constant;
}
		
/**
* @brief An instruction using zero page addressing mode has only an 8 bit
*	address operand. This limits it to addressing only the first 256 bytes of
*	memory (e.g. $0000 to $00FF) where the most significant byte of the address is
*	always zero. In zero page mode only the least significant byte of the address
*	is held in the instruction making it shorter by one byte (important for space
*	saving) and one less memory fetch during execution (important for speed).
*
* @param address
*
* @return 
*/
uint8_t NesCpu::zero_page(uint8_t address) {
	return memory_->get_byte(address);
}
		
/**
* @brief The address to be accessed by an instruction using indexed zero page
*	addressing is calculated by taking the 8 bit zero page address from the
*	instruction and adding the current value of the X register to it. For example
*	if the X register contains $0F and the instruction LDA $80,X is executed then
*	the accumulator will be loaded from $008F (e.g. $80 + $0F => $8F).
*
* @param address
*
* @return 
*/
uint8_t NesCpu::zero_page_x(uint8_t address) {
	return memory_->get_byte(address + register_x_);
}
		
/**
* @brief The address to be accessed by an instruction using indexed zero page
*	addressing is calculated by taking the 8 bit zero page address from the
*	instruction and adding the current value of the Y register to it. This mode can
*	only be used with the LDX and STX instructions.
*
* @param address
*
* @return 
*/
uint8_t NesCpu::zero_page_y(uint8_t address) {
	return memory_->get_byte(address + register_y_);
}
		
/**
* @brief Instructions using absolute addressing contain a full 16 bit address
*	to identify the target location.
*
* @param address
*
* @return 
*/
uint8_t NesCpu::absolute(uint16_t address) {
	return memory_->get_byte(address);
}
		
/**
* @brief The address to be accessed by an instruction using X register indexed
*	absolute addressing is computed by taking the 16 bit address from the
*	instruction and added the contents of the X register. For example if X contains
*	$92 then an STA $2000,X instruction will store the accumulator at $2092 (e.g.
*	$2000 + $92).
*
* @param address
*
* @return 
*/
uint8_t NesCpu::absolute_x(uint16_t address) {
	return memory_->get_byte(address + register_x_);
}
		
/**
* @brief The Y register indexed absolute addressing mode is the same as the
*	previous mode only with the contents of the Y register added to the 16 bit
*	address from the instruction.
*
* @param address
*
* @return 
*/
uint8_t NesCpu::absolute_y(uint16_t address) {
	return memory_->get_byte(address + register_y_);
}
		
/**
* @brief JMP is the only 6502 instruction to support indirection. The
*	instruction contains a 16 bit address which identifies the location of the
*	least significant byte of another 16 bit memory address which is the real
*	target of the instruction.
*	
*	For example if location $0120 contains $FC and location $0121 contains $BA then
*	the instruction JMP ($0120) will cause the next instruction execution to occur
*	at $BAFC (e.g. the contents of $0120 and $0121).
*
* @param address
*
* @return 
*/
uint8_t NesCpu::indirect(uint16_t address_location) {
	return memory_->get_byte(calculate_indirect_address(address_location));
}
		
/**
* @brief Indexed indirect addressing is normally used in conjunction with a
*	table of address held on zero page. The address of the table is taken from the
*	instruction and the X register added to it (with zero page wrap around) to give
*	the location of the least significant byte of the target address.
*
* @param address
*
* @return 
*/
// TODO: zero page wrap around?
uint8_t NesCpu::indexed_indirect(uint16_t address_location) {
	return indirect(address_location + register_x_);
}
		
/**
* @brief Indirect indirect addressing is the most common indirection mode used
*	on the 6502. In instruction contains the zero page location of the least
*	significant byte of 16 bit address. The Y register is dynamically added to this
*	value to generated the actual target address for operation.
*
* @param address
*
* @return 
*/
uint8_t NesCpu::indirect_indexed(uint16_t address_location) {
	uint16_t actual_addr = calculate_indirect_address(address_location);
	return memory_->get_byte(actual_addr + register_y_);
}
		
/**
* @brief Given the memory location of the least significant byte of a 16 bit
*	address, retrieve that address from memory and return it.
*
* @param address_location
*
* @return 
*/
uint16_t NesCpu::calculate_indirect_address(uint16_t address_location) {
	uint8_t actual_addr_lo = memory_->get_byte(address_location);
	uint8_t actual_addr_hi = memory_->get_byte(address_location+1);
	uint16_t actual_addr = ((actual_addr_hi << 8) | actual_addr_lo);  // TODO: test
	return actual_addr;
}
		
		
/*************** TESTING ***************/
void NesCpu::print_state() {
	printf("\nA:\t0x%02x  (%u)\nX:\t0x%02x  (%u)\nY:\t0x%02x  (%u)\n\n",
		register_a_, register_a_,
		register_x_, register_x_,
		register_y_, register_y_);

	printf("CARRY\tZERO\tINTER\tDECIMAL\tBREAK\tOVERFLW\tNEG\n");
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n\n", carry_flag_, zero_flag_,
		interrupt_disable_, decimal_mode_flag_, break_command_,
		overflow_flag_, negative_flag_);
}
