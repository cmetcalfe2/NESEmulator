#pragma once

#include "Mapper.h"


class PPU;

class Memory
{
public:
	Memory();
	~Memory();

	void Clear();

	void SetMapper(Mapper* m) { mapper = m; };
	void SetPPU(PPU* p) { ppu = p; };

	static const int MEMORY_AMOUNT = 0xFFFF;
	uint8_t memory[MEMORY_AMOUNT];
	uint16_t memoryMap[MEMORY_AMOUNT]; // maps mirrored memory locations to their real locations

	uint8_t ReadByte(uint16_t addr);
	uint16_t ReadWord(uint16_t addr);

	void SetByte(uint16_t addr, uint8_t val);
	void SetWord(uint16_t addr, uint16_t val);

	uint8_t ReadIORegister(uint16_t addr);
	void SetIORegister(uint16_t addr, uint8_t val);

private:
	void InitMemoryMap();

	Mapper* mapper;
	PPU* ppu;
};

