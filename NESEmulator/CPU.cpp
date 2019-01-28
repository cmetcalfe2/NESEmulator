#include "CPU.h"



CPU::CPU(Memory* memory, Interrupts* i)
{
	mem = memory;
	interrupts = i;
	InitialiseInstructionTables();
}


CPU::~CPU()
{
	mem = NULL;
	interrupts = NULL;
}

void CPU::Init()
{
	Reset();
}

void CPU::Reset()
{
	InitialiseRegisters();
	instructionFinished = true;
	startTime = std::clock();

	pendingInterrupt = INTERRUPT_RST;
	mem->SetByte(0x4015, 0); // APU Silenced on reset
}

void CPU::InitialiseRegisters()
{
	//pc = (mem->ReadByte(0xFFFD) << 8) + mem->ReadByte(0xFFFC);
	pc = 0;
	/*if (pc == 0 || pc == 0xFFFF)
	{
		pc = 0x8000;
	}*/
	//pc = 0xC000; // TEST : REMOVE
	sp = 0x01FF;
	//sp = 0x01FD; // TEST : REMOVE
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
		addressModeJumpTable[i] = addressModeFunctions[InstructionInfo::instructionAddressingModes[i]]; // address mode function jump table
		instructionNameStringTable[i] = InstructionInfo::instructionNameStrings.at(InstructionInfo::instructions[i]); // instruction name strings
	}
}

void CPU::SetMapper(Mapper* m)
{
	mapper = m;
}

void CPU::PollInterrupts()
{
	if ((pendingInterrupt == INTERRUPT_NONE) || (pendingInterrupt == INTERRUPT_IRQ)) // Only pending IRQ interrupts can be overridden (by NMI)
	{
		InterruptType interruptType = interrupts->PollEdgeDetectors();
		if (interruptType == INTERRUPT_RST)
		{
			pendingInterrupt = interruptType;
			Reset();
		}
		else if (((interruptType == INTERRUPT_IRQ) && (ps.I() == 0)) || (interruptType == INTERRUPT_NMI))
		{
			pendingInterrupt = interruptType;
		}
	}
}

void CPU::Cycle()
{
	if (mem->ppuOAMDMAActive)
	{
		mem->PPUOAMDMACycle();
		cyclesThisSecond++;
		cyclesElapsed++;
		return;
	}

	if (instructionFinished)
	{
		//Log();

		// Fetch opcode of next instruction
		instructionProgress = 0;

		curInstructionPC = pc;

		if (pendingInterrupt == INTERRUPT_NONE)
		{
			curInstructionOpcode = mem->ReadByte(pc);
			IncrementPC();

			if (curInstructionOpcode == 0x00)
			{
				// BRK interrupt
				interruptBeingHandled = INTERRUPT_BRK;
			}
		}
		else
		{
			// Handle interrupt
			interruptBeingHandled = pendingInterrupt;
			pendingInterrupt = INTERRUPT_NONE;
			curInstructionOpcode = 0x00; // BRK
		}

		//curInstructionNameString = instructionNameStringTable[curInstructionOpcode];

		curAddressMode = InstructionInfo::instructionAddressingModes[curInstructionOpcode];

		curInstructionType = instructionTypeTable[curInstructionOpcode];

		//callHistory.push_back(InstructionInfo::instructions[curInstructionOpcode]);

		mapper->OnInstructionFinished();

		instructionFinished = false;
	}
	else
	{
		(this->*addressModeJumpTable[curInstructionOpcode])();
	}
	instructionProgress++;
	cyclesThisSecond++;
	cyclesElapsed++;

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
	bool hasTwoOperands = (curAddressMode == AM_ABS || curAddressMode == AM_ABSX || curAddressMode == AM_ABSY || curAddressMode == AM_INDIR);
	Logger::LogCPUInstruction(curInstructionPC, curInstructionOpcode, curOperand1, curOperand2, address, a, x, y, RealStackPointer(), ps.status, cyclesElapsed, curInstructionNameString, hasTwoOperands);
}
