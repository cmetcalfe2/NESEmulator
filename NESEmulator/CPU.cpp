#include "CPU.h"



CPU::CPU(Memory* memory)
{
	InitialiseRegisters();
	mem = memory;
	std::cout << "Address mode: " << InstructionInfo::instructionAddressingModes[0x31] << std::endl;
}


CPU::~CPU()
{
	mem = NULL;
}

void CPU::InitialiseRegisters()
{
	pc = 0;
	sp = 0x01FF - 0x0100;
	a = 0;
	x = 0;
	y = 0;
	ps = ProcessorStatus();
}

bool CPU::IsSignOverflow(uint8_t oldVal, uint8_t val, uint8_t newVal)
{
	return (((oldVal & 0x80) == (val & 0x80)) &&
		((oldVal & 0x80) != (newVal & 0x80)));
}

void CPU::UpdateCFlag(bool set)
{
	if (set)
		ps.SetC();
	else
		ps.UnSetC();
}

void CPU::UpdateZFlag(bool set)
{
	if (set)
		ps.SetZ();
	else
		ps.UnSetZ();
}

void CPU::UpdateIFlag(bool set)
{
	if (set)
		ps.SetI();
	else
		ps.UnSetI();
}

void CPU::UpdateDFlag(bool set)
{
	if (set)
		ps.SetD();
	else
		ps.UnSetD();
}

void  CPU::UpdateNFlag(bool set)
{
	if (set)
		ps.SetN();
	else
		ps.UnSetN();
}

void CPU::UpdateZNFlags(uint8_t val)
{
	UpdateZFlag(val == 0);
	UpdateNFlag(val & 0x80 != 0);
}

void CPU::UpdateVFlag(bool set)
{
	if (set)
		ps.SetV();
	else
		ps.UnSetV();
}

void CPU::IncrementStackPointer()
{
	sp++;

	// 6502 has no detection of stack overflow, simply loops around
	// Stack overflow shouldnt occur on increment anyway (stack is top-down)
	if (sp > 0x01FF - 0x0100)
		sp = 0;
}

void CPU::DecrementStackPointer()
{
	sp++;

	// 6502 has no detection of stack overflow, simply loops around
	if (sp < 0)
		sp = 0x01FF - 0x0100;
}

void CPU::InitialiseInstructionTables()
{
	// Initialise instruction jump table
	for (int i = 0; i < 256; i++)
	{
		instructionJumpTable[i] = instructionFunctions[InstructionInfo::instructions[i]];
	}

	// Initalise instruction types (read, write, read-write)
	for (int i = 0; i < 256; i++)
	{
		Instruction inst = InstructionInfo::instructions[i];

		//if (inst == )
	}
}
