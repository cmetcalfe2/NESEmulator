#include "CPU.h"



CPU::CPU(Memory* memory)
{
	mem = memory;
	InitialiseInstructionTables();
	Reset();
}


CPU::~CPU()
{
	mem = NULL;
}

void CPU::Reset()
{
	InitialiseRegisters();
	instructionFinished = true;
	startTime = std::clock();
}

void CPU::InitialiseRegisters()
{
	pc = (mem->ReadByte(0xFFFD) << 8) + mem->ReadByte(0xFFFC);
	if (pc == 0 || pc == 0xFFFF)
	{
		pc = 0x8000;
	}
	pc = 0xC000; // TEST : REMOVE
	printf("0x%X\n", pc);
	sp = 0x01FF;
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
	UpdateNFlag((val & 0x80) != 0);
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
	if (sp > 0x01FF)
		sp = 0x0100;
}

void CPU::DecrementStackPointer()
{
	sp--;

	// 6502 has no detection of stack overflow, simply loops around
	if (sp < 0x0100)
		sp = 0x01FF;
}

void CPU::IncrementPC()
{
	pc++;
}

void CPU::InitialiseInstructionTables()
{
	for (int i = 0; i < 256; i++)
	{
		instructionJumpTable[i] = instructionFunctions[InstructionInfo::instructions[i]]; // instruction jump table
		instructionTypeTable[i] = InstructionInfo::instructionTypes.at(InstructionInfo::instructions[i]); // instruction type table(read, write, read - write)
		addressModeJumpTable[i] = addressModeFunctions[InstructionInfo::instructionAddressingModes[i]];
	}
}

void CPU::Cycle()
{
	if (instructionFinished)
	{
		Log();
		uint8_t lastError = mem->ReadByte(0x0002);
		if (lastError != 0)
		{
			printf("Error: %X\n", lastError);
		}

		// Fetch opcode of next instruction
		instructionProgress = 0;
		curInstructionPC = pc;
		curInstructionOpcode = mem->ReadByte(pc);
		callHistory.push_back(InstructionInfo::instructions[curInstructionOpcode]);
		curAddressMode = InstructionInfo::instructionAddressingModes[curInstructionOpcode];
		curInstructionType = instructionTypeTable[curInstructionOpcode];
		IncrementPC();
		instructionFinished = false;
	}
	else
	{
		(this->*addressModeJumpTable[curInstructionOpcode])();
	}
	instructionProgress++;
	cyclesThisSecond++;
	cyclesElapsed++;

	//if (cyclesThisSecond )

	// Timing
	if ((std::clock() - startTime) >= CLOCKS_PER_SEC)
	{
		printf("Cycles per second - %fM\n", (double)cyclesThisSecond / 1000000.0);
		cyclesThisSecond = 0;
		startTime = std::clock();
	}
}

void CPU::Log()
{
	printf("%04X  %02X %04X    A:%02X X:%02X Y:%02X    PS:%02X    CYC:%i\n", curInstructionPC, curInstructionOpcode, address, a, x, y, ps.status, cyclesElapsed);
}
