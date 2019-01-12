#pragma once
#include <stdint.h>
#include <vector>
#include <fstream>

class Mapper
{
public:
	Mapper();
	~Mapper();

	void LoadPRGROM(std::ifstream& inputStream, uint8_t numBanks); // Loads PRG ROM, caller should check that stream pointer is at beginning of PRG ROM

	virtual void WritePRGByte(uint32_t address, uint8_t val);
	virtual uint8_t ReadPRGByte(uint32_t address);

	virtual void OnInstructionFinished() {}; // Used by mappers such as MMC1 to reset write ignore
	
protected:
	void SetPRGByte(uint32_t address, uint8_t val);

	std::vector<uint8_t> prgRom;
	std::vector<uint8_t> chrRom;

	uint32_t prgBankOffset1; // Address of PRG ROM accessed via $8000-$BFFF
	uint32_t prgBankOffset2; // Address of PRG ROM accessed via
};

