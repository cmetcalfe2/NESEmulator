#pragma once
#ifndef PPU_HEADER
#define PPU_HEADER

#include <stdint.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include "Memory.h"
#include "Mapper.h"
#include "Interrupts.h"

class PPU;
typedef void (PPU::*CycleFunction)();


enum NametableMirroringMode
{
	MIRRORING_HORIZONTAL = 0,
	MIRRORING_VERTICAL = 1,
	MIRRORING_SINGLESCREEN = 2,
	MIRRORING_4SCREEN = 3
};

struct NESPixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class PPU
{
public:
	PPU(Memory* memory, Interrupts* interrupts);
	~PPU();

	void Init();
	void Reset();

	void Cycle();

	void SetMapper(Mapper* mapper);

	uint8_t ReadVRAMByte(uint16_t address);
	void SetVRAMByte(uint16_t address, uint8_t val);

	void SetOAMByte(uint8_t val);

	// Dumps VRAM into file to be read by hex editor
	void DumpVRAM();

	uint16_t GetRealVRAMAddress(uint16_t address);

	// Registers
	uint8_t ReadPPUSTATUS();
	uint8_t ReadOAMDATA();
	uint8_t ReadPPUDATA();

	void SetPPUCTRL(uint8_t val);
	void SetPPUMASK(uint8_t val);
	void SetOAMADDR(uint8_t val);
	void SetOAMDATA(uint8_t val);
	void SetPPUSCROLL(uint8_t val);
	void SetPPUADDR(uint8_t val);
	void SetPPUDATA(uint8_t val);

	// Mirroring
	void SetMirrorMode(NametableMirroringMode mode);

	void* GetPixelBuffer();
	bool switchPixelBuffers = false;

private:
	void InitVRAMMap();
	void LoadPaletteFile();

	void SpriteEvaluationCycle();

	// Cycle functions
	void FetchNTByte();
	void FetchATByte();
	void FetchLowBGByte();
	void FetchHighBGByte();
	void FeedBGRegisters();

	void SetVBlankStartedFlag();
	void ClearFlags();

	void IncCurVRAMAddrHorizontal();
	void IncCurVRAMAddrVertical();
	void CopyVRAMAddrHorizontal();
	void CopyVRAMAddrVertical();

	void RenderPixel();

	void ShiftBGRegisters();

	void InitCycleFunctionVectors();

	// Array of vectors of functions to be ran at each cycle
	CycleFunction cycleFunctions[341][262][10];
	int numFunctionsInCycle[341][262];

	// Sprite evaluation functions
	bool IsSpriteYInRange();
	void LoadSpritesIntoRegisters();

	// Palette
	NESPixel nesPalette[64];

	// Rendered pixel buffers
	static const int SCREEN_WIDTH = 256;
	static const int SCREEN_HEIGHT = 240;
	uint8_t renderedPixelBuffers[2][SCREEN_WIDTH * SCREEN_HEIGHT][4];
	int activePixelBuffer = 0;
	int renderedPixelBuffer = 0;

	// Memory
	uint8_t vram[0x4000];
	uint16_t vramMap[0x4000];

	uint8_t primaryOAM[256];
	uint8_t secondaryOAM[32];

	Memory* mem;
	Mapper* mapper;
	Interrupts* interrupts;

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

	// PPUCTRL
	uint16_t baseNametableAddress = 0x2000;
	uint8_t ppuDataAddressIncrement = 1;
	uint16_t spritePatternTableAddr = 0x0000;
	uint16_t bgPatternTableAddr = 0x0000;
	bool eightBySixteenSpriteMode = false;
	uint8_t spriteHeight;
	bool ppuSlave = false;
	bool nmiEnabled = false;

	// PPUMASK
	bool greyscaleEnabled = false;
	bool leftMostBGEnabled = false;
	bool leftMostSpritesEnabled = false;
	bool showBG = false;
	bool showSprites = false;
	bool emphasizeRed = false;
	bool emphasizeGreen = false;
	bool emphasizeBlue = false;

	bool renderingEnabled = true;

	// PPUSTATUS
	uint8_t leastSigBitsLastWrittenToReg = 0x00;
	uint8_t spriteOverflowFlag;
	uint8_t spriteZeroHitFlag;
	uint8_t vblankStartedFlag;

	// PPUSCROLL
	bool ppuRegSecondWrite = false;

	// PPUDATA
	uint8_t ppuDataReadLatch = 0x00;

	// Internal scroll registers
	uint16_t curVRAMAddr = 0x0000;
	uint16_t tempVRAMAddr = 0x0000;
	uint8_t fineXScroll = 0x00;

	// Scroll register bitmasks
	uint16_t bitMaskFineY = 0x7000;
	uint16_t bitMaskCoarseY = 0x03E0;
	uint16_t bitMaskCoarseX = 0x001F;

	// Nametable region maps
	uint16_t nametableL1Map[0x0400];
	uint16_t nametableL2Map[0x0400];
	uint16_t nametableL3Map[0x0400];
	uint16_t nametableL4Map[0x0400];

	// Internal background pixel registers

	/* 2 16-bit shift registers - These contain the bitmap data for two tiles. Every 8 cycles,
	the bitmap data for the next tile is loaded into the upper 8 bits of this shift register.
	Meanwhile, the pixel to render is fetched from one of the lower 8 bits.
	[0] contains low bit and [1] contains high bits */
	uint16_t bgPatternRegisters[2];

	/* 2 8-bit shift registers - These contain the palette attributes for the lower 8 pixels of the 16-bit shift register.
	These registers are fed by a latch which contains the palette attribute for the next tile.
	Every 8 cycles, the latch is loaded with the palette attribute for the next tile.
	[0] contains low bit and [1] contains high bits  */
	uint16_t bgAttributeRegisters[2];

	uint8_t ntByte = 0x0000;

	uint8_t atByte = 0x0000;
	uint8_t atLowLatch = 0x0000;
	uint8_t atHighLatch = 0x0000;

	uint8_t bgLowByte = 0x00;
	uint8_t bgHighByte = 0x00;

	// Internal sprite registers
	uint8_t spriteBitmaps[8][2]; // 2 bits for each pixel, 8 sprites [0] is low bit and [1] is high bit
	uint8_t spriteAttributes[8];
	uint8_t spriteXPositionCounters[8];
	uint8_t spriteIndices[8];

	uint8_t baseOAMAddr = 0x00;

	bool copyingSprite = false;
	uint8_t secondaryOAMIndex = 0;
	int numSpritesFound = 0;
	bool allSpritesEvaluated = false;

	uint8_t spriteEvalM = 0;
	uint8_t spriteEvalN = 0;
	uint8_t spriteEvalO = 0;

	uint8_t spriteY = 0;


};

#endif