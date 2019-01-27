#pragma once
#include "Mapper.h"
class MapperNROM :
	public Mapper
{
public:
	MapperNROM(PPU* ppu);
	~MapperNROM();

	void OnPRGROMLoaded() override;
	void OnCHRROMLoaded() override;
};

