#ifndef _MEM_H_
#define _MEM_H_

#include <stdint.h>
#include <stdbool.h>

#define KB 1024

// I'm only allocating a 1/16 of a page's worth of memory to stack and text segments because I can
// Not going to deal with semi virtual paged memory and allocating pages and stuff for now(?)
// Plus, I don't think this chronospatial computer would hold that much memory anyway

#define TEXT_SEG_MAX KB / 4
#define STACK_SEG_MAX KB / 4

// Magic numbers are not really enforced for now(?)

// Memory subsystem
typedef struct mem {
	uint16_t STACK_SEG_MAGIC;
	uint8_t stackSeg[STACK_SEG_MAX];
	uint16_t TEXT_SEG_MAGIC;
	uint8_t textSeg[TEXT_SEG_MAX];
} mem_t;


char memReadByte(uint16_t idx);

void memWriteLong(uint16_t idx, long data);
void memWriteByte(uint16_t idx, char data);

#endif