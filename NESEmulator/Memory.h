#pragma once
#include <stdint.h>

class Memory
{
public:
	Memory();
	~Memory();

	void Clear();

	static const int MEMORY_AMOUNT = 0xFFFF;
	uint8_t memory[MEMORY_AMOUNT];
	uint16_t memoryMap[MEMORY_AMOUNT]; // maps mirrored memory locations to their real locations

	uint8_t ReadByte(uint16_t addr);
	uint16_t ReadWord(uint16_t addr);

	void SetByte(uint16_t addr, uint8_t val);
	void SetWord(uint16_t addr, uint16_t val);

private:
	void InitMemoryMap();
};

