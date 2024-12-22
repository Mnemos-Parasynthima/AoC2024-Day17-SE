#ifndef _FORWARD_H_
#define _FORWARD_H_

#include <stdint.h>

/**
 * Forwarding unit. Handles forwarding for two outputs.
 * @param decodeSrc1 Register src 1 extracted in Decode
 * @param decodeSrc2 Register src 2 extracted in Decode
 * @param execDst Register to use in writeback, acquired from Decode, outgoing from Exec
 * @param memDst Register to use in writeback, acquired from Decode, outgoing from Mem
 * @param writeDst Register to use in writeback, acquired from Decode, outgoing from Write
 * @param execValRes Valued computed by ALU, outgoing from Exec|incoming to Mem
 * @param memValRes Valued computed by ALU, outgoing from Mem|incoming to Write
 * @param writeValRes Valued computed by ALU, outgoing from Write
 * @param execWriteEnable Write enable signal outgoing from Execute
 * @param memWriteEnable Write enable signal outgoing from Memory
 * @param writeWriteEnable Write enable signal outgoing from Writeback
 * @param valOutA Val A to be fed to ALU, possible correction
 * @param valOutB Val B to be fed to ALU, possible correction
 */
void forwardReg(uint8_t decodeSrc1, uint8_t decodeSrc2, uint8_t execDst, uint8_t memDst, uint8_t writeDst, uint64_t execValRes, 
		uint64_t memValRes, uint64_t writeValRes, bool execWriteEnable, bool memWriteEnable, bool writeWriteEnable, uint64_t* valOutA, uint64_t* valOutB);


#endif