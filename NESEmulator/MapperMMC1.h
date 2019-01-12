#pragma once
#include "Mapper.h"
class MapperMMC1 :
	public Mapper
{
public:
	MapperMMC1();
	~MapperMMC1();

	void OnInstructionFinished() override;

private:
	bool writesIgnored = false;
};

