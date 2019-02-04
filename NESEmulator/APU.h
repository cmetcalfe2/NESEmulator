#pragma once

#include <stdint.h>
#include "Simple_Apu.h"
#include "Sound_Queue.h"
#include "Memory.h"


class APU
{
public:
	APU(Memory* m);
	~APU();

	void RunForOneFrame();

	uint8_t ReadRegister(uint16_t address);
	void WriteRegister(uint16_t address, uint8_t val);

	static int DMCReader(void*, cpu_addr_t address);
	int DMCRead(void*, uint16_t address);

private:
	void PlaySamples(const blip_sample_t* samples, long count);

	static APU* instance;

	Simple_Apu* simpleAPU;
	Sound_Queue* soundQueue;

	Memory* mem;
};

