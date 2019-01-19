#include "CPU.h"

// Instruction functions
void CPU::ADC()
{
	uint8_t oldVal = a;
	uint8_t newVal = oldVal;
	bool overflow = false;

	// Add mem val and c bit seperately, check overflow at each stage
	newVal += memVal;
	if (newVal < oldVal)
		overflow = true;

	uint8_t oldVal2 = newVal;
	newVal += ps.C();
	if (newVal < oldVal2)
		overflow = true;

	// Save result to acc
	a = newVal;

	// Update flags
	UpdateVFlag(IsSignOverflow(oldVal, memVal, newVal));
	UpdateCFlag(overflow);
	UpdateZNFlags(a);
}

void CPU::AND()
{
	// Perform AND operation
	a &= memVal;

	// Update flags
	UpdateZNFlags(a);
}

void CPU::ASL()
{
	UpdateCFlag(memVal & 0x80);

	memVal = memVal << 1;

	UpdateZNFlags(memVal);

}

void CPU::BCC()
{
	branchTaken = !ps.C();
}

void CPU::BCS()
{
	branchTaken = ps.C();
}

void CPU::BEQ()
{
	branchTaken = ps.Z();
}

void CPU::BIT()
{
	UpdateVFlag(memVal & 0x40);
	UpdateNFlag(memVal & 0x80);

	memVal = a & memVal;

	UpdateZFlag(memVal == 0);
}

void CPU::BMI()
{
	branchTaken = ps.N();
}

void CPU::BNE()
{
	branchTaken = !ps.Z();
}

void CPU::BPL()
{
	branchTaken = !ps.N();
}

void CPU::BRK()
{
	switch (instructionProgress)
	{
	case 1:
		interrupts->InterruptHijacking(interruptBeingHandled);
		if (interruptBeingHandled == INTERRUPT_BRK)
		{
			IncrementPC();
		}
		break;
	case 2:
		interrupts->InterruptHijacking(interruptBeingHandled);
		if (interruptBeingHandled != INTERRUPT_RST)
		{
			// Stack writes suppressed on reset
			mem->SetByte(sp, (pc & 0xFF00) >> 8);
		}
		DecrementStackPointer();
		break;
	case 3:
		interrupts->InterruptHijacking(interruptBeingHandled);
		if (interruptBeingHandled != INTERRUPT_RST)
		{
			// Stack writes suppressed on reset
			mem->SetByte(sp, (pc & 0x00FF));
		}
		DecrementStackPointer();
		break;
	case 4:
		interrupts->InterruptHijacking(interruptBeingHandled);
		if (interruptBeingHandled == INTERRUPT_BRK)
		{
			ps.SetB();
		}
		ps.SetU();
		if (interruptBeingHandled != INTERRUPT_RST)
		{
			// Stack writes suppressed on reset
			mem->SetByte(sp, ps.status);
		}
		DecrementStackPointer();
		break;
	case 5:
		interrupts->InterruptHijacking(interruptBeingHandled);
		ps.SetI();
		if (interruptBeingHandled == INTERRUPT_BRK || interruptBeingHandled == INTERRUPT_IRQ)
		{
			pc = mem->ReadByte(0xFFFE);
		}
		else if (interruptBeingHandled == INTERRUPT_NMI)
		{
			pc = mem->ReadByte(0xFFFA);
		}
		else
		{
			pc = mem->ReadByte(0xFFFC);
		}

		break;
	case 6:
		if (interruptBeingHandled == INTERRUPT_BRK || interruptBeingHandled == INTERRUPT_IRQ)
		{
			pc += mem->ReadByte(0xFFFF) << 8;
		}
		else if (interruptBeingHandled == INTERRUPT_NMI)
		{
			pc += mem->ReadByte(0xFFFB) << 8;
		}
		else
		{
			pc += mem->ReadByte(0xFFFD) << 8;
		}
		
		interruptBeingHandled = INTERRUPT_NONE;
		instructionFinished = true;
		break;
	}
}

void CPU::BVC()
{
	branchTaken = !ps.V();
}

void CPU::BVS()
{
	branchTaken = ps.V();
}

void CPU::CLC()
{
	PollInterrupts();
	UpdateCFlag(false);
	instructionFinished = true;
}

void CPU::CLD()
{
	PollInterrupts();
	UpdateDFlag(false);
	instructionFinished = true;
}

void CPU::CLI()
{
	PollInterrupts();
	UpdateIFlag(false);
	instructionFinished = true;
}

void CPU::CLV()
{
	PollInterrupts();
	UpdateVFlag(false);
	instructionFinished = true;
}

void CPU::CMP()
{
	UpdateCFlag(a >= memVal);

	memVal = a - memVal;

	UpdateZNFlags(memVal);
}

void CPU::CPX()
{
	UpdateCFlag(x >= memVal);

	memVal = x - memVal;

	UpdateZNFlags(memVal);
}

void CPU::CPY()
{
	UpdateCFlag(y >= memVal);

	memVal = y - memVal;

	UpdateZNFlags(memVal);
}

void CPU::DEC()
{
	memVal--;

	UpdateZNFlags(memVal);
}

void CPU::DEX()
{
	PollInterrupts();

	x--;

	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::DEY()
{
	PollInterrupts();

	y--;

	UpdateZNFlags(y);

	instructionFinished = true;
}

void CPU::EOR()
{
	a = a ^ memVal;

	UpdateZNFlags(a);
}

void CPU::INC()
{
	memVal++;

	UpdateZNFlags(memVal);
}

void CPU::INX()
{
	PollInterrupts();

	x++;

	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::INY()
{
	PollInterrupts();

	y++;

	UpdateZNFlags(y);

	instructionFinished = true;
}

void CPU::JMP()
{
	// Do not implement, implemented in address mode functions
}

void CPU::JSR()
{
	switch (instructionProgress)
	{
	case 1:
		address = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		// internal operation (predecrement S?)
		break;
	case 3:
		mem->SetByte(sp, (pc & 0xFF00) >> 8);
		DecrementStackPointer();
		break;
	case 4:
		mem->SetByte(sp, pc & 0x00FF);
		DecrementStackPointer();
		break;
	case 5:
		PollInterrupts();
		address += mem->ReadByte(pc) << 8;
		pc = address;
		instructionFinished = true;
		break;
	}
}

void CPU::LDA()
{
	a = memVal;

	UpdateZNFlags(a);
}

void CPU::LDX()
{
	x = memVal;

	UpdateZNFlags(x);
}

void CPU::LDY()
{
	y = memVal;

	UpdateZNFlags(y);
}

void CPU::LSR()
{
	UpdateCFlag(memVal & 1);

	memVal = memVal >> 1;

	UpdateZNFlags(memVal);
}

void CPU::NOP()
{
	// Do nothing
	PollInterrupts();
	instructionFinished = true;
}

void CPU::ORA()
{
	a = a | memVal;
	
	UpdateZNFlags(a);
}

void CPU::PHA()
{
	if (instructionProgress == 2)
	{
		PollInterrupts();
		mem->SetByte(sp, a);
		DecrementStackPointer();
		instructionFinished = true;
	}
}

void CPU::PHP()
{
	if (instructionProgress == 2)
	{
		PollInterrupts();
		ps.SetB();
		ps.SetU();
		mem->SetByte(sp, ps.status);
		DecrementStackPointer();
		instructionFinished = true;
	}
}

void CPU::PLA()
{
	if (instructionProgress == 2)
	{
		IncrementStackPointer();
	}
	else if (instructionProgress == 3)
	{
		PollInterrupts();
		a = mem->ReadByte(sp);
		UpdateZNFlags(a);
		instructionFinished = true;
	}
}

void CPU::PLP()
{
	if (instructionProgress == 2)
	{
		IncrementStackPointer();
	}
	else if (instructionProgress == 3)
	{
		PollInterrupts();
		ps.status = mem->ReadByte(sp);
		instructionFinished = true;
	}
}

void CPU::ROL()
{
	uint8_t oldCarryFlag = ps.C();

	UpdateCFlag(memVal & 0x80);

	memVal = (memVal << 1) | oldCarryFlag;

	UpdateZNFlags(memVal);
}

void CPU::ROR()
{
	uint8_t oldCarryFlag = ps.C();

	UpdateCFlag(memVal & 1);

	memVal = (memVal >> 1) | (oldCarryFlag << 7);

	UpdateZNFlags(memVal);
}

void CPU::RTI()
{
	switch (instructionProgress)
	{
	case 2:
		IncrementStackPointer();
		break;
	case 3:
		ps.status = mem->ReadByte(sp);
		IncrementStackPointer();
		break;
	case 4:
		pc = mem->ReadByte(sp);
		IncrementStackPointer();
		break;
	case 5:
		PollInterrupts();
		pc += mem->ReadByte(sp) << 8;
		instructionFinished = true;
		break;
	}
}

void CPU::RTS()
{
	switch (instructionProgress)
	{
	case 2:
		IncrementStackPointer();
		break;
	case 3:
		pc = mem->ReadByte(sp);
		IncrementStackPointer();
		break;
	case 4:
		pc += mem->ReadByte(sp) << 8;
		break;
	case 5:
		PollInterrupts();
		IncrementPC();
		instructionFinished = true;
		break;
	}
}

void CPU::SBC()
{
	uint8_t oldVal = a;
	uint8_t newVal = oldVal;
	memVal = ~memVal;
	bool overflow = false;

	// Add mem val and c bit seperately, check overflow at each stage
	newVal += memVal;
	if (newVal < oldVal)
		overflow = true;

	uint8_t oldVal2 = newVal;
	newVal += ps.C();
	if (newVal < oldVal2)
		overflow = true;

	// Save result to acc
	a = newVal;

	// Update flags
	UpdateVFlag(IsSignOverflow(oldVal, memVal, newVal));
	UpdateCFlag(overflow);
	UpdateZNFlags(a);
}

void CPU::SEC()
{
	PollInterrupts();

	UpdateCFlag(true);

	instructionFinished = true;
}

void CPU::SED()
{
	PollInterrupts();

	UpdateDFlag(true);

	instructionFinished = true;
}

void CPU::SEI()
{
	PollInterrupts();

	UpdateIFlag(true);

	instructionFinished = true;
}

void CPU::STA()
{
	memVal = a;
}

void CPU::STX()
{
	memVal = x;
}

void CPU::STY()
{
	memVal = y;
}

void CPU::TAX()
{
	PollInterrupts();

	x = a;
	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::TAY()
{
	PollInterrupts();

	y = a;
	UpdateZNFlags(y);

	instructionFinished = true;
}

void CPU::TSX()
{
	PollInterrupts();

	x = RealStackPointer();
	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::TXA()
{
	PollInterrupts();

	a = x;
	UpdateZNFlags(a);

	instructionFinished = true;
}

void CPU::TXS()
{
	PollInterrupts();

	SetRealStackPointer(x);

	instructionFinished = true;
}

void CPU::TYA()
{
	PollInterrupts();

	a = y;
	UpdateZNFlags(a);

	instructionFinished = true;
}

void CPU::UNK()
{
	// Do nothing
}