#pragma once
#include <map>


enum InstructionAddressingMode
{
	AM_NA,		// Instruction not implemented (Unofficial/undocumented opcode)
	AM_ACC,		// Accumulator / Accumulator
	AM_IMPL,	// Accumulator
	AM_IMM,		// Immediate
	AM_ZP,		// Zero page
	AM_ZPX,		// Zero page, X
	AM_ZPY,		// Zero page, Y
	AM_REL,		// Relative
	AM_ABS,		// Absolute
	AM_ABSX,	// Absolute, X
	AM_ABSY,	// Absolute, Y
	AM_INDIR,	// Indirect
	AM_INDIRX,	// Indexed Indirect
	AM_INDIRY	// Indirect Indexed
};

enum InstructionType
{
	IT_NA,	// Unknown instruction
	IT_R,	// Read
	IT_RW,	// Read-modify-write
	IT_W	// Write
};

enum Instruction
{
	IN_UNK, // Unknown instruction (Unofficial/undocumented opcode)
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
};

class InstructionInfo
{
public:
	static const InstructionAddressingMode instructionAddressingModes[256];
	static const Instruction instructions[256];
	static const std::map<Instruction, InstructionType> instructionTypes;
};