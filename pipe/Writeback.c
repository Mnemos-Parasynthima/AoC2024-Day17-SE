#include "instr.h"
#include "pipeline.h"
#include "machine.h"
#include "hardware.h"

extern machine_t guest;

extern int64_t writeWVal;

void writeback(writeback_reg_t* in) {
	// printf("\nWriteback for %d!\n", in->op);

	writeWVal = in->valRes;

	if (writeOut->writeSigs.writeEnable) {
		guest.proc->reg[in->dst] = writeWVal;

		// printf("Writing to register %d with value %ld\n", in->dst, writeWVal);
	}
}