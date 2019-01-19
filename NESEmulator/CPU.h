#pragma once
#include "CPUInstructions.h"
#include "Memory.h"
#include "Logger.h"
#include "Mapper.h"
#include "Interrupts.h"
#include <iostream>
#include <ctime>
#include <vector>

class CPU;
typedef void (CPU::*InstructionFunction)();
typedef void (CPU::*AddressModeFunction)();

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

	uint8_t U() { return (status >> 5) & 1; }; // Unused bit, set by PHP and BRK
	void SetU() { status |= 1 << 5; };
	void UnSetU() { status &= ~(1 << 5); };

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
	CPU(Memory* memory, Interrupts* interrupts);
	~CPU();

	void Init();
	void Cycle();

	void OnReset();

	void SetMapper(Mapper* mapper);

	bool IsInstructionFinished() { return instructionFinished; }
	unsigned long GetCyclesElapsed() { return cyclesElapsed; }

	Memory* mem;
	ProcessorStatus ps;

private:
	void InitialiseInstructionTables();
	void InitialiseRegisters();

	void Log();

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

	// Address Mode functions
	void AddressMode_NA();						// Instruction not implemented (Unofficial/undocumented opcode)
	void AddressMode_ACC();						// Accumulator / Accumulator
	void AddressMode_IMPL();					// Accumulator
	void AddressMode_IMM();						// Immediate
	void AddressMode_ZP();						// Zero page
	void AddressMode_ZPI(uint8_t* reg);			// Zero page indexed
	void AddressMode_ZPX();						// Zero page, X
	void AddressMode_ZPY();						// Zero page, Y
	void AddressMode_REL();						// Relative
	void AddressMode_ABS();						// Absolute
	void AddressMode_ABSI(uint8_t* reg);		// Absolute indexed
	void AddressMode_ABSX();					// Absolute, X
	void AddressMode_ABSY();					// Absolute, Y
	void AddressMode_INDIR();					// Indirect
	void AddressMode_INDIRX();					// Indexed Indirect
	void AddressMode_INDIRY();					// Indirect Indexed

	// Processor Status Helpers
	bool IsSignOverflow(uint8_t oldVal, uint8_t val, uint8_t newVal);
	void UpdateCFlag(bool set);
	void UpdateZFlag(bool set);
	void UpdateIFlag(bool set);
	void UpdateDFlag(bool set);
	void UpdateVFlag(bool set);
	void UpdateNFlag(bool set);
	void UpdateZNFlags(uint8_t val);

	// Register helpers
	void IncrementPC();
	void IncrementStackPointer();
	void DecrementStackPointer();
	uint8_t RealStackPointer() { return (sp - 0x0100) & 0x00FF; };
	void SetRealStackPointer(uint8_t val) { sp = val + 0x0100; };

	// Interrupt polling
	void PollInterrupts();

	// Mapper
	Mapper* mapper;

	// Registers
	uint16_t pc;
	uint16_t sp;
	uint8_t a;
	uint8_t x;
	uint8_t y;

	// Busses/Instruction vars
	uint8_t tempByte = 0x00;
	uint16_t tempWord = 0x0000; // Temp latch used for some instructions (i.e. absolute JMP)
	uint8_t* index = NULL;
	uint16_t pointer = 0x00;
	uint16_t address = 0x0000;
	uint8_t memVal = 0x00;
	bool branchTaken = false;
	bool pageCrossed = false;

	// Interrupts
	Interrupts* interrupts;
	InterruptType pendingInterrupt = INTERRUPT_NONE;
	//InterruptType lastPendingInterrupt = INTERRUPT_NONE;
	InterruptType interruptBeingHandled = INTERRUPT_NONE;

	// Instruction tables
	std::map<Instruction, InstructionFunction> instructionFunctions = {
		{ IN_UNK, &CPU::UNK }, // Unknown instruction (Unofficial/undocumented opcode)
		{ IN_ADC, &CPU::ADC },
		{ IN_AND, &CPU::AND },
		{ IN_ASL, &CPU::ASL },
		{ IN_BCC, &CPU::BCC },
		{ IN_BCS, &CPU::BCS },
		{ IN_BEQ, &CPU::BEQ },
		{ IN_BIT, &CPU::BIT },
		{ IN_BMI, &CPU::BMI },
		{ IN_BNE, &CPU::BNE },
		{ IN_BPL, &CPU::BPL },
		{ IN_BRK, &CPU::BRK },
		{ IN_BVC, &CPU::BVC },
		{ IN_BVS, &CPU::BVS },
		{ IN_CLC, &CPU::CLC },
		{ IN_CLD, &CPU::CLD },
		{ IN_CLI, &CPU::CLI },
		{ IN_CLV, &CPU::CLV },
		{ IN_CMP, &CPU::CMP },
		{ IN_CPX, &CPU::CPX },
		{ IN_CPY, &CPU::CPY },
		{ IN_DEC, &CPU::DEC },
		{ IN_DEX, &CPU::DEX },
		{ IN_DEY, &CPU::DEY },
		{ IN_EOR, &CPU::EOR },
		{ IN_INC, &CPU::INC },
		{ IN_INX, &CPU::INX },
		{ IN_INY, &CPU::INY },
		{ IN_JMP, &CPU::JMP },
		{ IN_JSR, &CPU::JSR },
		{ IN_LDA, &CPU::LDA },
		{ IN_LDX, &CPU::LDX },
		{ IN_LDY, &CPU::LDY },
		{ IN_LSR, &CPU::LSR },
		{ IN_NOP, &CPU::NOP },
		{ IN_ORA, &CPU::ORA },
		{ IN_PHA, &CPU::PHA },
		{ IN_PHP, &CPU::PHP },
		{ IN_PLA, &CPU::PLA },
		{ IN_PLP, &CPU::PLP },
		{ IN_ROL, &CPU::ROL },
		{ IN_ROR, &CPU::ROR },
		{ IN_RTI, &CPU::RTI },
		{ IN_RTS, &CPU::RTS },
		{ IN_SBC, &CPU::SBC },
		{ IN_SEC, &CPU::SEC },
		{ IN_SED, &CPU::SED },
		{ IN_SEI, &CPU::SEI },
		{ IN_STA, &CPU::STA },
		{ IN_STX, &CPU::STX },
		{ IN_STY, &CPU::STY },
		{ IN_TAX, &CPU::TAX },
		{ IN_TAY, &CPU::TAY },
		{ IN_TSX, &CPU::TSX },
		{ IN_TXA, &CPU::TXA },
		{ IN_TXS, &CPU::TXS },
		{ IN_TYA, &CPU::TYA }
	};

	std::map<InstructionAddressingMode, AddressModeFunction> addressModeFunctions = {
		{ AM_NA, &CPU::AddressMode_NA },			// Instruction not implemented (Unofficial/undocumented opcode)
		{ AM_ACC, &CPU::AddressMode_ACC },			// Accumulator / Accumulator
		{ AM_IMPL, &CPU::AddressMode_IMPL },		// Accumulator
		{ AM_IMM, &CPU::AddressMode_IMM },			// Immediate
		{ AM_ZP, &CPU::AddressMode_ZP },			// Zero page
		{ AM_ZPX, &CPU::AddressMode_ZPX },			// Zero page, X
		{ AM_ZPY, &CPU::AddressMode_ZPY },			// Zero page, Y
		{ AM_REL, &CPU::AddressMode_REL },			// Relative
		{ AM_ABS, &CPU::AddressMode_ABS },			// Absolute
		{ AM_ABSX, &CPU::AddressMode_ABSX },		// Absolute, X
		{ AM_ABSY, &CPU::AddressMode_ABSY },		// Absolute, Y
		{ AM_INDIR, &CPU::AddressMode_INDIR },		// Indirect
		{ AM_INDIRX, &CPU::AddressMode_INDIRX },	// Indexed Indirect
		{ AM_INDIRY, &CPU::AddressMode_INDIRY }		// Indirect Indexed
	};

	InstructionFunction instructionJumpTable[256];
	AddressModeFunction addressModeJumpTable[256];
	InstructionType instructionTypeTable[256];
	std::string instructionNameStringTable[256];

	// Current instruction info
	uint8_t curInstructionOpcode;
	uint8_t curOperand1;
	uint8_t curOperand2;
	uint16_t curInstructionPC = 0x0000;
	InstructionAddressingMode curAddressMode = AM_NA;
	InstructionType curInstructionType = IT_NA;
	std::string curInstructionNameString = "UNK";
	int instructionProgress = 0; // Number of cycles executed for current instruction
	bool instructionFinished = true; // Instruction has finished executing

	// Timer/Debugging
	std::clock_t startTime;
	unsigned long cyclesThisSecond = 0;
	unsigned long cyclesElapsed = 0;
	std::vector<Instruction> callHistory;
};

