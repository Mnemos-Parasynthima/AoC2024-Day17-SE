#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "machine.h"

extern machine_t guest;

void initMachine() {
	guest.name = "advent"; // Maybe should name it to chronospatial or something, who cares :p
	guest.proc = malloc(sizeof(proc_t));

	for (int i = 0; i < 3; i++) {
		guest.proc->reg[i] = 0;
	}

	guest.mem = malloc(sizeof(mem_t));
	guest.mem->TEXT_SEG_MAGIC = 0xFAED;
	guest.mem->STACK_SEG_MAGIC = 0xDEAD; // Dead just like me after finishing this

	for (int i = 0; i < TEXT_SEG_MAX; i++) {
		guest.mem->textSeg[i] = packBits(OP_HLT, 0);
	}
}

void coredump() {
	// A proper programmer would check for returns in malloc and I/O
	// but I'm too lazy to do error handling
	// Plus, is the coredump reallyyyy necessary

	char* filename = malloc(strlen(guest.name) + 15);
	sprintf(filename, "%s-coredump.dump", guest.name);

	FILE* file = fopen(filename, "w");
	free(filename);

	char* contents = malloc(96);

	sprintf(contents, "**** COREDUMP ****\nR1: %ld\nR2: %ld\nR3: %ld\n**\nIP: 0x%x\n**\nSP: 0x%x\n",
		guest.proc->reg[0], guest.proc->reg[1], guest.proc->reg[2],
		guest.proc->ip, guest.proc->sp);

	fputs(contents, file);
	printf("Processor dumped...\n");
	free(contents);

	contents = malloc((TEXT_SEG_MAX / 16) * 82);

	sprintf(contents, "** STACK **\n%p:\n", guest.mem->stackSeg);

	// add stack contents
	// each line will have 16 bytes
	// so for 1024 bytes, need 1024 / 16 = 64 lines
	// line: 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f
	for (int line = 0; line < STACK_SEG_MAX / 16; line++) {
		char* bytesLine = malloc(80 + 2);

		char bytes[16] = {0x0};
		for (int i = 0, byte = line * 16; byte < (line + 1) * 16; byte++,i++) {
			bytes[i] = guest.mem->stackSeg[byte];
		}

		sprintf(bytesLine, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", 
			bytes[0] & 0xff, bytes[1] & 0xff, bytes[2] & 0xff, bytes[3] & 0xff, bytes[4] & 0xff, bytes[5] & 0xff, bytes[6] & 0xff, bytes[7] & 0xff, 
			bytes[8] & 0xff, bytes[9] & 0xff, bytes[10] & 0xff, bytes[11] & 0xff, bytes[12] & 0xff, bytes[13] & 0xff, bytes[14] & 0xff, bytes[15] & 0xff);

		strcat(contents, bytesLine);

		free(bytesLine);
	}

	fputs(contents, file);
	printf("Stack dumped...\n");
	free(contents);

	contents = malloc((TEXT_SEG_MAX / 16) * 82);

	sprintf(contents, "\n** TEXT **\n%p:\n", guest.mem->textSeg);

	// add text contents
	for (int line = 0; line < TEXT_SEG_MAX / 16; line++) {
		char* bytesLine = malloc(80 + 2);

		char bytes[16] = {0x0};
		for (int i = 0, byte = line * 16; byte < (line + 1) * 16; byte++,i++) {
			bytes[i] = guest.mem->textSeg[byte];
		}

		sprintf(bytesLine, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", 
			bytes[0] & 0xff, bytes[1] & 0xff, bytes[2] & 0xff, bytes[3] & 0xff, bytes[4] & 0xff, bytes[5] & 0xff, bytes[6] & 0xff, bytes[7] & 0xff, 
			bytes[8] & 0xff, bytes[9] & 0xff, bytes[10] & 0xff, bytes[11] & 0xff, bytes[12] & 0xff, bytes[13] & 0xff, bytes[14] & 0xff, bytes[15] & 0xff);

		strcat(contents, bytesLine);

		free(bytesLine);
	}

	fputs(contents, file);
	printf("Memory dumped...\n");
	free(contents);

	fclose(file);
}