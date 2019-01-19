#pragma once
#include "Mapper.h"


enum PRGBankMode
{
	PRGMODE_BOTH = 0,
	PRGMODE_SWITCHLAST = 1,
	PRGMODE_SWITCHFIRST = 2
};

class MapperMMC1 :
	public Mapper
{
public:
	MapperMMC1(PPU* p);
	~MapperMMC1();

	void WritePRGByte(uint32_t address, uint8_t val) override;
	void OnInstructionFinished() override;

	void SetControlReg();
	void SetCHR1Reg();
	void SetCHR2Reg();
	void SetPRGReg();

	void SwitchPRGBank();

private:
	int writeCounter = 0;
	bool writesIgnored = false;

	PRGBankMode prgMode = PRGMODE_SWITCHFIRST;

	// Registers
	uint8_t regShift;
	uint8_t regControl;
	uint8_t regCHR1;
	uint8_t regCHR2;
	uint8_t regPRG;

	// Register bit masks
	 
};

