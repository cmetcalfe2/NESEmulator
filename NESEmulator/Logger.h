#pragma once
#include <stdint.h>
#include <cstdio>
#include <string>

class Logger
{
public:
	static void Init();
	static void LogCPUInstruction(uint16_t pc, uint8_t opcode, uint8_t operand1, uint8_t operand2, uint16_t address, uint8_t a, uint8_t x, uint8_t y, uint8_t sp, uint8_t ps, unsigned long cycles, std::string instructionName, bool hasTwoOperands);
};

