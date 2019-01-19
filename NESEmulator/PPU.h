#pragma once
#include <stdint.h>
#include "Memory.h"
#include "Mapper.h"


class PPU
{
public:
	PPU(Memory* memory);
	~PPU();

	void Cycle();

	void SetMapper(Mapper* mapper);

	uint8_t ReadVRAMByte(uint16_t address);
	void SetVRAMByte(uint16_t address, uint8_t val);

	// Registers
	void SetPPUCTRL(uint8_t val) {};

private:
	void InitVRAMMap();

	void SpriteEvaluationCycle();

	uint8_t vram[0x4000];
	uint16_t vramMap[0x4000];

	Memory* mem;
	Mapper* mapper;

	// Registers
	uint8_t ppuCtrl;
	uint8_t ppuMask;
	uint8_t ppuStatus;
	uint8_t oamAddr;
	uint8_t oamData;
	uint8_t ppuScroll;
	uint8_t ppuAddr;
	uint8_t ppuData;
	uint8_t oamDma;

	bool oddFrame = false;
	int curPixel;
	int curScanline;

	bool renderingEnabled = true;
};

