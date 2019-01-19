#include "Memory.h"
#include "PPU.h"
#include <stdint.h>


Memory::Memory()
{
	Clear();
	InitMemoryMap();
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

void Memory::InitMemoryMap()
{
	for (uint16_t i = 0x0000; i < MEMORY_AMOUNT; i++)
	{
		memoryMap[i] = i;
	}

	// System memory mirroring
	uint16_t systemMirroringLocations[3] = { 0x0800, 0x1000, 0x1800 };
	for (int i = 0; i < 3; i++)
	{
		for (uint16_t j = 0x0000; j <= 0x07FF; j++)
		{
			memoryMap[systemMirroringLocations[i] + j] = j;
		}
	}

	// PPU I/O register mirroring
	for (uint16_t i = 0x2008; i <= 0x3FFF; i++)
	{
		memoryMap[i] = 0x2000 + (i % 8);
	}
}

uint8_t Memory::ReadByte(uint16_t addr)
{
	uint16_t realAddress = memoryMap[addr];

	if ((realAddress >= 0x2000 && realAddress <= 0x2007) || (realAddress >= 0x4000 && realAddress <= 0x401F))
	{
		// IO register
		return ReadIORegister(addr);
	}	
	else if (realAddress >= 0x8000)
	{
		// Read from PRG ROM/RAM
		return mapper->ReadPRGByte(realAddress);
	}
	else
	{
		return memory[realAddress];
	}
}

uint8_t Memory::ReadIORegister(uint16_t addr)
{
	switch (addr)
	{
	case 0x2002:
		// PPUSTATUS
		break;
	case 0x2004:
		// OAMDATA
		break;
	case 0x2007:
		// PPUDATA
		break;
	default:
		return 0x0000;
		break;
	}
}

uint16_t Memory::ReadWord(uint16_t addr)
{
	return ReadByte(addr) + (uint16_t(ReadByte(addr + 1)) << 8);
}

void Memory::SetByte(uint16_t addr, uint8_t val)
{
	uint16_t realAddress = memoryMap[addr];

	if ((realAddress >= 0x2000 && realAddress <= 0x2007) || (realAddress >= 0x4000 && realAddress <= 0x401F))
	{
		// IO register
		
	}
	else if (realAddress >= 0x8000)
	{
		// Write to PRG RAM
		mapper->WritePRGByte(realAddress, val);
	}
	else
	{
		memory[realAddress] = val;
	}
}

void Memory::SetIORegister(uint16_t addr, uint8_t val)
{
	switch (addr)
	{
	case 0x2000:
		// PPUCTRL
		break;
	case 0x2001:
		// PPUMASK
		break;
	case 0x2003:
		// OAMADDR
		break;
	case 0x2004:
		// OAMDATA
		break;
	case 0x2005:
		// PPUSCROL
		break;
	case 0x2006:
		// PPUADDR
		break;
	case 0x2007:
		// PPUDATA
	case 0x4014:
		// OAMDMA
		break;
	}
}

void Memory::SetWord(uint16_t addr, uint16_t val)
{
	SetByte(addr, val & 0xFF);
	SetByte(addr + 1, (val >> 8));
}