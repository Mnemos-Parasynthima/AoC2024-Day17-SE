#include <assert.h>

#include "instr.h"

static void initEntry(opcode_t op, unsigned idx) {
	assert(-1 == itable[idx]);
	itable[idx] = op;
}

void initItable() {
	for (int i = 0; i < 10; i++) itable[i] = -1;
	initEntry(OP_ADV, 0x0);
	initEntry(OP_BXL, 0x1);
	initEntry(OP_BST, 0x2);
	initEntry(OP_JNZ, 0x3);
	initEntry(OP_BXC, 0x4);
	initEntry(OP_OUT, 0x5);
	initEntry(OP_BDV, 0x6);
	initEntry(OP_CDV, 0x7);
	initEntry(OP_HLT, 0x8);
	initEntry(OP_NOP, 0x9);
}

uint16_t fetchIP;
uint64_t writeWVal;

uint8_t packBits(uint8_t opcode, uint8_t imm) {
	return (opcode << 4 | imm);
}

uint8_t extractBits(uint8_t src, unsigned lsb) {
	return (uint8_t) ((src >> lsb) & ((1 << 4) - 1));
}