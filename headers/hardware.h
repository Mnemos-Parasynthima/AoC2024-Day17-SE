#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include <stdbool.h>
#include <stdint.h>

#include "pipeline.h"

/**
 * Arithmetic and logic unit.
 * 
 * @param valA Input a
 * @param valB Input b
 * @param op The alu operation
 * @param setZZZ Whether to set the ZZZ flags
 * @param valRes The result of the operation
 * @param cond Condition result for testing the flags register
 * @param zzz The ZZZ flag register to update if needed
 */
void alu(uint64_t valA, uint64_t valB, alu_op_t op, bool setZZZ, uint64_t* valRes, bool* cond, uint8_t* zzz);

/**
 * Data memory, except it is just the stack.
 * 
 * @param dmemIdx The memory address
 * @param dmemWrite Whether to write
 * @param dmemWriteVal The value to write, if writing
 * @param dmemRead Whether to read
 * @param dmemReadVal The value read, if reading
 */
void dmem(uint16_t dmemIdx, bool dmemWrite, uint8_t dmemWriteVal, bool dmemRead, uint8_t* dmemReadVal);

/**
 * Instruction memory.
 * 
 * @param imemIdx The address of the instruction to get
 * @param imemRet The instruction to decode
 */
void imem(uint16_t imemIdx, uint8_t* imemRet);

/**
 * Register file.
 * 
 * Inputs to write are fed in Writeback while outputs to read in Decode.
 * 
 * @param src1 The source register 1 to read from
 * @param src2 The source register 2 to read from
 * @param dst The destination register to write, if any
 * @param valWrite The value to write, if any
 * @param writeEnable Whether to write or not
 * @param valOutA Outgoing value in src reg 1
 * @param valOutB Outgoing value in src reg 2
 */
void regfile(uint8_t src1, uint8_t src2, uint8_t dst, uint64_t valWrite,
	bool writeEnable, uint64_t* valOutA, uint64_t* valOutB);

#endif