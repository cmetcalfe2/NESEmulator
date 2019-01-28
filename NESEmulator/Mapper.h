#pragma once
#include <stdint.h>
#include <vector>
#include <fstream>

class PPU;

class Mapper
{
public:
	Mapper(PPU* ppu);
	~Mapper();

	void LoadPRGROM(std::ifstream& inputStream, uint8_t numBanks); // Loads PRG ROM, caller should check that stream pointer is at beginning of PRG ROM
	virtual void OnPRGROMLoaded() = 0;

	void LoadCHRROM(std::ifstream& inputStream, uint8_t numBanks); // Loads CHR ROM, caller should check that stream pointer is at beginning of CHR ROM
	virtual void OnCHRROMLoaded() = 0;

	virtual void WritePRGByte(uint32_t address, uint8_t val);
	virtual uint8_t ReadPRGByte(uint32_t address);

	virtual void WriteCHRByte(uint32_t address, uint8_t val);
	virtual uint8_t ReadCHRByte(uint32_t address);

	virtual void OnInstructionFinished() {}; // Used by mappers such as MMC1 to reset write ignore
	
protected:
	void SetPRGByte(uint32_t address, uint8_t val);
	void SetCHRByte(uint32_t address, uint8_t val);

	uint8_t prgRom[0x80000]; // Max size = 512KB
	uint8_t chrRom[0x80000];

	uint8_t numPRGBanks = 0;
	uint32_t prgBankOffset1; // Address of PRG ROM accessed via $8000-$BFFF
	uint32_t prgBankOffset2; // Address of PRG ROM accessed via $C000-$FFFF

	uint8_t numCHRBanks = 0;
	uint32_t chrBankOffset1; // Address of CHR ROM accessed via $0000-$0FFF
	uint32_t chrBankOffset2; // Address of CHR ROM accessed via $1000-$1FFF

	bool chrRAMEnabled = false;

	PPU* ppu;
};

