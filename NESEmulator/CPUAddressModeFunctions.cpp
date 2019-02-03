#include "CPU.h"

void CPU::AddressMode_NA()
{
	//printf("Address mode unknown!\n");
	instructionFinished = true;
}

void CPU::AddressMode_ACC()
{
	PollInterrupts();
	memVal = a;
	(this->*instructionJumpTable[curInstructionOpcode])();
	a = memVal;
	instructionFinished = true;
}

void CPU::AddressMode_IMPL()
{ 
	(this->*instructionJumpTable[curInstructionOpcode])();
}

void CPU::AddressMode_IMM()
{
	PollInterrupts();
	memVal = mem->ReadByte(pc);
	curOperand1 = memVal;
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
		curOperand1 = address & 0x00FF;
		IncrementPC();
		break;
	case 2:
		if (curInstructionType == IT_R || curInstructionType == IT_RW)
		{
			memVal = mem->ReadByte(address);
		}
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			PollInterrupts();

			(this->*instructionJumpTable[curInstructionOpcode])();

			if (curInstructionType == IT_W)
			{
				mem->SetByte(address, memVal);
			}

			instructionFinished = true;
		}
		break;
	case 3:
		mem->SetByte(address, memVal);
		(this->*instructionJumpTable[curInstructionOpcode])();
		break;
	case 4:
		PollInterrupts();
		mem->SetByte(address, memVal);
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
	{
		address = mem->ReadByte(pc);
		curOperand1 = address & 0x00FF;
		IncrementPC();
		break;
	}
	case 2:
	{
		memVal = mem->ReadByte(address);
		address += *reg;
		address &= 0x00FF; // High byte is always 0 (page crossings not handled)
		break;
	}
	case 3:
	{
		if (curInstructionType == IT_R || curInstructionType == IT_RW)
		{
			memVal = mem->ReadByte(address);
		}

		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			PollInterrupts();

			(this->*instructionJumpTable[curInstructionOpcode])();

			if (curInstructionType == IT_W)
			{
				mem->SetByte(address, memVal);
			}

			instructionFinished = true;
		}

		break;
	}
	case 4:
	{
		mem->SetByte(address, memVal);
		(this->*instructionJumpTable[curInstructionOpcode])();
		break;
	}
	case 5:
	{
		PollInterrupts();
		mem->SetByte(address, memVal);
		instructionFinished = true;
	}
	}
}

void CPU::AddressMode_REL()
{
	switch (instructionProgress)
	{
	case 1:
		PollInterrupts();
		(this->*instructionJumpTable[curInstructionOpcode])();
		memVal = mem->ReadByte(pc);
		curOperand1 = memVal;
		IncrementPC();
		if (!branchTaken)
		{
			instructionFinished = true;
		}
		break;
	case 2:
	{
		uint8_t pcLowByte = pc & 0x00FF; // PC low byte
		uint8_t oldPcLowByte = pcLowByte;
		bool negativeOffset = (memVal & 0x80);
		pcLowByte += memVal;
		pc = (pc & 0xFF00) | pcLowByte;

		if (!negativeOffset)
		{
			pageCrossed = (pcLowByte < oldPcLowByte);
		}
		else
		{
			pageCrossed = (pcLowByte > oldPcLowByte);
		}

		if (!pageCrossed)
		{
			instructionFinished = true;
		}
		
		break;
	}
	case 3:
	{
		PollInterrupts();
		pc += (memVal & 0x80) ? -0x0100 : 0x0100;
		instructionFinished = true;
		break;
	}
	}
}

void CPU::AddressMode_ABS()
{
	switch (instructionProgress)
	{
	case 1:
	{
		address = mem->ReadByte(pc);
		curOperand1 = address & 0x00FF;
		IncrementPC();
		break;
	}
	case 2:
	{
		address += (uint16_t)mem->ReadByte(pc) << 8;
		curOperand2 = (address >> 8) & 0x00FF;
		if (curInstructionOpcode == 0x4C) // JMP, special exception
		{
			PollInterrupts();
			pc = address;
			instructionFinished = true;
		}
		else
		{
			IncrementPC();
		}
		break;
	}
	case 3:
	{
		if (curInstructionType == IT_R || curInstructionType == IT_RW)
		{
			memVal = mem->ReadByte(address);
		}

		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			PollInterrupts();

			(this->*instructionJumpTable[curInstructionOpcode])();

			if (curInstructionType == IT_W)
			{
				mem->SetByte(address, memVal);
			}

			instructionFinished = true;
		}
		break;
	}
	case 4:
	{
		mem->SetByte(address, memVal);
		(this->*instructionJumpTable[curInstructionOpcode])();
		break;
	}
	case 5:
	{
		PollInterrupts();
		mem->SetByte(address, memVal);
		instructionFinished = true;
		break;
	}
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
	{
		address = mem->ReadByte(pc);
		curOperand1 = address & 0x00FF;
		IncrementPC();
		break;
	}
	case 2:
	{
		address += (uint16_t)mem->ReadByte(pc) << 8;
		curOperand2 = (address >> 8) & 0x00FF;
		uint8_t addressLowByte = address & 0x00FF;
		uint8_t oldAddressLowByte = addressLowByte;
		addressLowByte += *reg;
		address = (address & 0xFF00) | addressLowByte;
		pageCrossed = ((addressLowByte <= oldAddressLowByte) && (*reg != 0));
		if (pageCrossed == true)
		{
			int test = 0;
		}
		IncrementPC();
		break;
	}
	case 3:
	{
		memVal = mem->ReadByte(address);
		if (curInstructionType == IT_R && !pageCrossed)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();

			PollInterrupts();
			instructionFinished = true;
		}
		else if (pageCrossed)
		{
			address += 0x0100;
		}
		break;
	}
	case 4:
	{
		if (curInstructionType == IT_R || curInstructionType == IT_RW)
		{
			memVal = mem->ReadByte(address);
		}
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			PollInterrupts();

			(this->*instructionJumpTable[curInstructionOpcode])();

			if (curInstructionType == IT_W)
			{
				mem->SetByte(address, memVal);
			}

			instructionFinished = true;
		}
		break;
	}
	case 5:
	{
		mem->SetByte(address, memVal);
		(this->*instructionJumpTable[curInstructionOpcode])();
		break;
	}
	case 6:
	{
		PollInterrupts();
		mem->SetByte(address, memVal);
		instructionFinished = true;
		break;
	}
	}
}

void CPU::AddressMode_INDIR()
{
	switch (instructionProgress)
	{
	case 1:
	{
		pointer = mem->ReadByte(pc);
		curOperand1 = pointer & 0x00FF;
		IncrementPC();
		break;
	}
	case 2:
	{
		pointer += (uint16_t)mem->ReadByte(pc) << 8;
		curOperand2 = (pointer >> 8) & 0x00FF;
		IncrementPC();
		break;
	}
	case 3:
	{
		memVal = mem->ReadByte(pointer);
		break;
	}
	case 4:
	{
		PollInterrupts();
		uint8_t oldPointerLow = pointer & 0x00FF;
		pointer += 1;
		if ((pointer & 0x00FF) < oldPointerLow)
		{
			pointer -= 0x0100; // Pointer high byte remains same (page crossings not handled)
		}
		pc = (uint16_t)mem->ReadByte(pointer) << 8;
		pc += memVal;
		instructionFinished = true;
		break;
	}
	}
}

void CPU::AddressMode_INDIRX()
{
	switch (instructionProgress)
	{
	case 1:
	{
		pointer = mem->ReadByte(pc);
		curOperand1 = pointer & 0x00FF;
		IncrementPC();
		break;
	}
	case 2:
	{
		memVal = mem->ReadByte(pointer);
		pointer += x;
		pointer &= 0x00FF; // no page crossings
		break;
	}
	case 3:
	{
		address = mem->ReadByte(pointer);
		break;
	}
	case 4:
	{
		pointer += 1;
		pointer &= 0x00FF; // no page crossings
		address += (uint16_t)mem->ReadByte(pointer) << 8;
		break;
	}
	case 5:
	{
		if (curInstructionType == IT_R || curInstructionType == IT_RW)
		{
			memVal = mem->ReadByte(address);
		}
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			PollInterrupts();

			(this->*instructionJumpTable[curInstructionOpcode])();

			if (curInstructionType == IT_W)
			{
				mem->SetByte(address, memVal);
			}

			instructionFinished = true;
		}
		break;
	}
	case 6:
	{
		mem->SetByte(address, memVal);
		(this->*instructionJumpTable[curInstructionOpcode])();
		break;
	}
	case 7:
	{
		PollInterrupts();
		mem->SetByte(address, memVal);
		instructionFinished = true;
		break;
	}
	}
}

void CPU::AddressMode_INDIRY()
{
	switch (instructionProgress)
	{
	case 1:
	{
		pointer = mem->ReadByte(pc);
		curOperand1 = pointer & 0x00FF;
		IncrementPC();
		break;
	}
	case 2:
	{
		address = mem->ReadByte(pointer);
		break;
	}
	case 3:
	{
		pointer += 1;
		pointer &= 0x00FF; // no page crossings
		address += (uint16_t)mem->ReadByte(pointer) << 8;

		uint8_t addressLowByte = address & 0x00FF;
		uint8_t oldAddressLowByte = addressLowByte;
		addressLowByte += y;
		address = (address & 0xFF00) | addressLowByte;
		pageCrossed = (addressLowByte < oldAddressLowByte);
		break;
	}
	case 4:
	{
		memVal = mem->ReadByte(address);

		if (curInstructionType == IT_R && !pageCrossed)
		{
			(this->*instructionJumpTable[curInstructionOpcode])();

			PollInterrupts();
			instructionFinished = true;
		} else if (pageCrossed)
		{
			address += 0x0100;
		}

		break;
	}
	case 5:
	{
		if (curInstructionType == IT_R || curInstructionType == IT_RW)
		{
			memVal = mem->ReadByte(address);
		}
		if (curInstructionType == IT_R || curInstructionType == IT_W)
		{
			PollInterrupts();

			(this->*instructionJumpTable[curInstructionOpcode])();

			if (curInstructionType == IT_W)
			{
				mem->SetByte(address, memVal);
			}

			instructionFinished = true;
		}
		break;
	}
	case 6:
	{
		mem->SetByte(address, memVal);
		(this->*instructionJumpTable[curInstructionOpcode])();
		break;
	}
	case 7:
	{
		PollInterrupts();
		mem->SetByte(address, memVal);
		instructionFinished = true;
		break;
	}
	}
}