#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "machine.h"
#include "hardware.h"
#include "hazard.h"


opcode_t itable[9];

machine_t guest;
extern uint16_t fetchIP;


void loadProgram(const char* filename) {
	FILE* file = fopen(filename, "r");

	char* line = NULL;
	size_t n;

	char* token = NULL;

	for (int i = 0; i < 3; i++) {
		getline(&line, &n, file);

		token = strtok(line, " ");
		strtok(NULL, " ");
		token = strtok(NULL, " ");

		guest.proc->reg[i] = (uint64_t) atoi(token);
	}

	// Skip empty line
	getline(&line, &n, file);

	// Get line containing code
	getline(&line, &n, file);

	strtok(line, " ");
	token = strtok(NULL, " ");

	// token has the actual code, delim'd by ","
	char* saveptr = NULL;
	// opcode
	char* tok = strtok_r(token, ",", &saveptr);

	int i = 0;
	while (tok != NULL) {
		// first tok is the op
		// second is the operand/imm

		uint8_t opcode = atoi(tok);
		uint8_t imm = 0;

		// imm
		tok = strtok_r(NULL, ",", &saveptr);
		if (tok != NULL) {
			imm = atoi(tok);
		}
		uint8_t insn = packBits(opcode, imm);

		guest.mem->textSeg[i++] = insn;

		// opcode
		tok = strtok_r(NULL, ",", &saveptr);
	}

	fclose(file);
}

int run(const char* maxCycles) {
	int MAX_CYCLES = 200;

	if (maxCycles != NULL) {
		MAX_CYCLES = atoi(maxCycles);
	}

	guest.proc->ip = 0;
	guest.proc->sp = 0;

	bool regAZero = false, regBZero = false, regCZero = false;
	if (guest.proc->reg[0] == 0) regAZero = true;
	if (guest.proc->reg[1] == 0) regBZero = true;
	if (guest.proc->reg[2] == 0) regCZero = true;

	guest.proc->ZZZ = PACK_ZZZ(regAZero, regBZero, regCZero);

	// printf("ZZZ register: %x\n", guest.proc->ZZZ);

	pipe_reg_t** pipes[] = { &fetchP, &decodeP, &execP, &memP, &writeP };

	uint64_t sizes[5] = {
		sizeof(fetch_reg_t), sizeof(decode_reg_t),
		sizeof(exec_reg_t), sizeof(mem_reg_t),
		sizeof(writeback_reg_t)
	};

	for (int i = 0; i < 5; i++) {
		*pipes[i] = (pipe_reg_t*) calloc(1, sizeof(pipe_reg_t));

		(*pipes[i])->size = sizes[i];
		(*pipes[i])->in = (pipe_reg_u_t) calloc(1, sizes[i]);
		(*pipes[i])->out = (pipe_reg_u_t) calloc(1, sizes[i]);
		(*pipes[i])->ctrl = CTRL_BUBBLE;
	}

	fetchOut->nextIP = guest.proc->ip;
	fetchOut->status = STAT_OK;
	// printf("Init fetchOut->nextIP to %d\n\n", fetchOut->nextIP);

	uint64_t numInstr = 0;
	// uint64_t numInstrUntilRegA0 = 0;

	do {
		// sleep(1);
		// printf("\n\nCYCLE %ld.....\n", numInstr);

		writeback(writeOut);
		memory(memOut, writeIn);
		execute(execOut, memIn);
		decode(decodeOut, execIn);
		fetch(fetchOut, decodeIn);
		// printf("\n");

		fetchIn->nextIP = fetchIP;

		guest.proc->status = writeOut->status;

		// printf("decodeOut->op: %d; execOut->op: %d; memIn->cond: %d", decodeOut->op, execOut->op, memIn->cond);
		handleHazards(decodeOut->op, execOut->op, memIn->cond);

		guest.proc->ip = fetchIP;

		// printf("\n\nProcessor state:\nIP: 0x%x\nSP: 0x%x\nregister A: %ld\nregister B: %ld\nregister C: %ld\nstatus: %d\nZZZ: 0x%x\n", 
		// 	guest.proc->ip, guest.proc->sp, guest.proc->reg[0], guest.proc->reg[1], guest.proc->reg[2], guest.proc->status, guest.proc->ZZZ);

		for (int i = 0; i < 5; i++) {
			pipe_reg_t* pipe = *pipes[i];

			// if (i == 0) printf("guest.proc->fetchP: ");
			// else if (i == 1) printf("guest.proc->decodeP: ");
			// else if (i == 2) printf("guest.proc->execP: ");
			// else if (i == 3) printf("guest.proc->memP: ");
			// else printf("guest.proc->writeP: ");

			switch (pipe->ctrl) {
				case CTRL_LOAD:
					// printf("CTRL_LOAD\n");
					memcpy(pipe->out.generic, pipe->in.generic, pipe->size);
					break;
				case CTRL_ERROR:
					guest.proc->status = STAT_HLT;
				case CTRL_BUBBLE:
					// printf("CTRL_ERROR | CTRL_BUBBLE\n");
					memset(pipe->out.generic, 0, pipe->size);
					break;
				case CTRL_STALL:
					break;
			}
		}

		numInstr++;

		// if (guest.proc->reg[0] != 0) numInstrUntilRegA0++;

	} while ((guest.proc->status == STAT_OK || guest.proc->status == STAT_BUB) && numInstr < MAX_CYCLES);
	
	// printf("Num instructions with reg A != 0: %ld\n", numInstrUntilRegA0);

	return 0;
}

int main(int argc, char const* argv[]) {
	// ./se [filename] [?max_cycles_allowed]

	if (argc < 2) exit(1);

	printf("Initializing machine...\n");
	initMachine();
	printf("Initializing instruction table...\n");
	initItable();

	printf("Loading program...\n");
	loadProgram(argv[1]);
	printf("Program loaded! Running...\n");
	int ret = run(argv[2]);

	printf("Dumping core\n");
	coredump();

	return ret;
}