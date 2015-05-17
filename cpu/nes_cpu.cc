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

/*************** EXECUTION ***************/
/**
* @brief loop through all instructions, executing them in sequence.
*/
void NesCpu::cpu_loop() {
    uint8_t cycles;
    for(;;) {
        uint8_t opcode = memory_->get_byte(register_pc_++);
    
        cycles = run_instruction(opcode);
    }
}

// returns how many cycles this instruction took
/**
* @brief runs an instruction given an opcode and returns the number of 6502
* cycles it took to run that instruction.
*
* @param opcode
*
* @return
*/
uint8_t NesCpu::run_instruction(uint8_t opcode) {
    uint16_t addr;
    switch(opcode) {
    case 0x69: // immediate
        adc(immediate());
        return 2;
    case 0x65:
        adc(zero_page());
        return 3;
    case 0x75:
        adc(zero_page_x());
        return 4;
    case 0x6D:
        adc(absolute());
        return 4;
    case 0x7D:
        addr = absolute_x();
        adc(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x79:
        addr = absolute_y();
        adc(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x61:
        adc(indexed_indirect());
        return 6;
    case 0x71:
        addr = indirect_indexed();
        adc(addr);
        return (addr % 0x100 == 0xFF) ? 6 : 5;

    case 0x29:
        AND(immediate());
        return 2;
    case 0x25:
        AND(zero_page());
        return 3;
    case 0x35:
        AND(zero_page_x());
        return 4;
    case 0x2D:
        AND(absolute());
        return 4;
    case 0x3D:
        addr = absolute_x();
        AND(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x39:
        addr = absolute_y();
        AND(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x21:
        AND(indexed_indirect());
        return 6;
    case 0x31:
        addr = indirect_indexed();
        AND(addr);
        return (addr % 0x100 == 0xFF) ? 6 : 5;

    case 0x0A: // accumulator
        asl();
        return 2;
    case 0x06:
        asl(zero_page());
        return 5;
    case 0x16:
        asl(zero_page_x());
        return 6;
    case 0x0E:
        asl(absolute());
        return 6;
    case 0x1E:
        asl(absolute_x());
        return 7;

    case 0x90:
        return 2 + bcc(relative());

    case 0xB0:
        return 2 + bcs(relative());

    case 0xF0:
        return 2 + beq(relative());

    case 0x24:
        bit(zero_page());
        return 3;
    case 0x2C:
        bit(absolute());
        return 4;
        
    case 0x30:
        return 2 + bmi(relative());

    case 0xD0:
        return 2 + bne(relative());

    case 0x10:
        return 2 + bpl(relative());

    case 0x00:
        brk();
        return 7;

    case 0x50:
        return 2 + bvc(relative());

    case 0x70:
        return 2 + bvs(relative());

    case 0x18:
        clc();
        return 2;

    case 0xD8:
        cld();
        return 2;

    case 0x58:
        cli();
        return 2;

    case 0xB8:
        clv();
        return 2;

    case 0xC9:
        cmp(immediate());
        return 2;
    case 0xC5:
        cmp(zero_page());
        return 3;
    case 0xD5:
        cmp(zero_page_x());
        return 4;
    case 0xCD:
        cmp(absolute());
        return 4;
    case 0xDD:
        addr = absolute_x();
        cmp(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0xD9:
        addr = absolute_y();
        cmp(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0xC1:
        cmp(indexed_indirect());
        return 6;
    case 0xD1:
        addr = indirect_indexed();
        cmp(addr);
        return (addr % 0x100 == 0xFF) ? 6 : 5;

    case 0xE0:
        cpx(immediate());
        return 2;
    case 0xE4:
        cpx(zero_page());
        return 3;
    case 0xEC:
        cpx(absolute());
        return 4;

    case 0xC0:
        cpy(immediate());
        return 2;
    case 0xC4:
        cpy(zero_page());
        return 3;
    case 0xCC:
        cpy(absolute());
        return 4;

    case 0xC6:
        dec(zero_page());
        return 5;
    case 0xD6:
        dec(zero_page_x());
        return 6;
    case 0xCE:
        dec(absolute());
        return 6;
    case 0xDE:
        dec(absolute_x());
        return 7;

    case 0xCA:
        dex();
        return 2;

    case 0x88:
        dey();
        return 2;

    case 0x49:
        eor(immediate());
        return 2;
    case 0x45:
        eor(zero_page());
        return 3;
    case 0x55:
        eor(zero_page_x());
        return 4;
    case 0x4D:
        eor(absolute());
        return 4;
    case 0x5D:
        addr = absolute_x();
        eor(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x59:
        addr = absolute_y();
        eor(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x41:
        eor(indexed_indirect());
        return 6;
    case 0x51:
        addr = indirect_indexed();
        eor(addr);
        return (addr % 0x100 == 0xFF) ? 6 : 5;

    case 0xE6:
        inc(zero_page());
        return 5;
    case 0xF6:
        inc(zero_page_x());
        return 6;
    case 0xEE:
        inc(absolute());
        return 6;
    case 0xFE:
        inc(absolute_x());
        return 7;

    case 0xE8:
        inx();
        return 2;

    case 0xC8:
        iny();
        return 2;

    case 0x4C:
        jmp(absolute());
        return 3;
    case 0x6C:
        jmp(indirect());
        return 5;

    case 0x20:
        jsr(absolute());
        return 6;

    case 0xA9:
        lda(immediate());
        return 2;
    case 0xA5:
        lda(zero_page());
        return 3;
    case 0xB5:
        lda(zero_page_x());
        return 4;
    case 0xAD:
        lda(absolute());
        return 4;
    case 0xBD:
        addr = absolute_x();
        lda(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0xB9:
        addr = absolute_y();
        lda(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0xA1:
        lda(indexed_indirect());
        return 6;
    case 0xB1:
        addr = indirect_indexed();
        lda(addr);
        return (addr % 0x100 == 0xFF) ? 6 : 5;

    case 0xA2:
        ldx(immediate());
        return 2;
    case 0xA6:
        ldx(zero_page());
        return 3;
    case 0xB6:
        ldx(zero_page_y());
        return 4;
    case 0xAE:
        ldx(absolute());
        return 4;
    case 0xBE:
        addr = absolute_y();
        ldx(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;

    case 0xA0:
        ldy(immediate());
        return 2;
    case 0xA4:
        ldy(zero_page());
        return 3;
    case 0xB4:
        ldy(zero_page_x());
        return 4;
    case 0xAC:
        ldy(absolute());
        return 4;
    case 0xBC:
        addr = absolute_x();
        ldy(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;

    case 0x4A: // accumulator
        lsr();
        return 2;
    case 0x46:
        lsr(zero_page());
        return 5;
    case 0x56:
        lsr(zero_page_x());
        return 6;
    case 0x4E:
        lsr(absolute());
        return 6;
    case 0x5E:
        lsr(absolute_x());
        return 7;

    case 0xEA:
        nop();
        return 2;

    case 0x09:
        ora(immediate());
        return 2;
    case 0x05:
        ora(zero_page());
        return 3;
    case 0x15:
        ora(zero_page_x());
        return 4;
    case 0x0D:
        ora(absolute());
        return 4;
    case 0x1D:
        addr = absolute_x();
        ora(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x19:
        addr = absolute_y();
        ora(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0x01:
        ora(indexed_indirect());
        return 6;
    case 0x11:
        addr = indirect_indexed();
        ora(addr);
        return (addr % 0x100 == 0xFF) ? 6 : 5;
        
    case 0x48:
        pha();
        return 3;

    case 0x08:
        php();
        return 3;

    case 0x68:
        pla();
        return 4;
        
    case 0x28:
        plp();
        return 4;

    case 0x2A: // accumulator
        rol();
        return 2;
    case 0x26:
        rol(zero_page());
        return 5;
    case 0x36:
        rol(zero_page_x());
        return 6;
    case 0x2E:
        rol(absolute());
        return 6;
    case 0x3E:
        rol(absolute_x());
        return 7;

    case 0x6A: // accumulator
        ror();
        return 2;
    case 0x66:
        ror(zero_page());
        return 5;
    case 0x76:
        ror(zero_page_x());
        return 6;
    case 0x6E:
        ror(absolute());
        return 6;
    case 0x7E:
        ror(absolute_x());
        return 7;

    case 0x40:
        rti();
        return 6;

    case 0x60:
        rts();
        return 6;

    case 0xE9:
        sbc(immediate());
        return 2;
    case 0xE5:
        sbc(zero_page());
        return 3;
    case 0xF5:
        sbc(zero_page_x());
        return 4;
    case 0xED:
        sbc(absolute());
        return 4;
    case 0xFD:
        addr = absolute_x();
        sbc(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0xF9:
        addr = absolute_y();
        sbc(addr);
        return (addr % 0x100 == 0xFF) ? 5 : 4;
    case 0xE1:
        sbc(indexed_indirect());
        return 6;
    case 0xF1:
        addr = indirect_indexed();
        sbc(addr);
        return (addr % 0x100 == 0xFF) ? 6 : 5;

    case 0x38:
        sec();
        return 2;

    case 0xF8:
        sed();
        return 2;

    case 0x78:
        sei();
        return 2;

    case 0x85:
        sta(zero_page());
        return 3;
    case 0x95:
        sta(zero_page_x());
        return 4;
    case 0x8D:
        sta(absolute());
        return 4;
    case 0x9D:
        sta(absolute_x());
        return 5;
    case 0x99:
        sta(absolute_y());
        return 5;
    case 0x81:
        sta(indexed_indirect());
        return 6;
    case 0x91:
        sta(indirect_indexed());
        return 6;

    case 0x86:
        stx(zero_page());
        return 3;
    case 0x96:
        stx(zero_page_y());
        return 4;
    case 0x8E:
        stx(absolute());
        return 4;

    case 0x84:
        sty(zero_page());
        return 3;
    case 0x94:
        sty(zero_page_x());
        return 4;
    case 0x8C:
        sty(absolute());
        return 4;

    case 0xAA:
        tax();
        return 2;

    case 0xA8:
        tay();
        return 2;

    case 0xBA:
        tsx();
        return 2;

    case 0x8A:
        txa();
        return 2;

    case 0x9A:
        txs();
        return 2;

    case 0x98:
        tya();
        return 2;

    default:
        printf("bad opcode!\n");
    }
    return 0;
}	

/*************** INSTRUCTIONS ***************/
// Descriptions from: http://www.obelisk.demon.co.uk/6502/reference.html

/**
* @brief This instruction adds the contents of a memory location to the
*	accumulator together with the carry bit. If overflow occurs the carry bit is
*	set, this enables multiple byte addition to be performed.
*
* @param address
*/
void NesCpu::adc(uint16_t address) {
    uint8_t value = memory_->get_byte(address);
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
* @param address
*/
void NesCpu::AND(uint16_t address) {
    uint8_t value = memory_->get_byte(address);
	register_a_ &= value;

	zero_flag_ = register_a_;
	negative_flag_ = register_a_ & 0x80;
}

/**
* @brief This instruction compares the contents of the accumulator with another
*	memory held value and sets the zero and carry flags as appropriate.
*
* @param address
*/
void NesCpu::asl(uint16_t address) { // memory version
    uint8_t value = memory_->get_byte(address);

    carry_flag_ = value & 0x80;
    value <<= 1;

    memory_->set_byte(address, value);        

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

void NesCpu::asl() { // accumulator version
    uint8_t value = register_a_;

    carry_flag_ = value & 0x80;
    value <<= 1;

    register_a_ = value;

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

/**
* @brief If the carry flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t NesCpu::bcc(uint16_t address) {
    if (!carry_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
}

/**
* @brief If the carry flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t NesCpu::bcs(uint16_t address) {
    if (carry_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
}

/**
* @brief If the zero flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t NesCpu::beq(uint16_t address) {
    if (zero_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
}

/**
* @brief This instructions is used to test if one or more bits are set in a
* target memory location. The mask pattern in A is ANDed with the value in
* memory to set or clear the zero flag, but the result is not kept. Bits 7 and 6
* of the value from memory are copied into the N and V flags.
*
* @param address
*/
void NesCpu::bit(uint16_t address) {
    uint8_t result = register_a_ & memory_->get_byte(address);
    
    zero_flag_ = result;
    overflow_flag_ = result & 0x40;
    negative_flag_ = result & 0x80;
}

/**
* @brief If the negative flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t NesCpu::bmi(uint16_t address) {
    if (negative_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
}

/**
* @brief If the zero flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t NesCpu::bne(uint16_t address) {
    if (!zero_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
}

/**
* @brief If the negative flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t NesCpu::bpl(uint16_t address) {
    if (!negative_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
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
* @param address
*
* @return
*/
uint8_t NesCpu::bvc(uint16_t address) {
    if (!overflow_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
}

/**
* @brief If the overflow flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t NesCpu::bvs(uint16_t address) {
    if (overflow_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)memory_->get_byte(address);
        return 1 + abs(old_page - (register_pc_ >> 8));
    }
    return 0;
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
* @param address
*/
void NesCpu::cmp(uint16_t address) {
    uint8_t result = register_a_ - memory_->get_byte(address);
    carry_flag_ = result >= 0;
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief This instruction compares the contents of the X register with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param address
*/
void NesCpu::cpx(uint16_t address) {
    uint8_t result = register_x_ - memory_->get_byte(address);
    carry_flag_ = result >= 0;
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief This instruction compares the contents of the Y register with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param address
*/
void NesCpu::cpy(uint16_t address) {
    uint8_t result = register_y_ - memory_->get_byte(address);
    carry_flag_ = result >= 0;
    zero_flag_ = result;
    negative_flag_ = result & 0x80;
}

/**
* @brief Subtracts one from the value held at a specified memory location
* setting the zero and negative flags as appropriate.
*
* @param address
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
* @param address
*/
void NesCpu::eor(uint16_t address) {
    register_a_ = register_a_ ^ memory_->get_byte(address);
    zero_flag_ = register_a_;
    negative_flag_ = register_a_ & 0x80;
}

/**
* @brief Adds one to the value held at a specified memory location setting the
* zero and negative flags as appropriate.
*
* @param address
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
* @param address
*/
void NesCpu::lda(uint16_t address) {
	register_a_ = memory_->get_byte(address);

	zero_flag_ = register_a_;
	negative_flag_ = register_a_ & 0x80;
}

/**
* @brief Loads a byte of memory into the X register setting the zero and
* negative flags as appropriate.
*
* @param address
*/
void NesCpu::ldx(uint16_t address) {
	register_x_ = memory_->get_byte(address);

	zero_flag_ = register_x_;
	negative_flag_ = register_x_ & 0x80;
}

/**
* @brief Loads a byte of memory into the Y register setting the zero and
* negative flags as appropriate.
*
* @param address
*/
void NesCpu::ldy(uint16_t address) {
	register_y_ = memory_->get_byte(address);

	zero_flag_ = register_y_;
	negative_flag_ = register_y_ & 0x80;
}

/**
* @brief Each of the bits in A or M is shift one place to the right. The bit
* that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
*
* @param address
*/
void NesCpu::lsr(uint16_t address) { // memory version
    uint8_t value = memory_->get_byte(address);

    carry_flag_ = value & 0x01;
    value >>= 1;

    memory_->set_byte(address, value);        

    zero_flag_ = value; // not just register_a_
    negative_flag_ = value & 0x80;
}

void NesCpu::lsr() { // accumulator version
    uint8_t value = register_a_;

    carry_flag_ = value & 0x01;
    value >>= 1;

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
* @param address
*/
void NesCpu::ora(uint16_t address) {
	register_a_ |= memory_->get_byte(address);

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
* @param address
*/
void NesCpu::rol(uint16_t address) { // memory version
    uint8_t value = memory_->get_byte(address);

    uint8_t b_0 = carry_flag_;
    carry_flag_ = value & 0x80;
    value <<= 1;
    value |= b_0;

    memory_->set_byte(address, value);

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

void NesCpu::rol() { // accumulator version
    uint8_t value = register_a_;

    uint8_t b_0 = carry_flag_;
    carry_flag_ = value & 0x80;
    value <<= 1;
    value |= b_0;

    register_a_ = value;

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

/**
* @brief Move each of the bits in either A or M one place to the right. Bit 7 is
* filled with the current value of the carry flag whilst the old bit 0 becomes
* the new carry flag value.
*
* @param address
*/
void NesCpu::ror(uint16_t address) {
    uint8_t value = memory_->get_byte(address);

    uint8_t b_7 = carry_flag_;
    carry_flag_ = value & 0x01;
    value >>= 1;
    value |= (b_7 << 7);

    memory_->set_byte(address, value);        

    zero_flag_ = register_a_; // not value
    negative_flag_ = value & 0x80;
}

void NesCpu::ror() { // accumulator version
    uint8_t value = register_a_;

    uint8_t b_7 = carry_flag_;
    carry_flag_ = value & 0x01;
    value >>= 1;
    value |= (b_7 << 7);

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
* @param address
*/
void NesCpu::sbc(uint16_t address) {
    uint8_t value = memory_->get_byte(address);
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
* @return 
*/
uint16_t NesCpu::immediate() {
    return register_pc_++;
}
		
/**
* @brief An instruction using zero page addressing mode has only an 8 bit
*	address operand. This limits it to addressing only the first 256 bytes of
*	memory (e.g. $0000 to $00FF) where the most significant byte of the address is
*	always zero. In zero page mode only the least significant byte of the address
*	is held in the instruction making it shorter by one byte (important for space
*	saving) and one less memory fetch during execution (important for speed).
*
* @return 
*/
uint16_t NesCpu::zero_page() {
	return memory_->get_byte(register_pc_++);
}
		
/**
* @brief The address to be accessed by an instruction using indexed zero page
*	addressing is calculated by taking the 8 bit zero page address from the
*	instruction and adding the current value of the X register to it. For example
*	if the X register contains $0F and the instruction LDA $80,X is executed then
*	the accumulator will be loaded from $008F (e.g. $80 + $0F => $8F).
*
* @return 
*/
uint16_t NesCpu::zero_page_x() {
	return (memory_->get_byte(register_pc_++) + register_x_) % 0x100;
}
		
/**
* @brief The address to be accessed by an instruction using indexed zero page
*	addressing is calculated by taking the 8 bit zero page address from the
*	instruction and adding the current value of the Y register to it. This mode can
*	only be used with the LDX and STX instructions.
*
* @return 
*/
uint16_t NesCpu::zero_page_y() {
	return (memory_->get_byte(register_pc_++) + register_y_) % 0x100;
}

/**
* @brief Relative addressing mode is used by branch instructions (e.g. BEQ, BNE,
* etc.) which contain a signed 8 bit relative offset (e.g. -128 to +127) which
* is added to program counter if the condition is true. As the program counter
* itself is incremented during instruction execution by two the effective
* address range for the target instruction must be with -126 to +129 bytes of
* the branch.
*
* @return
*/
uint16_t NesCpu::relative() {
    return register_pc_++;
}
		
/**
* @brief Instructions using absolute addressing contain a full 16 bit address
*	to identify the target location.
*
* @return 
*/
uint16_t NesCpu::absolute() {
	uint16_t address = memory_->get_word(register_pc_);
    register_pc_ += 2;
    return address;
}
		
/**
* @brief The address to be accessed by an instruction using X register indexed
*	absolute addressing is computed by taking the 16 bit address from the
*	instruction and added the contents of the X register. For example if X contains
*	$92 then an STA $2000,X instruction will store the accumulator at $2092 (e.g.
*	$2000 + $92).
*
* @return 
*/
uint16_t NesCpu::absolute_x() {
    uint16_t address = memory_->get_word(register_pc_) + register_x_;
    register_pc_ += 2;
    return address;
}
		
/**
* @brief The Y register indexed absolute addressing mode is the same as the
*	previous mode only with the contents of the Y register added to the 16 bit
*	address from the instruction.
*
* @return 
*/
uint16_t NesCpu::absolute_y() {
    uint16_t address = memory_->get_word(register_pc_) + register_y_;
    register_pc_ += 2;
    return address;
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
* @return 
*/
uint16_t NesCpu::indirect() {
    uint16_t address = memory_->get_word(memory_->get_word(register_pc_));
    register_pc_ += 2;
    return address;
}
		
/**
* @brief Indexed indirect addressing is normally used in conjunction with a
*	table of address held on zero page. The address of the table is taken from the
*	instruction and the X register added to it (with zero page wrap around) to give
*	the location of the least significant byte of the target address.
*
* @return 
*/
uint16_t NesCpu::indexed_indirect() {
    return memory_->get_word((memory_->get_byte(register_pc_++) +
                              register_x_) % 0x100);
}
		
/**
* @brief Indirect indirect addressing is the most common indirection mode used
*	on the 6502. In instruction contains the zero page location of the least
*	significant byte of 16 bit address. The Y register is dynamically added to this
*	value to generated the actual target address for operation.
*
* @return 
*/
uint16_t NesCpu::indirect_indexed() {
    return memory_->get_word(memory_->get_byte(register_pc_++)) +
        register_y_;
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
