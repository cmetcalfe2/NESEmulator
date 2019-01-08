#include "Memory.h"



Memory::Memory()
{
	Clear();
}


Memory::~Memory()
{
}

void Memory::Clear()
{
	for (int i = 0; i < MEMORY_AMOUNT; i++)
	{
		memory[i] = 0;
	}
}

uint8_t Memory::ReadByte(uint16_t addr)
{
	return memory[addr];
}

uint16_t Memory::ReadWord(uint16_t addr)
{
	return ReadByte(addr) + (uint16_t(ReadByte(addr + 1)) << 8);
}

void Memory::SetByte(uint16_t addr, uint8_t val)
{
	memory[addr] = val;
}

void Memory::SetWord(uint16_t addr, uint16_t val)
{
	SetByte(addr, val & 0xFF);
	SetByte(addr + 1, (val >> 8));
}
