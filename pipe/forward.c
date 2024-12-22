#include <stdbool.h>
#include <stdio.h>

#include "forward.h"

void forwardReg(uint8_t decodeSrc1, uint8_t decodeSrc2, uint8_t execDst, uint8_t memDst, uint8_t writeDst, uint64_t execValRes, 
		uint64_t memValRes, uint64_t writeValRes, bool execWriteEnable, bool memWriteEnable, bool writeWriteEnable, uint64_t* valOutA, uint64_t* valOutB) {

	// printf("forwardReg::\n");
	// printf("execValRes: %ld; memValRes: %ld; writeValRes: %ld\n", execValRes, memValRes, writeValRes);
	// printf("execDst: %d; memDst: %d; writeDst: %d\n", execDst, memDst, writeDst);

	// I HAVE NO IDEA WHAT IS GOING ON BUT IT WORKS AND FORWARDING AND BLAH BLAH
	if (decodeSrc1 == execDst && execWriteEnable) {
		*valOutA = execValRes;
		// printf("decodeSrc1 %d == execDst %d && execWriteEnable %d\n", decodeSrc1, execDst, execWriteEnable);
	} else if (decodeSrc1 == memDst && memWriteEnable) {
		*valOutA = memValRes;
		// printf("decodeSrc1 %d == memDst %d && memWriteEnable %d\n", decodeSrc1, memDst, memWriteEnable);
	} else if (decodeSrc1 == writeDst && writeWriteEnable) {
		*valOutA = writeValRes;
		// printf("decodeSrc1 %d == writeDst %d && writeWriteEnable %d\n", decodeSrc1, writeDst, writeWriteEnable);
	}

	if (decodeSrc2 == execDst && execWriteEnable) *valOutB = execValRes;
	else if (decodeSrc2 == memDst && memWriteEnable) *valOutB = memValRes;
	else if (decodeSrc2 == writeDst && writeWriteEnable) *valOutB = writeValRes;
}