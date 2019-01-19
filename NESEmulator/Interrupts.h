#pragma once

enum InterruptType
{
	INTERRUPT_NONE,
	INTERRUPT_BRK,
	INTERRUPT_NMI,
	INTERRUPT_IRQ,
	INTERRUPT_RST
};

class Interrupts
{
public:
	void AssertIntertupt(InterruptType interruptType);
	InterruptType PollEdgeDetectors();
	void PollInterruptLines();
	void InterruptHijacking(InterruptType& interruptBeingHandled);

private:
	void Reset();

	bool rstPending = false;

	bool nmiEdgeDetectorPowered = false;
	bool irqEdgeDetectorPowered = false;

	bool nmiLinePowered = false;
	bool irqLinePowered = false;
};

