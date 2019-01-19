#include "Mapper.h"
#include "PPU.h"

Mapper::Mapper(PPU* p)
{
	ppu = p;
	ppu->Cycle();
}


Mapper::~Mapper()
{
}

void Mapper::LoadPRGROM(std::ifstream& inputStream, uint8_t numBanks)
{
	prgRom.resize(numBanks * 0x4000);
	for (uint32_t i = 0; i < (numBanks * 0x4000); i++)
	{
		prgRom[i] = inputStream.get();
	}
	OnRPRGROMLoaded();
}

void Mapper::WritePRGByte(uint32_t address, uint8_t val)
{
	SetPRGByte(address, val);
}

uint8_t Mapper::ReadPRGByte(uint32_t address)
{
	uint32_t bankOffset = (address < 0xC000) ? prgBankOffset1 : prgBankOffset2 - 0x4000;
	return prgRom[address - 0x8000 + bankOffset];
}

void Mapper::SetPRGByte(uint32_t address, uint8_t val)
{
	uint32_t bankOffset = (address < 0xC000) ? prgBankOffset1 : prgBankOffset2 - 0x4000;
	prgRom[address - 0x8000 + bankOffset] = val;
}
