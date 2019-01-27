#include "MapperNROM.h"
#include "PPU.h"


MapperNROM::MapperNROM(PPU* p) : Mapper(p)
{
}


MapperNROM::~MapperNROM()
{
}


void MapperNROM::OnPRGROMLoaded()
{
	prgBankOffset1 = 0x0000;
	if (numPRGBanks == 1 || numPRGBanks == 0)
	{
		prgBankOffset2 = 0x0000;
	}
	else
	{
		prgBankOffset2 = 0x4000;
	}
}

void MapperNROM::OnCHRROMLoaded()
{
	chrBankOffset1 = 0;
}
