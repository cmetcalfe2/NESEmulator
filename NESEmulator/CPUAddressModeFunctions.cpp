#include "CPU.h"

void CPU::AddressMode_NA()
{

}

void CPU::AddressMode_ACC()
{
	(this->*instructionJumpTable[curInstructionAddress])();
	instructionFinished = true;
}

void CPU::AddressMode_IMPL()
{
	(this->*instructionJumpTable[curInstructionAddress])();
}

void CPU::AddressMode_IMM()
{
	address = mem->ReadByte(pc);
	IncrementPC();

	(this->*instructionJumpTable[curInstructionAddress])();

	instructionFinished = true;
}

void CPU::AddressMode_ZP()
{
	switch (instructionProgress)
	{
	case 1:
		address = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			(this->*instructionJumpTable[curInstructionAddress])();
			instructionFinished = true;
		}
		else
		{
			tempByte = mem->ReadByte(address);
		}
		break;
	case 3:
		mem->SetByte(address, tempByte);
		break;
	case 4:
		(this->*instructionJumpTable[curInstructionAddress])();
		instructionFinished = true;
		break;
	}
}

void CPU::AddressMode_ZPX()
{
	AddressMode_ZPI(&x);
}

void CPU::AddressMode_ZPY()
{
	AddressMode_ZPI(&y);
}

void CPU::AddressMode_ZPI(uint8_t* reg)
{

}

void CPU::AddressMode_REL()
{

}

void CPU::AddressMode_ABS()
{

	switch (instructionProgress)
	{
	case 1:
		tempWord = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		tempWord += mem->ReadByte(pc) << 8;
		if (curInstructionAddress == 0x6C) // JMP, special exception
		{
			pc = tempWord;
			instructionFinished = true;
		}
		else
		{
			address = tempWord;
		}
		break;
	case 3:
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			(this->*instructionJumpTable[curInstructionAddress])();

			instructionFinished = true;
		}
		tempByte = mem->ReadByte(address);
		break;
	case 4:
		mem->SetByte(address, tempByte);
		break;
	case 5:
		(this->*instructionJumpTable[curInstructionAddress])();
		instructionFinished = true;
		break;
	}

}

void CPU::AddressMode_ABSX()
{

}

void CPU::AddressMode_ABSY()
{

}

void CPU::AddressMode_INDIR()
{

}

void CPU::AddressMode_INDIRX()
{

}

void CPU::AddressMode_INDIRY()
{

}