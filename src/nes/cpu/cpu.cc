#include "cpu.h"

Cpu::Cpu(Nes *nes, int clock_speed) : nes_(nes), clock_speed_(clock_speed) {}

/**
* @brief loop through all instructions, executing them in sequence.
* Returns when it encounters a STP instruction.
*
* @return total number of cycles run.
*/
int Cpu::Run() {
    uint32_t total_cycles = 0;
    uint8_t cycles;
    for(;;) {
        uint8_t opcode = nes_->GetByte(register_pc_++);
        if (opcode == 0xF2) {
            #ifdef DEBUG
            instr_pc_ = register_pc_-1;
            opcode_ = opcode;
            instr_ = "(HLT)";
            AMImplied();
            if(PrintState())
                return -1;
            #endif
            break;  // STP
        }
        cycles = RunInstruction(opcode);
        RanCycles(cycles);
        total_cycles += cycles;
        #ifdef DEBUG
        if(PrintState())
            return -1;
        #endif

    }

    #ifdef DEBUG
    PrintHeader();
    #endif

    return total_cycles;
}

/**
* @brief runs an instruction given an opcode and returns the number of 6502
* cycles it took to run that instruction.
*
* @param opcode
*
* @return number of cycles to run instruction
*/
uint8_t Cpu::RunInstruction(uint8_t opcode) {
    opcode_ = opcode;

    #ifdef DEBUG
    instr_pc_ = register_pc_ - 1;
    #endif

    int i = int(opcode);

    uint16_t addr = (this->*opcodes[i].am)();
    uint8_t extra_cycles = (this->*opcodes[i].op)(addr);
    uint8_t cycles = opcodes[i].cycles;
    bool cross_page = (addr % 0x100 == 0xFF);
    if (cross_page) 
        cycles += opcodes[i].penalty;
    return cycles + extra_cycles;
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
uint8_t Cpu::ADC(uint16_t address) {    
    uint8_t value = nes_->GetByte(address);
	uint8_t original_a = register_a_;
    register_a_= register_a_ + value + carry_flag_;

    // set if sign of result is different than the sign of both
    // the inputs.
	overflow_flag_ = (original_a ^ register_a_) & (value ^ register_a_) & 0x80;

    uint16_t resultu16 = original_a + value + carry_flag_;
	carry_flag_ = resultu16 > 0xFF;
	zero_flag_ = register_a_ == 0;
	negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "ADC";
    #endif

    return 0;
}
		
/**
* @brief A logical AND is performed, bit by bit, on the accumulator contents
*	using the contents of a byte of memory.
*
* @param address
*/
uint8_t Cpu::AND(uint16_t address) {    
    uint8_t value = nes_->GetByte(address);
	register_a_ &= value;

	zero_flag_ = register_a_ == 0;
	negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "AND";
    #endif

    return 0;
}

/**
* @brief This operation shifts all the bits of the accumulator or memory
* contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the carry
* flag. The effect of this operation is to multiply the memory contents by 2
* (ignoring 2's complement considerations), setting the carry if the result will
* not fit in 8 bits.
*
* @param address
*/
uint8_t Cpu::ASL(uint16_t address) {
    uint8_t value;
    if (opcode_ == 0x0A)
        value = register_a_;
    else
        value = nes_->GetByte(address);

    carry_flag_ = value & 0x80;
    value <<= 1;

    if (opcode_ == 0x0A)
        register_a_ = value;
    else
        nes_->SetByte(address, value);        

    zero_flag_ = value == 0;
    negative_flag_ = value & 0x80;

    #ifdef DEBUG
    instr_ = "ASL";
    if (opcode_ != 0x0A) {
        int i = strlen(context_) - 3;
        sprintf(&context_[i], "%02X]", value);
    }
    #endif

    return 0;
}

/**
* @brief If the carry flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BCC(uint16_t address) {
    uint8_t rc = 0;
    if (!carry_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BCC";
    #endif
    
    return rc;
}

/**
* @brief If the carry flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BCS(uint16_t address) {
    uint8_t rc = 0;
    if (carry_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BCS";
    #endif

    return rc;
}

/**
* @brief If the zero flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BEQ(uint16_t address) {
    uint8_t rc = 0;
    if (zero_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BEQ";
    #endif
    
    return rc;
}

/**
* @brief This instructions is used to test if one or more bits are set in a
* target memory location. The mask pattern in A is ANDed with the value in
* memory to set or clear the zero flag, but the result is not kept. Bits 7 and 6
* of the value from memory are copied into the N and V flags.
*
* @param address
*/
uint8_t Cpu::BIT(uint16_t address) {
    uint8_t mem = nes_->GetByte(address);
    uint8_t result = register_a_ & mem;
    
    zero_flag_ = result == 0;
    overflow_flag_ = mem & 0x40;
    negative_flag_ = mem & 0x80;

    #ifdef DEBUG
    instr_ = "BIT";
    #endif

    return 0;
}

/**
* @brief If the negative flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BMI(uint16_t address) {
    uint8_t rc = 0;
    if (negative_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BMI";
    #endif
    
    return rc;
}

/**
* @brief If the zero flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BNE(uint16_t address) {
    uint8_t rc = 0;
    if (!zero_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BNE";
    #endif
    
    return rc;
}

/**
* @brief If the negative flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BPL(uint16_t address) {
    uint8_t rc = 0;
    if (!negative_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BPL";
    #endif

    return rc;
}

/**
* @brief The BRK instruction forces the generation of an interrupt request. The
* program counter and processor status are pushed on the stack then the IRQ
* interrupt vector at $FFFE/F is loaded into the PC and the break flag in the
* status set to one.
*/
uint8_t Cpu::BRK(uint16_t address) {
    register_pc_++; // brk increments the pc first
    PushToStack(register_pc_ >> 8);
    PushToStack(register_pc_ & 0xFF);
    
    PushToStack(GetProcessorStatus());

    register_pc_ = nes_->GetWord(0xFFFE);

    break_command_ = 1;

    #ifdef DEBUG
    instr_ = "BRK";
    #endif

    return 0;
}

/**
* @brief If the overflow flag is clear then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BVC(uint16_t address) {
    uint8_t rc = 0;
    if (!overflow_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BVC";
    #endif
    
    return rc;
}

/**
* @brief If the overflow flag is set then add the relative displacement to the
* program counter to cause a branch to a new location.
*
* @param address
*
* @return
*/
uint8_t Cpu::BVS(uint16_t address) {
    uint8_t rc = 0;
    if (overflow_flag_) {
        uint8_t old_page = register_pc_ >> 8;
        register_pc_ += (int8_t)nes_->GetByte(address);
        rc = 1 + abs(old_page - (register_pc_ >> 8));
    }

    #ifdef DEBUG
    instr_ = "BVS";
    #endif

    return rc;
}

/**
* @brief Set the carry flag to zero.
*/
uint8_t Cpu::CLC(uint16_t address) {
    carry_flag_ = 0;

    #ifdef DEBUG
    instr_ = "CLC";
    #endif

    return 0;
}

/**
* @brief Sets the decimal mode flag to zero.
*/
uint8_t Cpu::CLD(uint16_t address) {
    decimal_mode_flag_ = 0;

    #ifdef DEBUG
    instr_ = "CLD";
    #endif

    return 0;
}

/**
* @brief Clears the interrupt disable flag allowing normal interrupt requests to
* be serviced.
*/
uint8_t Cpu::CLI(uint16_t address) {
    interrupt_disable_ = 0;

    #ifdef DEBUG
    instr_ = "CLI";
    #endif

    return 0;
}

/**
* @brief Clears the overflow flag.
*/
uint8_t Cpu::CLV(uint16_t address) {
    overflow_flag_ = 0;

    #ifdef DEBUG
    instr_ = "CLV";
    #endif

    return 0;
}

/**
* @brief This instruction compares the contents of the accumulator with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param address
*/
uint8_t Cpu::CMP(uint16_t address) {
    uint8_t value = nes_->GetByte(address);
    uint8_t result = register_a_ - value;
    carry_flag_ = register_a_ >= value;
    zero_flag_ = register_a_ == value;
    negative_flag_ = result  & 0x80;

    #ifdef DEBUG
    instr_ = "CMP";
    #endif

    return 0;
}

/**
* @brief This instruction compares the contents of the X register with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param address
*/
uint8_t Cpu::CPX(uint16_t address) {
    uint8_t value = nes_->GetByte(address);
    uint8_t result = register_x_ - value;
    carry_flag_ = register_x_ >= value;
    zero_flag_ = register_x_ == value;
    negative_flag_ = result & 0x80;

    #ifdef DEBUG
    instr_ = "CPX";
    #endif

    return 0;
}

/**
* @brief This instruction compares the contents of the Y register with another
* memory held value and sets the zero and carry flags as appropriate.
*
* @param address
*/
uint8_t Cpu::CPY(uint16_t address) {
    uint8_t value = nes_->GetByte(address);
    uint8_t result = register_y_ - value;
    carry_flag_ = register_y_ >= value;
    zero_flag_ = register_y_ == value;
    negative_flag_ = result & 0x80;

    #ifdef DEBUG
    instr_ = "CPY";
    #endif

    return 0;
}

/**
* @brief Subtracts one from the value held at a specified memory location
* setting the zero and negative flags as appropriate.
*
* @param address
*/
uint8_t Cpu::DEC(uint16_t address) {
    uint8_t result = nes_->GetByte(address) - 1;
    nes_->SetByte(address, result);
    zero_flag_ = result == 0;
    negative_flag_ = result & 0x80;

    #ifdef DEBUG
    instr_ = "DEC";
    int i = strlen(context_) - 3;
    sprintf(&context_[i], "%02X]", result);
    #endif

    return 0;
}

/**
* @brief Subtracts one from the X register setting the zero and negative flags
* as appropriate.
*/
uint8_t Cpu::DEX(uint16_t address) {
    register_x_--;
    zero_flag_ = register_x_ == 0;
    negative_flag_ = register_x_ & 0x80;

    #ifdef DEBUG
    instr_ = "DEX";
    #endif

    return 0;
}

/**
* @brief Subtracts one from the Y register setting the zero and negative flags
* as appropriate.
*/
uint8_t Cpu::DEY(uint16_t address) {
    register_y_--;
    zero_flag_ = register_y_ == 0;
    negative_flag_ = register_y_ & 0x80;

    #ifdef DEBUG
    instr_ = "DEY";
    #endif

    return 0;
}

/**
* @brief An exclusive OR is performed, bit by bit, on the accumulator contents
* using the contents of a byte of memory.
*
* @param address
*/
uint8_t Cpu::EOR(uint16_t address) {
    register_a_ = register_a_ ^ nes_->GetByte(address);
    zero_flag_ = register_a_ == 0;
    negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "EOR";
    #endif

    return 0;
}

/**
* @brief Adds one to the value held at a specified memory location setting the
* zero and negative flags as appropriate.
*
* @param address
*/
uint8_t Cpu::INC(uint16_t address) {
    uint8_t result = nes_->GetByte(address) + 1;
    nes_->SetByte(address, result);
    zero_flag_ = result == 0;
    negative_flag_ = result & 0x80;

    #ifdef DEBUG
    instr_ = "INC";
    int i = strlen(context_) - 3;
    sprintf(&context_[i], "%02X]", result);
    #endif

    return 0;
}

/**
* @brief Adds one to the X register setting the zero and negative flags as
* appropriate.
*/
uint8_t Cpu::INX(uint16_t address) {
    register_x_++;
    zero_flag_ = register_x_ == 0;
    negative_flag_ = register_x_ & 0x80;

    #ifdef DEBUG
    instr_ = "INX";
    #endif

    return 0;
}

/**
* @brief Adds one to the Y register setting the zero and negative flags as
* appropriate.
*/
uint8_t Cpu::INY(uint16_t address) {
    register_y_++;
    zero_flag_ = register_y_ == 0;
    negative_flag_ = register_y_ & 0x80;

    #ifdef DEBUG
    instr_ = "INY";
    #endif

    return 0;
}

/**
* @brief Sets the program counter to the address specified by the operand.
*
* @param address
*/
uint8_t Cpu::JMP(uint16_t address) {
    register_pc_ = address;

    #ifdef DEBUG
    instr_ = "JMP";
    if(opcode_ == 0x4C) {
        int i = strlen(context_) - 3;
        sprintf(&context_[i], "00]");
    }
    #endif

    return 0;
}

/**
* @brief The JSR instruction pushes the address (minus one) of the return point
* on to the stack and then sets the program counter to the target memory
* address.
*
* @param address
*/
uint8_t Cpu::JSR(uint16_t address) {
    PushToStack((register_pc_ - 1) >> 8);
    PushToStack((register_pc_ - 1) & 0xFF);

    register_pc_ = address;

    #ifdef DEBUG
    instr_ = "JSR";
    int i = strlen(context_) - 3;
    sprintf(&context_[i], "00]");
    #endif

    return 0;
}

/**
* @brief Loads a byte of memory into the accumulator setting the zero and
*	negative flags as appropriate.
*
* @param address
*/
uint8_t Cpu::LDA(uint16_t address) {
	register_a_ = nes_->GetByte(address);

	zero_flag_ = register_a_ == 0;
	negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "LDA";
    #endif

    return 0;
}

/**
* @brief Loads a byte of memory into the X register setting the zero and
* negative flags as appropriate.
*
* @param address
*/
uint8_t Cpu::LDX(uint16_t address) {
	register_x_ = nes_->GetByte(address);

	zero_flag_ = register_x_ == 0;
	negative_flag_ = register_x_ & 0x80;

    #ifdef DEBUG
    instr_ = "LDX";
    #endif

    return 0;
}

/**
* @brief Loads a byte of memory into the Y register setting the zero and
* negative flags as appropriate.
*
* @param address
*/
uint8_t Cpu::LDY(uint16_t address) {
	register_y_ = nes_->GetByte(address);

	zero_flag_ = register_y_ == 0;
	negative_flag_ = register_y_ & 0x80;

    #ifdef DEBUG
    instr_ = "LDY";
    #endif

    return 0;
}

/**
* @brief Each of the bits in A or M is shift one place to the right. The bit
* that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
*
* @param address
*/
uint8_t Cpu::LSR(uint16_t address) {
    uint8_t value;
    if (opcode_ == 0x4A)
        value = register_a_;
    else
        value = nes_->GetByte(address);

    carry_flag_ = value & 0x01;
    value >>= 1;

    if (opcode_ == 0x4A)
        register_a_ = value;
    else
        nes_->SetByte(address, value);        

    zero_flag_ = value == 0; // not just register_a_
    negative_flag_ = value & 0x80;

    #ifdef DEBUG
    instr_ = "LSR";
    if (opcode_ != 0x4A) {
        int i = strlen(context_) - 3;
        sprintf(&context_[i], "%02X]", value);
    }
    #endif

    return 0;
}


/**
* @brief The NOP instruction causes no changes to the processor other than the
* normal incrementing of the program counter to the next instruction.
*/
uint8_t Cpu::NOP(uint16_t address) {
    #ifdef DEBUG
    instr_ = "NOP";
    #endif

    return 0;
}


/**
* @brief An inclusive OR is performed, bit by bit, on the accumulator contents
* using the contents of a byte of memory.
*
* @param address
*/
uint8_t Cpu::ORA(uint16_t address) {
	register_a_ |= nes_->GetByte(address);

	zero_flag_ = register_a_ == 0;
	negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "ORA";
    #endif

    return 0;
}

/**
* @brief Pushes a copy of the accumulator on to the stack.
*/
uint8_t Cpu::PHA(uint16_t address) {
    PushToStack(register_a_);

    #ifdef DEBUG
    instr_ = "PHA";
    #endif

    return 0;
}

/**
* @brief Pushes a copy of the status flags on to the stack.
*/
uint8_t Cpu::PHP(uint16_t address) {
    PushToStack(GetProcessorStatus());

    #ifdef DEBUG
    instr_ = "PHP";
    #endif

    return 0;
}

/**
* @brief Pulls an 8 bit value from the stack and into the accumulator. The zero
* and negative flags are set as appropriate.
*/
uint8_t Cpu::PLA(uint16_t address) {
    register_a_ = PopFromStack();
    
    zero_flag_ = register_a_ == 0;
    negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "PLA";
    #endif

    return 0;
}

/**
* @brief Pulls an 8 bit value from the stack and into the processor flags. The
* flags will take on new states as determined by the value pulled.
*/
uint8_t Cpu::PLP(uint16_t address) {
    SetProcessorStatus(PopFromStack());

    #ifdef DEBUG
    instr_ = "PLP";
    #endif

    return 0;
}

/**
* @brief Move each of the bits in either A or M one place to the left. Bit 0 is
* filled with the current value of the carry flag whilst the old bit 7 becomes
* the new carry flag value.
*
* @param address
*/
uint8_t Cpu::ROL(uint16_t address) {
    uint8_t value;
    if (opcode_ == 0x2A)
        value = register_a_;
    else
        value = nes_->GetByte(address);

    uint8_t b_0 = carry_flag_;
    carry_flag_ = value & 0x80;
    value <<= 1;
    value |= b_0;

    if (opcode_ == 0x2A)
        register_a_ = value;
    else
        nes_->SetByte(address, value);

    zero_flag_ = value == 0;
    negative_flag_ = value & 0x80;

    #ifdef DEBUG
    instr_ = "ROL";
    if (opcode_ != 0x2A) {
        int i = strlen(context_) - 3;
        sprintf(&context_[i], "%02X]", value);
    }
    #endif

    return 0;
}

/**
* @brief Move each of the bits in either A or M one place to the right. Bit 7 is
* filled with the current value of the carry flag whilst the old bit 0 becomes
* the new carry flag value.
*
* @param address
*/
uint8_t Cpu::ROR(uint16_t address) {
    uint8_t value;
    if (opcode_ == 0x6A)
        value = register_a_;
    else
        value = nes_->GetByte(address);

    uint8_t b_7 = carry_flag_;
    carry_flag_ = value & 0x01;
    value >>= 1;
    value |= (b_7 << 7);

    if (opcode_ == 0x6A)
        register_a_ = value;
    else
        nes_->SetByte(address, value);        

    zero_flag_ = register_a_ == 0; // not value
    negative_flag_ = value & 0x80;

    #ifdef DEBUG
    instr_ = "ROR";
    if (opcode_ != 0x6A) {
        int i = strlen(context_) - 3;
        sprintf(&context_[i], "%02X]", value);
    }
    #endif

    return 0;
}

/**
* @brief The RTI instruction is used at the end of an interrupt processing
* routine. It pulls the processor flags from the stack followed by the program
* counter.
*/
uint8_t Cpu::RTI(uint16_t address) {
    SetProcessorStatus(PopFromStack());
    register_pc_ = PopFromStack();
    register_pc_ |= (PopFromStack() << 8);

    #ifdef DEBUG
    instr_ = "RTI";
    #endif

    return 0;
}

/**
* @brief The RTS instruction is used at the end of a subroutine to return to the
* calling routine. It pulls the program counter (minus one) from the stack.
*/
uint8_t Cpu::RTS(uint16_t address) {
    register_pc_ = PopFromStack();
    register_pc_ |= (PopFromStack() << 8);
    register_pc_++;

    #ifdef DEBUG
    instr_ = "RTS";
    #endif

    return 0;
}

/**
* @brief This instruction subtracts the contents of a memory location to the
* accumulator together with the not of the carry bit. If overflow occurs the
* carry bit is clear, this enables multiple byte subtraction to be performed.
*
* @param address
*/
uint8_t Cpu::SBC(uint16_t address) {
    uint8_t value = nes_->GetByte(address);
	uint8_t original_a = register_a_;
    register_a_ = register_a_ - value - (1 - carry_flag_);

    int16_t result16 = (int8_t)original_a - (int8_t)value - (1 - carry_flag_);
    overflow_flag_ = result16 > 127 || result16 < -128;

    carry_flag_ = original_a >= value + (1 - carry_flag_);
	zero_flag_ = register_a_ == 0;
	negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "SBC";
    #endif

    return 0;
}

/**
* @brief Set the carry flag to one.
*/
uint8_t Cpu::SEC(uint16_t address) {
    carry_flag_ = 1;

    #ifdef DEBUG
    instr_ = "SEC";
    #endif

    return 0;
}

/**
* @brief Set the decimal mode flag to one.
*/
uint8_t Cpu::SED(uint16_t address) {
    decimal_mode_flag_ = 1;

    #ifdef DEBUG
    instr_ = "SED";
    #endif

    return 0;
}

/**
* @brief Set the interrupt disable flag to one.
*/
uint8_t Cpu::SEI(uint16_t address) {
    interrupt_disable_ = 1;

    #ifdef DEBUG
    instr_ = "SEI";
    #endif

    return 0;
}

/**
* @brief Stores the contents of the accumulator into memory.
*
* @param address
*/
uint8_t Cpu::STA(uint16_t address) {
    nes_->SetByte(address, register_a_);

    #ifdef DEBUG
    instr_ = "STA";
    int i = strlen(context_) - 3;
    sprintf(&context_[i], "%02X]", register_a_);
    #endif

    return 0;
}

/**
* @brief Stores the contents of the X register into memory.
*
* @param address
*/
uint8_t Cpu::STX(uint16_t address) {
    nes_->SetByte(address, register_x_);

    #ifdef DEBUG
    instr_ = "STX";
    int i = strlen(context_) - 3;
    sprintf(&context_[i], "%02X]", register_x_);
    #endif

    return 0;
}

/**
* @brief Stores the contents of the Y register into memory.
*
* @param address
*/
uint8_t Cpu::STY(uint16_t address) {
    nes_->SetByte(address, register_y_);

    #ifdef DEBUG
    instr_ = "STY";
    int i = strlen(context_) - 3;
    sprintf(&context_[i], "%02X]", register_y_);
    #endif

    return 0;
}

/**
* @brief Copies the current contents of the accumulator into the X register and
* sets the zero and negative flags as appropriate.
*/
uint8_t Cpu::TAX(uint16_t address) {
    register_x_ = register_a_;

    zero_flag_ = register_x_ == 0;
    negative_flag_ = register_x_ & 0x80;

    #ifdef DEBUG
    instr_ = "TAX";
    #endif

    return 0;
}

/**
* @brief Copies the current contents of the accumulator into the Y register and
* sets the zero and negative flags as appropriate.
*/
uint8_t Cpu::TAY(uint16_t address) {
    register_y_ = register_a_;

    zero_flag_ = register_y_ == 0;
    negative_flag_ = register_y_ & 0x80;

    #ifdef DEBUG
    instr_ = "TAY";
    #endif

    return 0;
}

/**
* @brief Copies the current contents of the stack register into the X register
* and sets the zero and negative flags as appropriate.
*/
uint8_t Cpu::TSX(uint16_t address) {
    register_x_ = register_s_;

    zero_flag_ = register_x_ == 0;
    negative_flag_ = register_x_ & 0x80;

    #ifdef DEBUG
    instr_ = "TSX";
    #endif

    return 0;
}

/**
* @brief Copies the current contents of the X register into the accumulator and
* sets the zero and negative flags as appropriate.
*/
uint8_t Cpu::TXA(uint16_t address) {
    register_a_ = register_x_;

    zero_flag_ = register_a_ == 0;
    negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "TXA";
    #endif

    return 0;
}

/**
* @brief Copies the current contents of the X register into the stack register.
*/
uint8_t Cpu::TXS(uint16_t address) {
    register_s_ = register_x_;

    #ifdef DEBUG
    instr_ = "TXS";
    #endif

    return 0;
}

/**
* @brief Copies the current contents of the Y register into the accumulator and
* sets the zero and negative flags as appropriate.
*/
uint8_t Cpu::TYA(uint16_t address) {
    register_a_ = register_y_;

    zero_flag_ = register_a_ == 0;
    negative_flag_ = register_a_ & 0x80;

    #ifdef DEBUG
    instr_ = "TYA";
    #endif

    return 0;
}


/*************** HELPER FUNCTIONS ***************/

/**
* @brief pushes the input byte onto the stack and adjusts the stack pointer.
*
* @param value
*/
void Cpu::PushToStack(uint8_t value) {
    nes_->SetByte(0x100 + register_s_, value);
    register_s_--;
}

/**
* @brief pops a byte off the stack and returns it, adjusting the stack pointer.
*
* @return 
*/
uint8_t Cpu::PopFromStack() {		
    register_s_++;
    uint8_t result = nes_->GetByte(0x100 + register_s_);
    return result;
}

// Descriptions from: http://nesdev.com/6502.txt
/**
* @brief sets the processors status flags based on the bits in the input.
*
* @param value
*/
void Cpu::SetProcessorStatus(uint8_t value) {
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
uint8_t Cpu::GetProcessorStatus() {
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

uint16_t Cpu::AMImplied() {
    #ifdef DEBUG
    sprintf(context_, "                ");
    #endif
    return 0;
}

/**
* @brief Some instructions have an option to operate directly upon the
* accumulator. The programmer specifies this by using a special operand value,
* 'A'.
*
* @return 
*/
uint16_t Cpu::AMAccumulator() {
    #ifdef DEBUG
    sprintf(context_, "A               ");
    #endif
    return 0;
}


/**
* @brief Immediate addressing allows the programmer to directly specify an 8
*	bit constant within the instruction. It is indicated by a '#' symbol followed
*	by an numeric expression.
*
* @return 
*/
uint16_t Cpu::AMImmediate() {
    #ifdef DEBUG
    sprintf(context_, "#%02X             ", nes_->GetByte(register_pc_));
    #endif
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
uint16_t Cpu::AMZeroPage() {
    #ifdef DEBUG
    uint8_t addr = nes_->GetByte(register_pc_);
    uint8_t value = nes_->GetByte(addr);
    sprintf(context_, "%02X          [%02X]", addr, value);
    #endif
	return nes_->GetByte(register_pc_++);
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
uint16_t Cpu::AMZeroPageX() {
    #ifdef DEBUG
    uint8_t addr = nes_->GetByte(register_pc_);
    uint8_t value = nes_->GetByte((addr + register_x_) % 0x100);
    sprintf(context_, "%02X,X   [00%02X=%02X]", addr,
        (addr+register_x_) % 0x100, value);
    #endif
	return (nes_->GetByte(register_pc_++) + register_x_) % 0x100;
}
		
/**
* @brief The address to be accessed by an instruction using indexed zero page
*	addressing is calculated by taking the 8 bit zero page address from the
*	instruction and adding the current value of the Y register to it. This mode can
*	only be used with the LDX and STX instructions.
*
* @return 
*/
uint16_t Cpu::AMZeroPageY() {
    #ifdef DEBUG
    uint8_t addr = nes_->GetByte(register_pc_);
    uint8_t value = nes_->GetByte((addr + register_y_) % 0x100);
    sprintf(context_, "%02X,Y   [00%02X=%02X]", addr,
        (addr+register_x_) % 0x100, value);
    #endif
	return (nes_->GetByte(register_pc_++) + register_y_) % 0x100;
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
uint16_t Cpu::AMRelative() {
    #ifdef DEBUG
    int8_t addr = nes_->GetByte(register_pc_);
    sprintf(context_, "%02X        [%04X]", (uint8_t)addr, addr+register_pc_+1);
    #endif
    return register_pc_++;
}
		
/**
* @brief Instructions using absolute addressing contain a full 16 bit address
*	to identify the target location.
*
* @return 
*/
uint16_t Cpu::AMAbsolute() {
    #ifdef DEBUG
    uint16_t addr = nes_->GetWord(register_pc_);
    uint8_t value = nes_->GetByte(addr);
    sprintf(context_, "%04X        [%02X]", addr, value);
    #endif
	uint16_t address = nes_->GetWord(register_pc_);
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
uint16_t Cpu::AMAbsoluteX() {
    #ifdef DEBUG
    uint16_t addr = nes_->GetWord(register_pc_);
    uint8_t value = nes_->GetByte(addr+register_x_);
    sprintf(context_, "%04X,X [%04X=%02X]", addr, addr+register_x_,
        value);
    #endif
    uint16_t address = nes_->GetWord(register_pc_) + register_x_;
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
uint16_t Cpu::AMAbsoluteY() {
    #ifdef DEBUG
    uint16_t addr = nes_->GetWord(register_pc_);
    uint8_t value = nes_->GetByte(addr+register_y_);
    sprintf(context_, "%04X,Y [%04X=%02X]", addr, addr+register_y_,
        value);
    #endif
    uint16_t address = nes_->GetWord(register_pc_) + register_y_;
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
uint16_t Cpu::AMIndirect() {
    #ifdef DEBUG
    uint16_t addr = nes_->GetWord(register_pc_);
    uint16_t value = nes_->GetWord(addr);
    sprintf(context_, "(%04X)    [%04X]", addr, value);
    #endif
    uint16_t address = nes_->GetWord(nes_->GetWord(register_pc_));
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
uint16_t Cpu::AMIndexedIndirect() {
    #ifdef DEBUG
    uint8_t addr = nes_->GetByte(register_pc_);
    uint8_t value = nes_->GetByte(nes_->GetWord((addr+register_x_) % 0x100));
    sprintf(context_, "(%02X,X) [%04X=%02X]", addr,
            nes_->GetWord((addr+register_x_) % 0x100), value);
    #endif
    return nes_->GetWord((nes_->GetByte(register_pc_++) +
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
uint16_t Cpu::AMIndirectIndexed() {
    #ifdef DEBUG
    uint8_t addr = nes_->GetByte(register_pc_);
    uint8_t value = nes_->GetByte(nes_->GetWord(addr) +
                                      register_y_);
    sprintf(context_, "(%02X),Y [%04X=%02X]", addr,
            nes_->GetWord(addr) + register_y_, value);
    #endif
    return nes_->GetWord(nes_->GetByte(register_pc_++)) +
        register_y_;
}		

		
/*************** LOGGING ***************/
void Cpu::PrintHeader() {
    //    if (call_number_ <= 30) {
        printf("\n\n\n");
        printf("%s\n", file_name_.c_str());
        printf("Track Number %d, Call number %d\n\n", track_, call_number_);
        printf("PC     Instr.      Context            A  X  Y  Status    SP\n");
        printf("===========================================================\n");
        //}
    call_number_++;
}

void Cpu::SetLogging(std::string file_name, int track) {
    file_name_ = file_name;
    track_ = track;

    #ifdef DEBUG
    PrintHeader();
    #endif
}

void Cpu::SetLogChecking(std::string correct_log) {
    correct_log_ = fopen(correct_log.c_str(), "r");
    size_t buffer_size = 1024;
    char *throw_away = (char *)malloc(buffer_size * sizeof(char));
    for(int i = 0; i < 8; i++) {
        getline(&throw_away, &buffer_size, correct_log_);
    }
    free(throw_away);
}

int Cpu::PrintState() {
    int invalid = 0;
    std::string extra_space;
    extra_space = (instr_.length() == 3) ? " " : "";

    std::string flags;
    flags += negative_flag_ ? "N" : ".";
    flags += overflow_flag_ ? "V" : ".";
    flags += interrupt_disable_ ? "I" : ".";
    flags += zero_flag_ ? "Z" : ".";
    flags += carry_flag_ ? "C" : ".";

    size_t state_buffer_size = 1024;
    char *current_state = (char *)malloc(state_buffer_size * sizeof(char));
    char *correct_state = (char *)malloc(state_buffer_size * sizeof(char));

    sprintf(current_state, "%04X   %02X   %s%s%s  %s  %02X %02X %02X  [%s]   %02X\n",
            instr_pc_, opcode_, extra_space.c_str(),
            instr_.c_str(), extra_space.c_str(),
            context_, register_a_,
            register_x_, register_y_,
            flags.c_str(), register_s_);

    // if (call_number_ <= 30) {
    //     printf("%s", current_state);
    // }

    if (correct_log_) {
        ssize_t bytes = getline(&correct_state, &state_buffer_size, correct_log_);
        if (bytes == 1) {
            for(int i = 0; i < 8; i++) {
                getline(&correct_state, &state_buffer_size, correct_log_);
            }
        }
        if (bytes > 0) {
            if (strcmp(current_state, correct_state)) {
                printf("error!\n");
                printf("current: %scorrect: %s", current_state, correct_state);
                printf("bytes: %zu\n", bytes);
                invalid = 1;
            }
        }
        // if (bytes < 0) {
        //     printf("error\n");
        // }
    }

    free(current_state);
    free(correct_state);
    return invalid;
}
