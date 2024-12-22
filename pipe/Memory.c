#include "instr.h"
#include "pipeline.h"
#include "machine.h"
#include "hardware.h"

extern machine_t guest;

extern void copyMemCtrlSigs(mem_ctrl_t* dest, mem_ctrl_t* src);
extern void copyWriteCtrlSigs(writeback_ctrl_t* dest, writeback_ctrl_t* src);


void memory(mem_reg_t* in, writeback_reg_t* out) {
	// printf("\nMemory for %d!\n", in->op);

	copyWriteCtrlSigs(&out->writeSigs, &in->writeSigs);
	copyMemCtrlSigs(&memOut->memSigs, &in->memSigs);

	out->op = in->op;
	out->dst = in->dst;
	out->valRes = in->valRes;
	out->status = in->status;
	
	if (in->memSigs.dmemWrite) {
		// printf("Writing to stack sp %d with value of %ld\n", guest.proc->sp, in->valRes);
		// Cast to uint8_t since the out instruction only results in writing to memory
		// And it only results in a small value due to mod 8
		// Small enough that it can be in a byte
		dmem(guest.proc->sp, true, (uint8_t) in->valRes, false, 0);
	}

	// printf("Outgoing destination register: %d\n", out->dst);
}