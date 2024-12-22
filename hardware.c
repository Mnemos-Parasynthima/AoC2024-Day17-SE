#include <math.h>

#include "machine.h"
#include "mem.h"
#include "hardware.h"

extern machine_t guest;


void alu(uint64_t valA, uint64_t valB, alu_op_t op, bool setZZZ, uint64_t* valRes, bool* cond, uint8_t* zzz) {
	uint64_t res = 0xFEEDFAEDDEADDEED;

	switch (op)	{
		case ALU_DIV:
			res = valA >> valB;
			break;
		case ALU_XOR:
			res = valA ^ valB;
			break;
		case ALU_MOD:
			res = valA % 8; // Hardcoded, allow valB for future
			break;
		default:
			break;
	}

	*valRes = res;

	if (setZZZ && res == 0) {
		// printf("ALU res is 0 and setZZZ is true, setting flags\n");

		bool za = GET_ZA(guest.proc->ZZZ);
		bool zb = GET_ZB(guest.proc->ZZZ);
		bool zc = GET_ZC(guest.proc->ZZZ);

		if (memIn->dst == REG_A) {
			za = true;
			// printf("memIn->dst is reg A\n");
		} else if (memIn->dst == REG_B) zb = true;
		else if (memIn->dst == REG_C) zc = true;
	
		*zzz = PACK_ZZZ(za, zb, zc);
	}

	// Hardcode for now
	if (GET_ZA(*zzz) == 1) {
		// printf("if za is true (reg a holds 0)\n");
		*cond = true;
	} else *cond = false;
}

void dmem(uint16_t dmemIdx, bool dmemWrite, uint8_t dmemWriteVal, bool dmemRead, uint8_t* dmemReadVal) {
	if (dmemWrite) memWriteByte(dmemIdx, dmemWriteVal);
}

void imem(uint16_t imemIdx, uint8_t* imemRet) {
	*imemRet = (uint8_t) memReadByte(imemIdx);

	// printf("hardware.c::imem: Read at idx %d -> 0x%x\n", imemIdx, *imemRet);
}

void regfile(uint8_t src1, uint8_t src2, uint8_t dst, uint64_t valWrite, bool writeEnable, uint64_t* valOutA, uint64_t* valOutB) {
	if (writeEnable) {
		guest.proc->reg[dst] = valWrite;
	}

	*valOutA = guest.proc->reg[src1];
	*valOutB = guest.proc->reg[src2];
}