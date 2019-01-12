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
		IncrementPC();
		break;
	case 2:
		mem->SetByte(sp, (pc & 0xFF00) >> 8);
		DecrementStackPointer();
		break;
	case 3:
		mem->SetByte(sp, (pc & 0x00FF));
		DecrementStackPointer();
		break;
	case 4:
		ps.SetB();
		ps.SetU();
		mem->SetByte(sp, ps.status);
		DecrementStackPointer();
		break;
	case 5:
		ps.SetB();
		pc = mem->ReadByte(0xFFFE);
		break;
	case 6:
		pc += mem->ReadByte(0xFFFF) << 8;
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
	UpdateCFlag(false);
	instructionFinished = true;
}

void CPU::CLD()
{
	UpdateDFlag(false);
	instructionFinished = true;
}

void CPU::CLI()
{
	UpdateIFlag(false);
	instructionFinished = true;
}

void CPU::CLV()
{
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
	x--;

	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::DEY()
{
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
	x++;

	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::INY()
{
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
		mem->SetByte(sp, a);
		DecrementStackPointer();
		instructionFinished = true;
	}
}

void CPU::PHP()
{
	if (instructionProgress == 2)
	{
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
	UpdateCFlag(true);

	instructionFinished = true;
}

void CPU::SED()
{
	UpdateDFlag(true);

	instructionFinished = true;
}

void CPU::SEI()
{
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
	x = a;
	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::TAY()
{
	y = a;
	UpdateZNFlags(y);

	instructionFinished = true;
}

void CPU::TSX()
{
	x = RealStackPointer();
	UpdateZNFlags(x);

	instructionFinished = true;
}

void CPU::TXA()
{
	a = x;
	UpdateZNFlags(a);

	instructionFinished = true;
}

void CPU::TXS()
{
	SetRealStackPointer(x);

	instructionFinished = true;
}

void CPU::TYA()
{
	a = y;
	UpdateZNFlags(a);

	instructionFinished = true;
}

void CPU::UNK()
{
	// Do nothing
}