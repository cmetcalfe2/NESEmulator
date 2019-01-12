#pragma once
#include "ROMTest.h"
class CPUTestBlargg :
	public ROMTest
{
public:
	CPUTestBlargg();
	~CPUTestBlargg();

protected:
	void PostCycle() override;
};

