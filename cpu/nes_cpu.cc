#include <iostream>
#include "nes_cpu.h"


/*************** CONSTRUCTOR/DESTRUCTOR ***************/
NesCpu::NesCpu() {
	memory_ = new NesMemory();

	// TESTING
	std::cout << "Testing CPU" << std::endl;

	lda(23);
	print_state();
	ora(67);
	print_state();
}

NesCpu::~NesCpu() {
	delete memory_;
}
		

/*************** INSTRUCTIONS ***************/
// Descriptions from: http://www.obelisk.demon.co.uk/6502/reference.html

/**
* @brief This instruction adds the contents of a memory location to the
*	accumulator together with the carry bit. If overflow occurs the carry bit is
*	set, this enables multiple byte addition to be performed.
*
* @param value
*/
void NesCpu::adc(uint8_t value) {
	uint8_t original_a = register_a_;
    uint16_t result = register_a_ + value + carry_flag_;
    register_a_= (uint8_t)(result & 0xFF);

	carry_flag_ = result & 0x100;
	zero_flag_ = register_a_;
	negative_flag_ = register_a_ & 0x80;

    // set if sign of result is different than the sign of both
    // the inputs.
	overflow_flag_ = (original_a ^ register_a_) & (value ^ register_a_) & 0x80;
}
		
/**
* @brief A logical AND is performed, bit by bit, on the accumulator contents
*	using the contents of a byte of memory.
*
* @param value
*/
void NesCpu::AND(uint8_t value) {
	register_a_ &= value;

	zero_flag_ = register_a_;
	negative_flag_ = register_a_ & 0x80;
}

/**
* @brief This instruction compares the contents of the accumulator with another
*	memory held value and sets the zero and carry flags as appropriate.
*
* @param mem, address
*/
void NesCpu::asl(bool mem, uint16_t address) {
    uint8_t value = mem ? memory_->get_byte(address) : register_a_;

    carry_flag_ = value & 0x80;
    value <<= 1;

    if (mem)
        memory_->set_byte(address, value);        
    else
        register_a_ = value;

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

/**
* @brief If the carry flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::bcc(uint8_t value) {
    if (!carry_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief If the carry flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::bcs(uint8_t value) {
    if (carry_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief If the zero flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::beq(uint8_t value) {
    if (zero_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief This instructions is used to test if one or more bits are set in a
* target memory location. The mask pattern in A is ANDed with the value in
* memory to set or clear the zero flag, but the result is not kept. Bits 7 and 6
* of the value from memory are copied into the N and V flags.
*
* @param value
*/
void NesCpu::bit(uint8_t value) {
    uint8_t result = register_a_ & value;
    
    zero_flag_ = result;
    overflow_flag_ = result & 0x40;
    negative_flag_ = result & 0x80;
}

/**
* @brief If the negative flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::bmi(uint8_t value) {
    if (negative_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief If the zero flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::bne(uint8_t value) {
    if (!zero_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief If the negative flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::bpl(uint8_t value) {
    if (!negative_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief The BRK instruction forces the generation of an interrupt request. The
* program counter and processor status are pushed on the stack then the IRQ
* interrupt vector at $FFFE/F is loaded into the PC and the break flag in the
* status set to one.
*/
void NesCpu::brk() {
    register_pc_++; // brk increments the pc first
    push_to_stack(register_pc_ >> 8);
    push_to_stack(register_pc_ & 0xFF);
    
    push_to_stack(get_processor_status());

    register_pc_ = memory_->get_word(0xFFFE);

    break_command_ = 1;
}

/**
* @brief If the overflow flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::bvc(uint8_t value) {
    if (!overflow_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief If the overflow flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param value
*/
void NesCpu::bvs(uint8_t value) {
    if (overflow_flag_)
        register_pc_ += (int8_t)value;
}

/**
* @brief Set the carry flag to zero.
*/
void NesCpu::clc() {
    carry_flag_ = 0;
}

/**
* @brief Sets the decimal mode flag to zero.
*/
void NesCpu::cld() {
    decimal_mode_flag_ = 0;
}

/**
* @brief Clears the interrupt disable flag allowing normal interrupt requests to
* be serviced.
*/
void NesCpu::cli() {
    interrupt_disable_ = 0;
}

/**
* @brief Clears the overflow flag.
*/
void NesCpu::clv() {
    overflow_flag_ = 0;
}

/**
* @brief This instruction compares the contents of the accumulator with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param value
*/
void NesCpu::cmp(uint8_t value) {
    uint8_t result = register_a_ - value;
    carry_flag_ = result >= 0;
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief This instruction compares the contents of the X register with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param value
*/
void NesCpu::cpx(uint8_t value) {
    uint8_t result = register_x_ - value;
    carry_flag_ = result >= 0;
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief This instruction compares the contents of the Y register with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param value
*/
void NesCpu::cpy(uint8_t value) {
    uint8_t result = register_y_ - value;
    carry_flag_ = result >= 0;
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief Subtracts one from the value held at a specified memory location
* setting the zero and negative flags as appropriate.
*
* @param value
*/
void NesCpu::dec(uint16_t address) {
    uint8_t result = memory_->get_byte(address) - 1;
    memory_->set_byte(address, result);
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief Subtracts one from the X register setting the zero and negative flags
* as appropriate.
*/
void NesCpu::dex() {
    register_x_--;
    zero_flag_ = register_x_;
    negative_flag_ = register_x_ & 0x80;
}

/**
* @brief Subtracts one from the Y register setting the zero and negative flags
* as appropriate.
*/
void NesCpu::dey() {
    register_y_--;
    zero_flag_ = register_y_;
    negative_flag_ = register_y_ & 0x80;
}

/**
* @brief An exclusive OR is performed, bit by bit, on the accumulator contents
* using the contents of a byte of memory.
*
* @param value
*/
void NesCpu::eor(uint8_t value) {
    register_a_ = register_a_ ^ value;
    zero_flag_ = register_a_;
    negative_flag_ = register_a_ & 0x80;
}

/**
* @brief Adds one to the value held at a specified memory location setting the
* zero and negative flags as appropriate.
*
* @param value
*/
void NesCpu::inc(uint16_t address) {
    uint8_t result = memory_->get_byte(address) + 1;
    memory_->set_byte(address, result);
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief Adds one to the X register setting the zero and negative flags as
* appropriate.
*/
void NesCpu::inx() {
    register_x_++;
    zero_flag_ = register_x_;
    negative_flag_ = register_x_ & 0x80;
}

/**
* @brief Adds one to the Y register setting the zero and negative flags as
* appropriate.
*/
void NesCpu::iny() {
    register_y_++;
    zero_flag_ = register_y_;
    negative_flag_ = register_y_ & 0x80;
}

/**
* @brief Sets the program counter to the address specified by the operand.
*
* @param address
*/
void NesCpu::jmp(uint16_t address) {
    register_pc_ = address;
}

/**
* @brief The JSR instruction pushes the address (minus one) of the return point
* on to the stack and then sets the program counter to the target memory
* address.
*
* @param address
*/
void NesCpu::jsr(uint16_t address) {
    push_to_stack(register_pc_ >> 8);
    push_to_stack(register_pc_ & 0xFF);
    register_pc_ = address;
}

/**
* @brief Loads a byte of memory into the accumulator setting the zero and
*	negative flags as appropriate.
*
* @param value
*/
void NesCpu::lda(uint8_t value) {
	register_a_ = value;

	zero_flag_ = register_a_;
	negative_flag_ = register_a_ & 0x80;
}

/**
* @brief Loads a byte of memory into the X register setting the zero and
* negative flags as appropriate.
*
* @param value
*/
void NesCpu::ldx(uint8_t value) {
	register_x_ = value;

	zero_flag_ = register_x_;
	negative_flag_ = register_x_ & 0x80;
}

/**
* @brief Loads a byte of memory into the Y register setting the zero and
* negative flags as appropriate.
*
* @param value
*/
void NesCpu::ldy(uint8_t value) {
	register_y_ = value;

	zero_flag_ = register_y_;
	negative_flag_ = register_y_ & 0x80;
}

/**
* @brief Each of the bits in A or M is shift one place to the right. The bit
* that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
*
* @param mem, address
*/
void NesCpu::lsr(bool mem, uint16_t address) {
    uint8_t value = mem ? memory_->get_byte(address) : register_a_;

    carry_flag_ = value & 0x01;
    value >>= 1;

    if (mem)
        memory_->set_byte(address, value);        
    else
        register_a_ = value;

    zero_flag_ = value; // not just register_a_
    negative_flag_ = value & 0x80;
}

/**
* @brief The NOP instruction causes no changes to the processor other than the
* normal incrementing of the program counter to the next instruction.
*/
void NesCpu::nop() {
}


/**
* @brief An inclusive OR is performed, bit by bit, on the accumulator contents
* using the contents of a byte of memory.
*
* @param value
*/
void NesCpu::ora(uint8_t value) {
	register_a_ |= value;

	zero_flag_ = register_a_;
	negative_flag_ = register_a_ & 0x80;
}

/**
* @brief Pushes a copy of the accumulator on to the stack.
*/
void NesCpu::pha() {
    push_to_stack(register_a_);
}

/**
* @brief Pushes a copy of the status flags on to the stack.
*/
void NesCpu::php() {
    push_to_stack(get_processor_status());
}

/**
* @brief Pulls an 8 bit value from the stack and into the accumulator. The zero
* and negative flags are set as appropriate.
*/
void NesCpu::pla() {
    register_a_ = pop_from_stack();
    
    zero_flag_ = register_a_;
    negative_flag_ = register_a_ & 0x80;
}

/**
* @brief Pulls an 8 bit value from the stack and into the processor flags. The
* flags will take on new states as determined by the value pulled.
*/
void NesCpu::plp() {
    set_processor_status(pop_from_stack());
}

/**
* @brief Move each of the bits in either A or M one place to the left. Bit 0 is
* filled with the current value of the carry flag whilst the old bit 7 becomes
* the new carry flag value.
*
* @param mem, address
*/
void NesCpu::rol(bool mem, uint16_t address) {
    uint8_t value = mem ? memory_->get_byte(address) : register_a_;

    uint8_t b_0 = carry_flag_;
    carry_flag_ = value & 0x80;
    value <<= 1;
    value |= b_0;

    if (mem)
        memory_->set_byte(address, value);        
    else
        register_a_ = value;

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

/**
* @brief Move each of the bits in either A or M one place to the right. Bit 7 is
* filled with the current value of the carry flag whilst the old bit 0 becomes
* the new carry flag value.
*
* @param mem, address
*/
void NesCpu::ror(bool mem, uint16_t address) {
    uint8_t value = mem ? memory_->get_byte(address) : register_a_;

    uint8_t b_7 = carry_flag_;
    carry_flag_ = value & 0x01;
    value >>= 1;
    value |= (b_7 << 7);

    if (mem)
        memory_->set_byte(address, value);        
    else
        register_a_ = value;

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

/**
* @brief The RTI instruction is used at the end of an interrupt processing
* routine. It pulls the processor flags from the stack followed by the program
* counter.
*/
void NesCpu::rti() {
    set_processor_status(pop_from_stack());
    register_pc_ = pop_from_stack();
    register_pc_ |= (pop_from_stack() << 8);
}

/**
* @brief The RTS instruction is used at the end of a subroutine to return to the
* calling routine. It pulls the program counter (minus one) from the stack.
*/
void NesCpu::rts() {
    register_pc_ = pop_from_stack();
    register_pc_ |= (pop_from_stack() << 8);
}

/**
* @brief This instruction subtracts the contents of a memory location to the
* accumulator together with the not of the carry bit. If overflow occurs the
* carry bit is clear, this enables multiple byte subtraction to be performed.
*
* @param value
*/
void NesCpu::sbc(uint8_t value) {
	uint8_t original_a = register_a_;
    register_a_ = register_a_ - value - (1 - carry_flag_);

	carry_flag_ = register_a_ < value + (1 - carry_flag_);
	zero_flag_ = register_a_;
	negative_flag_ = register_a_ & 0x80;

    // set if sign of result is different than the sign of both
    // the inputs.
	overflow_flag_ = (original_a ^ register_a_) & (value ^ register_a_) & 0x80;
}

/**
* @brief Set the carry flag to one.
*/
void NesCpu::sec() {
    carry_flag_ = 1;
}

/**
* @brief Set the decimal mode flag to one.
*/
void NesCpu::sed() {
    decimal_mode_flag_ = 1;
}

/**
* @brief Set the interrupt disable flag to one.
*/
void NesCpu::sei() {
    interrupt_disable_ = 1;
}

/**
* @brief Stores the contents of the accumulator into memory.
*
* @param address
*/
void NesCpu::sta(uint16_t address) {
    memory_->set_byte(address, register_a_);
}

/**
* @brief Stores the contents of the X register into memory.
*
* @param address
*/
void NesCpu::stx(uint16_t address) {
    memory_->set_byte(address, register_x_);
}

/**
* @brief Stores the contents of the Y register into memory.
*
* @param address
*/
void NesCpu::sty(uint16_t address) {
    memory_->set_byte(address, register_y_);
}

/**
* @brief Copies the current contents of the accumulator into the X register and
* sets the zero and negative flags as appropriate.
*/
void NesCpu::tax() {
    register_x_ = register_a_;

    zero_flag_ = register_x_;
    negative_flag_ = register_x_ & 0x80;
}

/**
* @brief Copies the current contents of the accumulator into the Y register and
* sets the zero and negative flags as appropriate.
*/
void NesCpu::tay() {
    register_x_ = register_a_;

    zero_flag_ = register_y_;
    negative_flag_ = register_y_ & 0x80;
}

/**
* @brief Copies the current contents of the stack register into the X register
* and sets the zero and negative flags as appropriate.
*/
void NesCpu::tsx() {
    register_x_ = register_s_;

    zero_flag_ = register_x_;
    negative_flag_ = register_x_ & 0x80;
}

/**
* @brief Copies the current contents of the X register into the accumulator and
* sets the zero and negative flags as appropriate.
*/
void NesCpu::txa() {
    register_a_ = register_x_;

    zero_flag_ = register_a_;
    negative_flag_ = register_a_ & 0x80;
}

/**
* @brief Copies the current contents of the X register into the stack register.
*/
void NesCpu::txs() {
    register_s_ = register_x_;
}

/**
* @brief Copies the current contents of the Y register into the accumulator and
* sets the zero and negative flags as appropriate.
*/
void NesCpu::tya() {
    register_a_ = register_y_;

    zero_flag_ = register_a_;
    negative_flag_ = register_a_ & 0x80;
}

/*************** HELPER FUNCTIONS ***************/

/**
* @brief pushes the input byte onto the stack and adjusts the stack pointer.
*
* @param value
*/
void NesCpu::push_to_stack(uint8_t value) {
    memory_->set_byte(0x100 + register_s_, value);
    register_s_--;
}

/**
* @brief pops a byte off the stack and returns it, adjusting the stack pointer.
*
* @return 
*/
uint8_t NesCpu::pop_from_stack() {		
    uint8_t result = memory_->get_byte(0x100 + register_s_);
    register_s_++;
    return result;
}

// Descriptions from: http://nesdev.com/6502.txt
/**
* @brief sets the processors status flags based on the bits in the input.
*
* @param value
*/
void NesCpu::set_processor_status(uint8_t value) {
    carry_flag_ = value & 0x01;
    zero_flag_ = value & 0x02;
    interrupt_disable_ = value & 0x04;
    decimal_mode_flag_ = value & 0x08;
    break_command_ = value & 0x10;
    overflow_flag_ = value & 0x40;
    negative_flag_ = value & 0x80;
}

/**
* @brief returns the set of current processor status flags as a byte.
*
* @return 
*/
uint8_t NesCpu::get_processor_status() {
    uint8_t status = 0;
    status += carry_flag_;
    status += zero_flag_ << 1;
    status += interrupt_disable_ << 2;
    status += decimal_mode_flag_ << 3;
    status += break_command_ << 4;
    status += 1 << 5;
    status += overflow_flag_ << 6;
    status += negative_flag_ << 7;
    return status;
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
* @brief Relative addressing mode is used by branch instructions (e.g. BEQ, BNE,
* etc.) which contain a signed 8 bit relative offset (e.g. -128 to +127) which
* is added to program counter if the condition is true. As the program counter
* itself is incremented during instruction execution by two the effective
* address range for the target instruction must be with -126 to +129 bytes of
* the branch.
*
* @param offset
*
* @return
*/
uint8_t NesCpu::relative(uint8_t offset) {
    return offset;
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
	return memory_->get_byte(memory_->get_word(address_location));
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
	uint16_t actual_addr = memory_->get_word(address_location);
	return memory_->get_byte(actual_addr + register_y_);
}		

		
/*************** TESTING ***************/
void NesCpu::print_state() {
	printf("\nA:\t0x%02x  (%u)\nX:\t0x%02x  (%u)\nY:\t0x%02x  (%u)\n\n",
		register_a_, register_a_,
		register_x_, register_x_,
		register_y_, register_y_);

    printf("\nS:\t0x%02x  (%u)\nPC:\t0x%02x  (%u)\n\n",
           register_s_, register_s_,
           register_pc_, register_pc_);

	printf("CARRY\tZERO\tINTER\tDECIMAL\tBREAK\tOVERFLW\tNEG\n");
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n\n", carry_flag_, zero_flag_,
		interrupt_disable_, decimal_mode_flag_, break_command_,
		overflow_flag_, negative_flag_);
}
