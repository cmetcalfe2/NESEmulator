#pragma once
#include "CPUInstructions.h"
#include "Memory.h"
#include <iostream>

class CPU;
typedef void (CPU::*InstructionFunction)();

struct ProcessorStatus
{
public:
	uint8_t status = 0x00;

	uint8_t N() { return (status >> 7) & 1; };
	void SetN() { status |= 1 << 7; };
	void UnSetN() { status &= ~(1 << 7); };

	uint8_t V() { return (status >> 6) & 1; };
	void SetV() { status |= 1 << 6; };
	void UnSetV() { status &= ~(1 << 6); };

	uint8_t B() { return (status >> 4) & 1; };
	void SetB() { status |= 1 << 4; };
	void UnSetB() { status &= ~(1 << 4); };

	uint8_t D() { return (status >> 3) & 1; };
	void SetD() { status |= 1 << 3; };
	void UnSetD() { status &= ~(1 << 3); };

	uint8_t I() { return (status >> 2) & 1; };
	void SetI() { status |= 1 << 2; };
	void UnSetI() { status &= ~(1 << 2); };

	uint8_t Z() { return (status >> 1) & 1; };
	void SetZ() { status |= 1 << 1; };
	void UnSetZ() { status &= ~(1 << 1); };

	uint8_t C() { return (status >> 0) & 1; };
	void SetC() { status |= 1 << 0; };
	void UnSetC() { status &= ~(1 << 0); };

};

class CPU
{
public:
	CPU(Memory* memory);
	~CPU();

	Memory* mem;
	ProcessorStatus ps;

private:
	void InitialiseInstructionTables();
	void InitialiseRegisters();

	// Instruction functions
	void ADC();
	void AND();
	void ASL();
	void BCC();
	void BCS();
	void BEQ();
	void BIT();
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void CPX();
	void CPY();
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void INC();
	void INX();
	void INY();
	void JMP();
	void JSR();
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void ROL();
	void ROR();
	void RTI();
	void RTS();
	void SBC();
	void SEC();
	void SED();
	void SEI();
	void STA();
	void STX();
	void STY();
	void TAX();
	void TAY();
	void TSX();
	void TXA();
	void TXS();
	void TYA();
	void UNK();

	// Processor Status Helpers
	bool IsSignOverflow(uint8_t oldVal, uint8_t val, uint8_t newVal);
	void UpdateCFlag(bool set);
	void UpdateZFlag(bool set);
	void UpdateIFlag(bool set);
	void UpdateDFlag(bool set);
	void UpdateNFlag(bool set);
	void UpdateZNFlags(uint8_t val);
	void UpdateVFlag(bool set);

	// Register helpers
	void IncrementStackPointer();
	void DecrementStackPointer();

	// Registers
	uint16_t pc;
	uint8_t sp;
	uint8_t a;
	uint8_t x;
	uint8_t y;

	// Busses/Instruction vars
	uint16_t pointer = 0x0000;
	uint8_t* index = NULL;
	uint16_t address = 0x0000;

	// Instruction tables
	InstructionFunction funcs[1] = { &CPU::TYA };
	std::map<Instruction, InstructionFunction> instructionFunctions[57] = {
		{ IN_UNK, &CPU::UNK } // Unknown instruction (Unofficial/undocumented opcode)
		IN_ADC,
		IN_AND,
		IN_ASL,
		IN_BCC,
		IN_BCS,
		IN_BEQ,
		IN_BIT,
		IN_BMI,
		IN_BNE,
		IN_BPL,
		IN_BRK,
		IN_BVC,
		IN_BVS,
		IN_CLC,
		IN_CLD,
		IN_CLI,
		IN_CLV,
		IN_CMP,
		IN_CPX,
		IN_CPY,
		IN_DEC,
		IN_DEX,
		IN_DEY,
		IN_EOR,
		IN_INC,
		IN_INX,
		IN_INY,
		IN_JMP,
		IN_JSR,
		IN_LDA,
		IN_LDX,
		IN_LDY,
		IN_LSR,
		IN_NOP,
		IN_ORA,
		IN_PHA,
		IN_PHP,
		IN_PLA,
		IN_PLP,
		IN_ROL,
		IN_ROR,
		IN_RTI,
		IN_RTS,
		IN_SBC,
		IN_SEC,
		IN_SED,
		IN_SEI,
		IN_STA,
		IN_STX,
		IN_STY,
		IN_TAX,
		IN_TAY,
		IN_TSX,
		IN_TXA,
		IN_TXS,
		IN_TYA
	}

	InstructionAddressingMode curAddressMode = AM_NA;
	int instructionProgress = 0; // Number of cycles executed for current instruction
	bool instructionFinished = false; // Instruction has finished executing
};

