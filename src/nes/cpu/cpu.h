#ifndef NES_CPU_CPU_H_
#define NES_CPU_CPU_H_

#include <unistd.h>
#include <ctime>
#include <iostream>

#include "nes.h"

//#define DEBUG

namespace nes_cpu {
    class Cpu {
    public:
        Cpu(Nes nes) : nes_(nes) {};
        void Run();

    private:
        Nes nes_;
        
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
        inline uint16_t AMImplied();
        inline uint16_t AMAccumulator();
        inline uint16_t AMImmediate();
        inline uint16_t AMZeroPage();
        inline uint16_t AMZeroPageX();
        inline uint16_t AMZeroPageY();
        inline uint16_t AMRelative();
        inline uint16_t AMAbsolute();
        inline uint16_t AMAbsoluteX();
        inline uint16_t AMAbsoluteY();
        inline uint16_t AMIndirect();
        inline uint16_t AMIndexedIndirect();
        inline uint16_t AMIndirectIndexed();

        /*************** LOGGING ***************/
        #ifdef DEBUG
        void PrintHeader(char* file_name, int track, int call_number);
        void PrintState();
        char context_[20];
        uint16_t instr_pc_;
        string instr_;
        #endif
    };
}
#endif  // NES_CPU_CPU_H_
