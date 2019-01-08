#include "CPU.h"

// Instruction functions
void CPU::ADC()
{
	uint8_t oldVal = a;
	uint8_t newVal = oldVal;
	uint8_t val = mem->ReadByte(address);
	bool overflow = false;

	// Add mem val and c bit seperately, check overflow at each stage
	newVal += val;
	if (newVal < oldVal)
		overflow = true;

	uint8_t oldVal2 = newVal;
	newVal += ps.C();
	if (newVal < oldVal2)
		overflow = true;

	// Save result to acc
	a = newVal;

	// Update flags
	UpdateVFlag(IsSignOverflow(oldVal, val, newVal));
	UpdateCFlag(overflow);
	UpdateZNFlags(a);
}

void CPU::AND()
{
	// Perform AND operation
	a = mem->ReadByte(address) & 0xFF;

	// Update flags
	UpdateZNFlags(a);
}

void CPU::ASL()
{
	uint8_t oldVal;

	if (curAddressMode == AM_ACC)
		oldVal = a;
	else
		oldVal = mem->ReadByte(address);

	uint8_t newVal = oldVal << 1;

	if (curAddressMode == AM_ACC)
		a = newVal;
	else
		mem->SetByte(address, newVal);

	UpdateCFlag(oldVal & 0x80);	
	UpdateZNFlags(newVal);

}

void CPU::BCC()
{

}

void CPU::BCS()
{

}

void CPU::BEQ()
{

}

void CPU::BIT()
{
	uint8_t memVal = mem->ReadByte(address);
	uint8_t val = a & memVal;

	UpdateZFlag(val == 0);
	UpdateVFlag(memVal & 0x40);
	UpdateNFlag(memVal & 0x80);
}

void CPU::BMI()
{

}

void CPU::BNE()
{

}

void CPU::BPL()
{

}

void CPU::BRK()
{
	// Do not implement
}

void CPU::BVC()
{
	
}

void CPU::BVS()
{

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
	uint8_t memVal = mem->ReadByte(address);
	uint8_t val = a - memVal;

	UpdateCFlag(a >= memVal);
	UpdateZNFlags(val);
}

void CPU::CPX()
{
	uint8_t memVal = mem->ReadByte(address);
	uint8_t val = x - memVal;

	UpdateCFlag(x >= memVal);
	UpdateZNFlags(val);
}

void CPU::CPY()
{
	uint8_t memVal = mem->ReadByte(address);
	uint8_t val = y - memVal;

	UpdateCFlag(y >= memVal);
	UpdateZNFlags(val);
}

void CPU::DEC()
{
	uint8_t oldVal = mem->ReadByte(address);
	uint8_t newVal = oldVal  - 1;

	mem->SetByte(address, newVal);

	UpdateZNFlags(newVal);
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
	a = a ^ mem->ReadByte(address);

	UpdateZNFlags(a);
}

void CPU::INC()
{
	uint8_t oldVal = mem->ReadByte(address);
	uint8_t newVal = oldVal + 1;

	mem->SetByte(address, newVal);

	UpdateZNFlags(newVal);
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

}

void CPU::JSR()
{

}

void CPU::LDA()
{
	a = mem->ReadByte(address);

	UpdateZNFlags(a);
}

void CPU::LDX()
{
	x = mem->ReadByte(address);

	UpdateZNFlags(x);
}

void CPU::LDY()
{
	y = mem->ReadByte(address);

	UpdateZNFlags(y);
}

void CPU::LSR()
{
	uint8_t oldVal;

	if (curAddressMode == AM_ACC)
		oldVal = a;
	else
		oldVal = mem->ReadByte(address);

	uint8_t newVal = oldVal >> 1;

	if (curAddressMode == AM_ACC)
		mem->SetByte(address, newVal);
	else
		a = newVal;

	UpdateCFlag(oldVal & 1);
	UpdateZNFlags(newVal);
}

void CPU::NOP()
{
	// Do nothing
	instructionFinished = true;
}

void CPU::ORA()
{
	a = a | mem->ReadByte(address);
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
	else
	{
		a = mem->ReadByte(sp);
		instructionFinished = true;
	}
}

void CPU::PLP()
{
	if (instructionProgress == 2)
	{
		IncrementStackPointer();
	}
	else
	{
		ps.status = mem->ReadByte(sp);
		instructionFinished = true;
	}
}

void CPU::ROL()
{
	uint8_t oldVal;

	if (curAddressMode == AM_ACC)
		oldVal = a;
	else
		oldVal = mem->ReadByte(address);

	uint8_t newVal = (oldVal << 1) | ps.C();

	if (curAddressMode == AM_ACC)
		a = newVal;
	else
		mem->SetByte(address, newVal);

	UpdateCFlag(oldVal & 0x80);
	UpdateZNFlags(newVal);
}

void CPU::ROR()
{
	uint8_t oldVal;

	if (curAddressMode == AM_ACC)
		oldVal = a;
	else
		oldVal = mem->ReadByte(address);

	uint8_t newVal = (oldVal >> 1) | (ps.C() << 7);

	if (curAddressMode == AM_ACC)
		a = newVal;
	else
		mem->SetByte(address, newVal);

	UpdateCFlag(oldVal & 1);
	UpdateZNFlags(newVal);
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
		IncrementStackPointer();
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
		IncrementStackPointer();
		instructionFinished = true;
		break;
	case 5:
		pc += 2;
		break;
	}
}

void CPU::SBC()
{
	uint8_t oldVal = a;
	uint8_t newVal = oldVal;
	uint8_t val = ~mem->ReadByte(address);
	bool overflow = false;

	// Add mem val and c bit seperately, check overflow at each stage
	newVal += val;
	if (newVal < oldVal)
		overflow = true;

	uint8_t oldVal2 = newVal;
	newVal += ps.C();
	if (newVal < oldVal2)
		overflow = true;

	// Save result to acc
	a = newVal;

	// Update flags
	UpdateVFlag(IsSignOverflow(oldVal, val, newVal));
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
	mem->SetByte(address, a);
}

void CPU::STX()
{
	mem->SetByte(address, x);
}

void CPU::STY()
{
	mem->SetByte(address, y);
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
	x = sp;
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
	sp = x;

	instructionFinished = true;
}

void CPU::TYA()
{
	a = y;
	UpdateZNFlags(a);

	instructionFinished = true;
}
