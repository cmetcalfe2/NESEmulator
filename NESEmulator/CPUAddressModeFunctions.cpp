#include "CPU.h"

void CPU::AddressMode_NA()
{
	//printf("Address mode unknown!\n");
	instructionFinished = true;
}

void CPU::AddressMode_ACC()
{
	(this->*instructionJumpTable[curInstructionOpcode])();
	instructionFinished = true;
}

void CPU::AddressMode_IMPL()
{
	(this->*instructionJumpTable[curInstructionOpcode])();
}

void CPU::AddressMode_IMM()
{
	address = mem->ReadByte(pc);
	IncrementPC();

	(this->*instructionJumpTable[curInstructionOpcode])();

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
			(this->*instructionJumpTable[curInstructionOpcode])();
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
		(this->*instructionJumpTable[curInstructionOpcode])();
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
	switch (instructionProgress)
	{
	case 1:
		address = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		tempByte = mem->ReadByte(address);
		address += *reg;
		address &= 0x00FF; // High byte is always 0 (page crossings not handled)
		break;
	case 3:
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();
			instructionFinished = true;
		}
		else
		{
			tempByte = mem->ReadByte(address);
		}
		break;
	case 4:
		mem->SetByte(address, tempByte);
		break;
	case 5:
		(this->*instructionJumpTable[curInstructionOpcode])();
		instructionFinished = true;
	}
}

void CPU::AddressMode_REL()
{
	switch (instructionProgress)
	{
	case 1:
		(this->*instructionJumpTable[curInstructionOpcode])();
		tempByte = mem->ReadByte(pc);
		IncrementPC();
		if (!branchTaken)
		{
			instructionFinished = true;
		}
		break;
	case 2:
		tempWord = pc & 0x00FF; // PC low byte
		tempWord += tempByte;
		if (tempWord <= 0x00FF)
		{
			// no page crossing
			pc += tempByte;
			instructionFinished = true;
		}
		break;
	case 3:
		pc += tempByte;
		instructionFinished = true;
		break;
	}
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
		if (curInstructionOpcode == 0x4C) // JMP, special exception
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
			(this->*instructionJumpTable[curInstructionOpcode])();

			instructionFinished = true;
		}
		tempByte = mem->ReadByte(address);
		break;
	case 4:
		mem->SetByte(address, tempByte);
		break;
	case 5:
		(this->*instructionJumpTable[curInstructionOpcode])();
		instructionFinished = true;
		break;
	}

}

void CPU::AddressMode_ABSX()
{
	AddressMode_ABSI(&x);
}

void CPU::AddressMode_ABSY()
{
	AddressMode_ABSI(&y);
}

void CPU::AddressMode_ABSI(uint8_t* reg)
{
	switch (instructionProgress)
	{
	case 1:
		tempWord = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		address = mem->ReadByte(pc) << 8;
		tempWord += *reg;
		IncrementPC();
		break;
	case 3:
		tempByte = mem->ReadByte(address + (tempWord & 0x00FF));
		if (curInstructionType == IT_R)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();
			if (tempWord <= 0x00FF)
			{
				// No page crossing
				instructionFinished = true;
			}
		}
		address += tempWord;
		break;
	case 4:
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();
			instructionFinished = true;
		}
		else
		{
			tempByte = mem->ReadByte(address);
		}
		break;
	case 5:
		mem->SetByte(address, tempByte);
		break;
	case 6:
		(this->*instructionJumpTable[curInstructionOpcode])();
		instructionFinished = true;
		break;
	}
}

void CPU::AddressMode_INDIR()
{
	switch (instructionProgress)
	{
	case 1:
		pointer = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		pointer += mem->ReadByte(pc) << 8;
		IncrementPC();
		break;
	case 3:
		tempByte = mem->ReadByte(pointer);
		break;
	case 4:
		uint8_t oldPointerHigh = pointer >> 8;
		pointer += 1;
		if ((pointer & 0xFF00) >> 8 > oldPointerHigh)
		{
			pointer -= 0xFF00; // Pointer high byte remains same (page crossings not handled)
		}
		pc = mem->ReadByte(pointer) << 8;
		pc += tempByte;
	}
}

void CPU::AddressMode_INDIRX()
{
	switch (instructionProgress)
	{
	case 1:
		pointer = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		tempByte = mem->ReadByte(pointer);
		pointer += x;
		break;
	case 3:
		address = mem->ReadByte(pointer);
		break;
	case 4:
		address += mem->ReadByte(pointer + 1) << 8;
		break;
	case 5:
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();
			instructionFinished = true;
		}
		else
		{
			tempByte = mem->ReadByte(address);
		}
		break;
	case 6:
		mem->SetByte(address, tempByte);
		break;
	case 7:
		(this->*instructionJumpTable[curInstructionOpcode])();
		instructionFinished = true;
		break;
	}
}

void CPU::AddressMode_INDIRY()
{
	switch (instructionProgress)
	{
	case 1:
		pointer = mem->ReadByte(pc);
		IncrementPC();
		break;
	case 2:
		tempWord = mem->ReadByte(pointer);
		break;
	case 3:
		address = mem->ReadByte(pointer + 1) << 8;
		break;
	case 4:
		tempByte = mem->ReadByte(address + (tempWord & 0x00FF));
		if (curInstructionType == IT_R)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();
			if (tempWord <= 0x00FF)
			{
				// No page crossing
				instructionFinished = true;
			}
		}
		address += tempWord;
		break;
	case 5:
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();
			instructionFinished = true;
		}
		else
		{
			tempByte = mem->ReadByte(address);
		}
		break;
	case 6:
		mem->SetByte(address, tempByte);
		break;
	case 7:
		(this->*instructionJumpTable[curInstructionOpcode])();
		instructionFinished = true;
		break;
	}
}