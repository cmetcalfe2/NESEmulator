#include "MapperMMC1.h"



MapperMMC1::MapperMMC1()
{
	prgBankOffset1 = 0x0000;
	prgBankOffset2 = (0x4000 * 15) - 0x4000;

}


MapperMMC1::~MapperMMC1()
{
}

void MapperMMC1::OnInstructionFinished()
{
	writesIgnored = false;
}
