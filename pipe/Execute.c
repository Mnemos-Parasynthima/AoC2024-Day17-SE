#include "instr.h"
#include "pipeline.h"
#include "machine.h"
#include "hardware.h"

extern machine_t guest;

extern void copyMemCtrlSigs(mem_ctrl_t* dest, mem_ctrl_t* src);
extern void copyWriteCtrlSigs(writeback_ctrl_t* dest, writeback_ctrl_t* src);

void execute(exec_reg_t* in, mem_reg_t* out) {
	// printf("\nExecuting OP %d!\n", in->op);

	copyMemCtrlSigs(&out->memSigs, &in->memSigs);
	copyWriteCtrlSigs(&out->writeSigs, &in->writeSigs);

	out->seqIP = in->seqIP;
	out->dst = in->dst;
	out->op = in->op;

	uint64_t aluValA, aluValB;

	if (in->execSigs.valASel) aluValA = in->valA;
	else aluValA = in->immA;

	if (in->execSigs.valBSel) aluValB = in->valB;
	else aluValB = in->immB;

	alu(aluValA, aluValB, in->aluOP, in->execSigs.setZZZ, &(out->valRes), &(out->cond), &(guest.proc->ZZZ));

	// printf("aluValA: %ld, aluValB: %ld, valRes: %ld\n", aluValA, aluValB, out->valRes);
	// printf("Outgoing destination register: %d; Incoming destination regsiter: %d\n", out->dst, in->dst);

	out->status = in->status;
}