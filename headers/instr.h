#ifndef _INSTR_H_
#define _INSTR_H_

#include <stdint.h>


#define PACK_ZZZ(za, zb, zc) ((za<<2) | (zb<<1) | (zc<<0))

#define GET_ZA(zzz) (((zzz) >> 2) & 0x1)
#define GET_ZB(zzz) (((zzz) >> 1) & 0x1)
#define GET_ZC(zzz) (((zzz) >> 0) & 0x1)

typedef enum opcode {
	OP_ADV,
	OP_BXL,
	OP_BST,
	OP_JNZ,
	OP_BXC,
	OP_OUT,
	OP_BDV,
	OP_CDV,
	OP_HLT,
	OP_NOP
} opcode_t;

typedef enum instrState {
	STAT_BUB = 0,
	STAT_OK,
	STAT_HLT
} stat_t;

extern opcode_t itable[];

void initItable();


uint8_t packBits(uint8_t opcode, uint8_t imm);

uint8_t extractBits(uint8_t src, unsigned lsb);

#endif