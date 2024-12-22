#ifndef _MACHINE_H
#define _MACHINE_H

#include <stdint.h>

#include <stdio.h>

#include "instr.h"
#include "pipeline.h"
#include "mem.h"

typedef enum registers {
	REG_A,
	REG_B,
	REG_C,
	REG_NONE // No register to use
} reg_t;

typedef struct proc {
	uint64_t reg[3]; // General purpose registers
	uint16_t ip; // Index into textSeg[]
	uint16_t sp; // Index into stackSeg[]
	uint8_t ZZZ; // Zero flags for the registers

	pipe_reg_t* fetchPipe; // The full pipeline register for Fetch
	pipe_reg_t* decodePipe; // The full pipeline register for Decode
	pipe_reg_t* executePipe; // The full pipeline register for Execute
	pipe_reg_t* memoryPipe; // The full pipeline register for Memory
	pipe_reg_t* writebackPipe; // The full pipeline register for Writeback

	stat_t status;
} proc_t;

typedef struct machine {
	char* name;
	proc_t* proc;
	mem_t* mem;
} machine_t;

void initMachine();

/**
 * Dumps processor and memory contents into a file.
 */
void coredump();

#endif