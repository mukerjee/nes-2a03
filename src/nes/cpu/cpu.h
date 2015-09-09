#ifndef NES_CPU_CPU_H_
#define NES_CPU_CPU_H_

#include <unistd.h>
#include <ctime>
#include <iostream>

#include "gallant_signal.h"

#include "nes.h"

//#define DEBUG

#define NUM_OPCODES 256

class Nes;

class Cpu {
    friend class Nes;
 public:
    Cpu(Nes *nes, int clock_speed);
    int Run();
    Gallant::Signal1<int> RanCycles;

 private:
    Nes *nes_;
    int clock_speed_;
        
    /*************** REGISTERS ***************/
    uint8_t register_a_ = 0x00;
    uint8_t register_x_ = 0x00;
    uint8_t register_y_ = 0x00;
    uint8_t register_s_ = 0xFD;
    uint16_t register_pc_ = 0xFFFC;  // TODO: implement reset vector

    bool carry_flag_ = 0;
    bool zero_flag_ = 0;
    bool interrupt_disable_ = 1;
    bool decimal_mode_flag_ = 0;
    bool break_command_ = 1;
    bool overflow_flag_ = 0;
    bool negative_flag_ = 0;

    /*************** EXECUTION ***************/
    uint8_t opcode_;
    uint8_t RunInstruction(uint8_t opcode);
        
    /*************** INSTRUCTIONS ***************/
    uint8_t ADC(uint16_t address);
    uint8_t AND(uint16_t address);
    uint8_t ASL(uint16_t address);
    uint8_t BCC(uint16_t address);
    uint8_t BCS(uint16_t address);
    uint8_t BEQ(uint16_t address);
    uint8_t BIT(uint16_t address);
    uint8_t BMI(uint16_t address);
    uint8_t BNE(uint16_t address);
    uint8_t BPL(uint16_t address);
    uint8_t BRK(uint16_t address);
    uint8_t BVC(uint16_t address);
    uint8_t BVS(uint16_t address);
    uint8_t CLC(uint16_t address);
    uint8_t CLD(uint16_t address);
    uint8_t CLI(uint16_t address);
    uint8_t CLV(uint16_t address);
    uint8_t CMP(uint16_t address);
    uint8_t CPX(uint16_t address);
    uint8_t CPY(uint16_t address);
    uint8_t DEC(uint16_t address);
    uint8_t DEX(uint16_t address);
    uint8_t DEY(uint16_t address);
    uint8_t EOR(uint16_t address);
    uint8_t INC(uint16_t address);
    uint8_t INX(uint16_t address);
    uint8_t INY(uint16_t address);
    uint8_t JMP(uint16_t address);
    uint8_t JSR(uint16_t address);
    uint8_t LDA(uint16_t address);
    uint8_t LDX(uint16_t address);
    uint8_t LDY(uint16_t address);
    uint8_t LSR(uint16_t address);
    uint8_t NOP(uint16_t address);
    uint8_t ORA(uint16_t address);
    uint8_t PHA(uint16_t address);
    uint8_t PHP(uint16_t address);
    uint8_t PLA(uint16_t address);
    uint8_t PLP(uint16_t address);
    uint8_t ROL(uint16_t address);
    uint8_t ROR(uint16_t address);
    uint8_t RTI(uint16_t address);
    uint8_t RTS(uint16_t address);
    uint8_t SBC(uint16_t address);
    uint8_t SEC(uint16_t address);
    uint8_t SED(uint16_t address);
    uint8_t SEI(uint16_t address);
    uint8_t STA(uint16_t address);
    uint8_t STX(uint16_t address);
    uint8_t STY(uint16_t address);
    uint8_t TAX(uint16_t address);
    uint8_t TAY(uint16_t address);
    uint8_t TSX(uint16_t address);
    uint8_t TXA(uint16_t address);
    uint8_t TXS(uint16_t address);
    uint8_t TYA(uint16_t address);

    /*************** HELPER FUNCTIONS ***************/
    void PushToStack(uint8_t value);
    uint8_t PopFromStack();
    void SetProcessorStatus(uint8_t value);
    uint8_t GetProcessorStatus();

    /*************** ADDRESSING MODES ***************/
    uint16_t AMImplied();
    uint16_t AMAccumulator();
    uint16_t AMImmediate();
    uint16_t AMZeroPage();
    uint16_t AMZeroPageX();
    uint16_t AMZeroPageY();
    uint16_t AMRelative();
    uint16_t AMAbsolute();
    uint16_t AMAbsoluteX();
    uint16_t AMAbsoluteY();
    uint16_t AMIndirect();
    uint16_t AMIndexedIndirect();
    uint16_t AMIndirectIndexed();

    /************* OPCODE TABLE ************/
    struct OpcodeProperties {
        uint8_t (Cpu::*op)(uint16_t);
        uint16_t (Cpu::*am)();  // addressing mode
        uint8_t cycles;
        uint8_t penalty;  // penalty for crossing a page (cycles)
    };

    const OpcodeProperties opcodes[NUM_OPCODES] = {
        {.op = &Cpu::BRK, .am = &Cpu::AMImplied, .cycles = 7, .penalty = 0},
        {.op = &Cpu::ORA, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::ORA, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::ASL, .am = &Cpu::AMZeroPage, .cycles = 5, .penalty = 0},
        {},
        {.op = &Cpu::PHP, .am = &Cpu::AMImplied, .cycles = 3, .penalty = 0},
        {.op = &Cpu::ORA, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::ASL, .am = &Cpu::AMAccumulator, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ORA, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ASL, .am = &Cpu::AMAbsolute, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::BPL, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::ORA, .am = &Cpu::AMIndirectIndexed, .cycles = 5, .penalty = 1},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ORA, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ASL, .am = &Cpu::AMZeroPageX, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::CLC, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::ORA, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {.op = &Cpu::NOP, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::ORA, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::ASL, .am = &Cpu::AMAbsoluteX, .cycles = 7, .penalty = 0},
        {},
        {.op = &Cpu::JSR, .am = &Cpu::AMAbsolute, .cycles = 6, .penalty = 0},
        {.op = &Cpu::AND, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {},
        {},
        {.op = &Cpu::BIT, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::AND, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::ROL, .am = &Cpu::AMZeroPage, .cycles = 5, .penalty = 0},
        {},
        {.op = &Cpu::PLP, .am = &Cpu::AMImplied, .cycles = 4, .penalty = 0},
        {.op = &Cpu::AND, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::ROL, .am = &Cpu::AMAccumulator, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::BIT, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::AND, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ROL, .am = &Cpu::AMAbsolute, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::BMI, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::AND, .am = &Cpu::AMIndirectIndexed, .cycles = 5, .penalty = 1},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::AND, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ROL, .am = &Cpu::AMZeroPageX, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::SEC, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::AND, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {.op = &Cpu::NOP, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::AND, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::ROL, .am = &Cpu::AMAbsoluteX, .cycles = 7, .penalty = 0},
        {},
        {.op = &Cpu::RTI, .am = &Cpu::AMImplied, .cycles = 6, .penalty = 0},
        {.op = &Cpu::EOR, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::EOR, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::LSR, .am = &Cpu::AMZeroPage, .cycles = 5, .penalty = 0},
        {},
        {.op = &Cpu::PHA, .am = &Cpu::AMImplied, .cycles = 3, .penalty = 0},
        {.op = &Cpu::EOR, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::LSR, .am = &Cpu::AMAccumulator, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::JMP, .am = &Cpu::AMAbsolute, .cycles = 3, .penalty = 0},
        {.op = &Cpu::EOR, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::LSR, .am = &Cpu::AMAbsolute, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::BVC, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::EOR, .am = &Cpu::AMIndirectIndexed, .cycles = 5, .penalty = 1},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::EOR, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::LSR, .am = &Cpu::AMZeroPageX, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::CLI, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::EOR, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {.op = &Cpu::NOP, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::EOR, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::LSR, .am = &Cpu::AMAbsoluteX, .cycles = 7, .penalty = 0},
        {},
        {.op = &Cpu::RTS, .am = &Cpu::AMImplied, .cycles = 6, .penalty = 0},
        {.op = &Cpu::ADC, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::ADC, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::ROR, .am = &Cpu::AMZeroPage, .cycles = 5, .penalty = 0},
        {},
        {.op = &Cpu::PLA, .am = &Cpu::AMImplied, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ADC, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::ROR, .am = &Cpu::AMAccumulator, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::JMP, .am = &Cpu::AMIndirect, .cycles = 5, .penalty = 0},
        {.op = &Cpu::ADC, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ROR, .am = &Cpu::AMAbsolute, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::BVS, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::ADC, .am = &Cpu::AMIndirectIndexed, .cycles = 5, .penalty = 1},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ADC, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::ROR, .am = &Cpu::AMZeroPageX, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::SEI, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::ADC, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {.op = &Cpu::NOP, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::ADC, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::ROR, .am = &Cpu::AMAbsoluteX, .cycles = 7, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::STA, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {.op = &Cpu::NOP, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::STY, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::STA, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::STX, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {},
        {.op = &Cpu::DEY, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::NOP, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::TXA, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::STY, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::STA, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::STX, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {},
        {.op = &Cpu::BCC, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::STA, .am = &Cpu::AMIndirectIndexed, .cycles = 6, .penalty = 0},
        {},
        {},
        {.op = &Cpu::STY, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::STA, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::STX, .am = &Cpu::AMZeroPageY, .cycles = 4, .penalty = 0},
        {},
        {.op = &Cpu::TYA, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::STA, .am = &Cpu::AMAbsoluteY, .cycles = 5, .penalty = 0},
        {.op = &Cpu::TXS, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {},
        {.op = &Cpu::STA, .am = &Cpu::AMAbsoluteX, .cycles = 5, .penalty = 0},
        {},
        {},
        {.op = &Cpu::LDY, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::LDA, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {.op = &Cpu::LDX, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::LDY, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::LDA, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::LDX, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {},
        {.op = &Cpu::TAY, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::LDA, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::TAX, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::LDY, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::LDA, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::LDX, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {},
        {.op = &Cpu::BCS, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::LDA, .am = &Cpu::AMIndirectIndexed, .cycles = 5, .penalty = 1},
        {},
        {},
        {.op = &Cpu::LDY, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::LDA, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::LDX, .am = &Cpu::AMZeroPageY, .cycles = 4, .penalty = 0},
        {},
        {.op = &Cpu::CLV, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::LDA, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {.op = &Cpu::TSX, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::LDY, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::LDA, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::LDX, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {},
        {.op = &Cpu::CPY, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::CMP, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {.op = &Cpu::NOP, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::CPY, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::CMP, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::DEC, .am = &Cpu::AMZeroPage, .cycles = 5, .penalty = 0},
        {},
        {.op = &Cpu::INY, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::CMP, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::DEX, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::CPY, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::CMP, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::DEC, .am = &Cpu::AMAbsolute, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::BNE, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::CMP, .am = &Cpu::AMIndirectIndexed, .cycles = 5, .penalty = 1},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::CMP, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::DEC, .am = &Cpu::AMZeroPageX, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::CLD, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::CMP, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {.op = &Cpu::NOP, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::CMP, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::DEC, .am = &Cpu::AMAbsoluteX, .cycles = 7, .penalty = 0},
        {},
        {.op = &Cpu::CPX, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::SBC, .am = &Cpu::AMIndexedIndirect, .cycles = 6, .penalty = 0},
        {.op = &Cpu::NOP, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::CPX, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::SBC, .am = &Cpu::AMZeroPage, .cycles = 3, .penalty = 0},
        {.op = &Cpu::INC, .am = &Cpu::AMZeroPage, .cycles = 5, .penalty = 0},
        {},
        {.op = &Cpu::INX, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::SBC, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::NOP, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::SBC, .am = &Cpu::AMImmediate, .cycles = 2, .penalty = 0},
        {.op = &Cpu::CPX, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::SBC, .am = &Cpu::AMAbsolute, .cycles = 4, .penalty = 0},
        {.op = &Cpu::INC, .am = &Cpu::AMAbsolute, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::BEQ, .am = &Cpu::AMRelative, .cycles = 2, .penalty = 1},
        {.op = &Cpu::SBC, .am = &Cpu::AMIndirectIndexed, .cycles = 5, .penalty = 1},
        {},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::SBC, .am = &Cpu::AMZeroPageX, .cycles = 4, .penalty = 0},
        {.op = &Cpu::INC, .am = &Cpu::AMZeroPageX, .cycles = 6, .penalty = 0},
        {},
        {.op = &Cpu::SED, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {.op = &Cpu::SBC, .am = &Cpu::AMAbsoluteY, .cycles = 4, .penalty = 1},
        {.op = &Cpu::NOP, .am = &Cpu::AMImplied, .cycles = 2, .penalty = 0},
        {},
        {.op = &Cpu::NOP, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::SBC, .am = &Cpu::AMAbsoluteX, .cycles = 4, .penalty = 1},
        {.op = &Cpu::INC, .am = &Cpu::AMAbsoluteX, .cycles = 7, .penalty = 0},
        {}
    };

    /*************** LOGGING ***************/
    void PrintHeader();
    void SetLogging(std::string file_name, int track);
    void SetLogChecking(std::string correct_log);
    int PrintState();
    char context_[20];
    uint16_t instr_pc_;
    std::string instr_;
    int call_number_ = 0;
    std::string file_name_;
    FILE *correct_log_ = NULL;
    int track_ = 0;
};

#endif  // NES_CPU_CPU_H_
