#include "mem.h"
#include "machine.h"

extern machine_t guest;


static uint8_t _memReadByte(const uint16_t idx) {
	// printf("mem.c::_memReadByte: Reading byte at idx %d from text segment -> 0x%x\n", idx, guest.mem->textSeg[idx]);

	return guest.mem->textSeg[idx];
}

static uint8_t _memRead(const uint16_t idx, const unsigned width) {
	uint8_t ret = 0x0;
	for (int i = width-1; i >= 0; i--) {
		ret = _memReadByte(idx+i);
	}

	// printf("mem.c::_memRead: Reading width %d at idx %d -> 0x%x\n", width, idx, ret);

	return ret;
}

static void _memWriteByte(const uint16_t idx, const uint8_t data) {
	// printf("mem.c::_memWriteByte: Writing data byte %d at idx %d\n", data, idx);
	guest.mem->stackSeg[idx] = data;
}

static void _memWrite(const uint16_t idx, const uint64_t data, const unsigned width) {
	uint8_t* _data = (uint8_t*) &data;

	// printf("mem.c::_memWrite: Writing data %ld at idx %d with width %d\n", data, idx, width);

	for (int i = 0; i < width; i++) {
		_memWriteByte(idx+i, _data[i]);
		guest.proc->sp += 1;
	}
}


char memReadByte(uint16_t idx) { return (char) _memRead(idx, 1); }
void memWriteLong(uint16_t idx, long data) { _memWrite(idx, (uint64_t) data, 8); }
void memWriteByte(uint16_t idx, char data) { _memWrite(idx, (uint8_t) data, 1); }