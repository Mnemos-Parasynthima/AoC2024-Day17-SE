#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include <stdbool.h>

#include "instr.h"


typedef enum procStage {
	FETCH,
	DECODE,
	EXECUTE,
	MEMORY,
	WRITE,
	UPDATE_IP,
	ERROR = -1
} proc_stage_t;

typedef enum alu_op {
	ALU_DIV, // valA /  2^valB
	ALU_XOR, // valA ^ valB
	ALU_MOD, // valA % valB
	ALU_PASS, // valA
	ALU_ERR = -1
} alu_op_t;

typedef struct decodeControl {
	bool src1Sel; // 0 for imm/literal, 1 for register
	bool src2Sel; // 0 for imm/literal, 1 for register
	bool literal; // 0 for combo operand, 1 for literal operand
	bool registerCombo; // 0 for a literal in a combo, 1 for a register in a combo
} decode_ctrl_t;

typedef struct executeControl {
	bool valASel; // 0 to use literal (immA), 1 to use reg[i] (valA)
	bool valBSel; // 0 to use literal (immB), 1 to use reg[i] (valB)
	bool setZZZ; // 0 to set ZZZ flags, 1 to not
} exec_ctrl_t;

typedef struct memoryControl {
	bool dmemWrite; // 1 if to write (stack)
} mem_ctrl_t;

typedef struct writebackControl {
	bool writeEnable; // 1 if write back to register
} writeback_ctrl_t;

// Pipeline register feeding the Fetch stage
typedef struct fetchRegiser {
	uint16_t nextIP; // the next IP
	stat_t status; // status of the instruction
} fetch_reg_t;

// Pipeline register feeding the Decode stage
typedef struct decodeRegiser {
	uint8_t insn; // instruction bits
	opcode_t op; // instruction opcode
	uint16_t seqIP; // next sequential IP
	stat_t status; // status of the instruction
} decode_reg_t;

// Pipeline register feeding the Execute stage
typedef struct executeRegiser {
	opcode_t op; // instruction opcode
	uint16_t seqIP; // next sequential IP
	exec_ctrl_t execSigs; // signals consumed by execute
	mem_ctrl_t memSigs; // signals consumed by memory
	writeback_ctrl_t writeSigs; // signals consumed by writeback
	alu_op_t aluOP; // operation for the ALU to perform
	uint64_t valA; // regfile output from register src1 to be fed to ALU
	uint64_t valB; // regfile output from register src2 to be fed to ALU
	uint8_t immA; // imm 1 to be fed to ALU (false valASel); immB must not be used if this is used
	uint8_t immB; // imm 2 to be fed to ALU (false valBSel); immA must not be used if this is used
	uint8_t dst; // destination register acquired from decode and used in writeback
	stat_t status; // status of the instruction
} exec_reg_t;

// Pipeline register feeding the Memory stage
typedef struct memoryRegiser {
	opcode_t op; // instruction opcode
	uint16_t seqIP; // next sequential IP
	bool cond; // result of register A test, true if reg A holds 0
	mem_ctrl_t memSigs; // signals consumed by memory
	writeback_ctrl_t writeSigs; // signals consumed by writeback
	uint64_t valRes; // value computed by ALU, may be written to *SP here or to dst reg
	uint8_t dst; // destination register acquired from decode and used in writeback
	stat_t status; // status of the instruction
} mem_reg_t;

// Pipeline register feeding the Writeback stage
typedef struct writebackRegiser {
	opcode_t op; // instruction opcode
	writeback_ctrl_t writeSigs; // signals consumed by writeback
	uint8_t dst; // destination register acquired from decode and used here
	uint64_t valRes; // value computed by ALU, may be written to dst reg
	stat_t status; // status of the instruction
} writeback_reg_t;

typedef enum pipeCtrlStat {
	CTRL_LOAD, // let instruction through
	CTRL_ERROR, // error in future stage, stop
	CTRL_BUBBLE, // bubble
	CTRL_STALL // stall
} pipe_ctrl_stat_t;

// Unified version of the pipeline stage registers.
typedef union pipeRegisterU {
	fetch_reg_t* fetch; // Pipeline into Fetch
	decode_reg_t* decode;
	exec_reg_t* exec;
	mem_reg_t* mem;
	writeback_reg_t* write;
	void* generic;
} pipe_reg_u_t;

// The full pipeline register, containing input and out
typedef struct pipelineRegister {
	pipe_reg_u_t in; // input side, prev stage writes to this
	pipe_reg_u_t out; // output side, curr state reads from this
	uint64_t size; // size of respective pipeline register struct
	pipe_ctrl_stat_t ctrl; // what to do between cycles
} pipe_reg_t;


#define fetchP (guest.proc->fetchPipe)
#define fetchIn (guest.proc->fetchPipe->in.fetch)
#define fetchOut (guest.proc->fetchPipe->out.fetch)

#define decodeP (guest.proc->decodePipe)
#define decodeIn (guest.proc->decodePipe->in.decode)
#define decodeOut (guest.proc->decodePipe->out.decode)

#define execP (guest.proc->executePipe)
#define execIn (guest.proc->executePipe->in.exec)
#define execOut (guest.proc->executePipe->out.exec)

#define memP (guest.proc->memoryPipe)
#define memIn (guest.proc->memoryPipe->in.mem)
#define memOut (guest.proc->memoryPipe->out.mem)

#define writeP (guest.proc->writebackPipe)
#define writeIn (guest.proc->writebackPipe->in.write)
#define writeOut (guest.proc->writebackPipe->out.write)


extern void fetch(fetch_reg_t* in, decode_reg_t* out);
extern void decode(decode_reg_t* in, exec_reg_t* out);
extern void execute(exec_reg_t* in, mem_reg_t* out);
extern void memory(mem_reg_t* in, writeback_reg_t* out);
extern void writeback(writeback_reg_t* in);

#endif