#include "APU.h"

APU* APU::instance;


APU::APU(Memory* m)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}

	simpleAPU = new Simple_Apu();
	mem = m;

	simpleAPU->sample_rate(44100);
	simpleAPU->dmc_reader(APU::DMCReader);

	soundQueue = new Sound_Queue();
	soundQueue->init(44100);
}


APU::~APU()
{
	delete simpleAPU;
	simpleAPU = NULL;

	delete soundQueue;
	soundQueue = NULL;

	instance = NULL;
}


void APU::RunForOneFrame()
{
	simpleAPU->end_frame();

	int const bufSize = 2048;
	static blip_sample_t buf[bufSize];

	// Play whatever samples are available
	long count = simpleAPU->read_samples(buf, bufSize);
	PlaySamples(buf, count);
}

uint8_t APU::ReadRegister(uint16_t address)
{
	return 0x00;
}

void APU::WriteRegister(uint16_t address, uint8_t val)
{
	simpleAPU->write_register(address, val);
}

int APU::DMCReader(void* arg, cpu_addr_t address)
{
	return instance->DMCRead(arg, address);
}

int APU::DMCRead(void*, uint16_t address)
{
	return mem->ReadByte(address);
}

void APU::PlaySamples(const blip_sample_t* samples, long count)
{
	soundQueue->write(samples, count);
}
