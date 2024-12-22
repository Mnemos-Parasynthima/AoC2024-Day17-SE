#include "instr.h"
#include "pipeline.h"
#include "forward.h"
#include "machine.h"
#include "hardware.h"

extern machine_t guest;

extern int64_t writeWVal;


/**
 * Logic for extracting the immediate value for instructions using literal operands.
 * @param insn The encoded instruction
 * @param op The opcode
 * @param imm The extracted immediate value
 */
static void extractImm(uint8_t insn, opcode_t op, uint8_t* imm) {
	uint8_t immb = extractBits(insn, 0);

	// For instructions bxl and jnz, it is always a literal operand
	// For the rest, it is a combo operand
	// Check if the combo operand is literal (0-3) or a reg (4-7)

	*imm = 0x0;

	if (op == OP_BXL || op == OP_JNZ) *imm = immb;
	else if (op == OP_ADV || op == OP_BST || op == OP_OUT || op == OP_BDV || op == OP_CDV) {
		// operand is combo
		// it is only imm if it's 0-3
		if (immb >= 0 || immb <= 3) *imm = immb;
	}
}

/**
 * Logic for determining the ALU operation needed based on the opcode.
 * @param op The opcode
 * @param aluOP The ALU operation to do
 */
static void decideALU(opcode_t op, alu_op_t* aluOP) {
	switch (op)	{
		case OP_ADV:
		case OP_BDV:
		case OP_CDV:
			*aluOP = ALU_DIV;
			break;
		case OP_BXL:
		case OP_BXC:
			*aluOP = ALU_XOR;
			break;
		case OP_BST:
		case OP_OUT:
			*aluOP = ALU_MOD;
			break;
		default:
			*aluOP = ALU_PASS;
			break;
	}
}

/**
 * Control signals for Decode, Execute, Memory, Writeback stages.
 * Generated in Decode stage with Decode signals consumed locally.
 * Others are to be buffered in pipeline registers.
 * @param op The opcode
 * @param imm The operand to be interpreted as a register number
 * @param decodeSigs The control signals for Decode
 * @param execSigs The control signals for Execute
 * @param memSigs The control signals for Memory
 * @param writeSigs The control signals for Writeback
 */
static void generateControlSigs(opcode_t op, uint64_t imm, decode_ctrl_t* decodeSigs, exec_ctrl_t* execSigs, mem_ctrl_t* memSigs, writeback_ctrl_t* writeSigs) {
	// reset
	decodeSigs->src1Sel = false;
	decodeSigs->src2Sel = false;
	decodeSigs->literal = true;
	decodeSigs->registerCombo = false;

	execSigs->valASel = false;
	execSigs->valBSel = false;
	execSigs->setZZZ = false;

	memSigs->dmemWrite = false;

	writeSigs->writeEnable = false;


	// these instructions use combo operand
	// which operand does not matter for now
	if (op == OP_ADV || op == OP_BST || op == OP_OUT || op == OP_BDV || op == OP_CDV) {
		decodeSigs->literal = false;
	}

	// for the instructions that use combo, these use a register
	if (!decodeSigs->literal) {
		// printf("Instruction uses combo operand\n");

		if (imm >= 4 && imm <= 6) {
			// printf("Operand is a register: %ld\n", imm);
			decodeSigs->registerCombo = true;
		}
	}

	// for the combo register instructions, these,
	// which operand 
	if (decodeSigs->registerCombo) {
		// printf("Instruction uses a register as its combo operand\n");

		if (op == OP_BST || op == OP_OUT) decodeSigs->src1Sel = true;
		else decodeSigs->src2Sel = true;
	}


	// these instructions always have their valA come from a register
	if (op == OP_ADV || op == OP_BXL || op == OP_BXC || op == OP_BDV || op == OP_CDV) {
		// printf("Instruction always uses register for first operand\n");

		decodeSigs->src1Sel = true;
		execSigs->valASel = true;
	}

	// the bxc always has its valB come from a register
	if (op == OP_BXC) {
		// printf("Instruction always uses register for second operand\n");
		decodeSigs->src2Sel = true;
		execSigs->valBSel = true;
	}

	// for the instructions that use combo, only apply those that use registers 
	if (decodeSigs->registerCombo) {
		// these instructions have their combo register for valA
		if (op == OP_BST || op == OP_OUT) execSigs->valASel = true;

		// these instructions have their combo register for valB
		if (op == OP_ADV || op == OP_BDV || op == OP_CDV) execSigs->valBSel = true;
	}

	if (!(op == OP_OUT || op == OP_HLT || op == OP_NOP)) {
		// printf("Set ZZZ? true\n");
		execSigs->setZZZ = true;
	}


	// the out instruction is the only one to write to mem (stack)
	if (op == OP_OUT) memSigs->dmemWrite = true;


	// except for these instructions, all other ones write back to registers
	if (!(op == OP_HLT || op == OP_JNZ || op == OP_OUT)) writeSigs->writeEnable = true;
}

void copyMemCtrlSigs(mem_ctrl_t* dest, mem_ctrl_t* src) {
	dest->dmemWrite = src->dmemWrite;
}

void copyWriteCtrlSigs(writeback_ctrl_t* dest, writeback_ctrl_t* src) {
	dest->writeEnable = src->writeEnable;
}

static void extractReg(uint8_t insn, opcode_t op, uint8_t* src1, uint8_t* src2, uint8_t* dst, decode_ctrl_t* decodeSigs) {
	if (op == OP_ADV) *dst = REG_A;
	if (op == OP_BXL || op == OP_BST || op == OP_BXC || op == OP_BDV) *dst = REG_B;
	if (op == OP_CDV) *dst = REG_C;

	// printf("Set destination register to %d\n", *dst);

	uint8_t reg = extractBits(insn, 0);

	if (decodeSigs->src1Sel) {
		// printf("src1 is to be a register\n");
		// at this point, src1 is to be a register

		if (op == OP_ADV || op == OP_BDV || op == OP_CDV) *src1 = REG_A;
		if (op == OP_BXL || op == OP_BXC) *src1 = REG_B;

		if (op == OP_BST || op == OP_OUT) {
			// for combo operands, it is guaranteed that the extracted bits represent a register
			if (reg == 4) *src1 = REG_A;
			else if (reg == 5) *src1 = REG_B;
			else *src1 = REG_C; // if reg == 6
		}
	}

	if (decodeSigs->src2Sel) {
		// printf("src2 is to be a register\n");
		// at this point, src2 is to be a register

		if (op == OP_BXC) *src2 = REG_C;

		if (op == OP_ADV || op == OP_BDV || op == OP_CDV) {
			if (reg == 4) *src2 = REG_A;
			else if (reg == 5) *src2 = REG_B;
			else *src2 = REG_C; // if reg == 6
		}
	}
}

void decode(decode_reg_t* in, exec_reg_t* out) {
	// printf("\nDecoding!\n");

	opcode_t op = in->op;
	uint8_t insn = in->insn;

	// printf("in op: %d, in insn: 0x%x\n", op, insn);

	out->op = op;
	out->seqIP = in->seqIP;

	decode_ctrl_t decodeSigs;

	extractImm(insn, op, &(out->immA));
	out->immB = out->immA;
	generateControlSigs(op, out->immA, &decodeSigs, &(out->execSigs), &(out->memSigs), &(out->writeSigs));
	decideALU(op, &(out->aluOP));

	uint8_t src1 = REG_NONE; // register to be as src1
	uint8_t src2 = REG_NONE; // register be as src2
	uint8_t dst = REG_NONE; // register to be as dst

	extractReg(insn, op, &src1, &src2, &dst, &decodeSigs);
	out->dst = dst;

	// Register sources (src1, src2) are set/absolutely known
	// due to extractReg (do not depend on other instructions)
	// However, when getting the values from the registers,
	// data race may happen, that is, the intended value
	// may not even be there and out->valA/out->valB will be garbage values
	// This is taken care of by forward()
	// although I don't know how but it does :p
	
	regfile(src1, src2, writeOut->dst, writeWVal, writeOut->writeSigs.writeEnable, &(out->valA), &(out->valB));

	forwardReg(src1, src2, execOut->dst, memOut->dst, writeOut->dst, memIn->valRes, memOut->valRes, writeOut->valRes, 
			execOut->writeSigs.writeEnable, memOut->writeSigs.writeEnable, writeOut->writeSigs.writeEnable, &(out->valA), &(out->valB));

	out->status = in->status;

	// printf("insn: 0x%x; OP: %d; immA: %d; immB: %d; ALU OP: %d, reg src1: %d, reg src1 val: %ld, reg src2: %d, reg src2 val: %ld, reg dst: %d\n",
	// 	insn, out->op, out->immA, out->immB, out->aluOP, src1, out->valA, src2, out->valB, dst);

	// if (op == OP_HLT || op == OP_NOP) out->valB = out.
}