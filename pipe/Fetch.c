#include "instr.h"
#include "pipeline.h"
#include "machine.h"
#include "hardware.h"

extern machine_t guest;

extern uint16_t fetchIP;


static void selectIP(uint16_t predIP, opcode_t decodeOP, uint16_t decodeSeqIP,
		opcode_t memOP, uint8_t seqIP, uint16_t* currIP) {
	
	if (decodeOP == OP_HLT) *currIP = 0xFFFF;
	else *currIP = predIP;

	// printf("Selected currIP %d from predIP %d\n", *currIP, predIP);
}

/**
 * Predict IP logic. JNZ are predicted taken, that is, it assumes reg A != 0.
 * @param currIP The current IP
 * @param insn The encoded instruction
 * @param op The opcode
 * @param predIP The predicted next IP
 * @param seqIP The next sequential IP
 */
static void predIP(uint16_t currIP, uint8_t insn, opcode_t op, uint16_t* predIP, uint16_t* seqIP) {
	if (currIP == 0xFFFF) return;

	uint8_t imm = extractBits(insn, 0);

	*seqIP = currIP + 1;

	if (op == OP_JNZ) *predIP = imm;
	else *predIP = *seqIP;

	// printf("New pred IP: %d; Seq IP: %d\n", *predIP, *seqIP);
}

void fetch(fetch_reg_t* in, decode_reg_t* out) {
	// printf("\nFetching!\n");

	uint16_t currIP;
	selectIP(in->nextIP, execOut->op, execOut->seqIP, memOut->op, memOut->seqIP, &currIP);

	if (currIP == 0xFFFF || fetchIn->status == STAT_HLT) {
		// printf("Setting out->op to HALT\n");
		out->insn = 0xFF;
		out->op = OP_HLT;
	} else {
		// printf("For IP %d\n", currIP);

		imem(currIP, &(out->insn));

		uint8_t opcode = extractBits(out->insn, 4);
		opcode_t op = itable[opcode];

		// printf("Fetched instruction opcode: %x\n", opcode);

		out->op = op;

		predIP(currIP, out->insn, out->op, &fetchIP, &(out->seqIP));

		// printf("global fetchIP: %d\n", fetchIP);
	}

	if (out->op == OP_HLT) {
		in->status = STAT_HLT;
		fetchIn->status = in->status;
		// printf("Setting fetchIn->status to HALT\n");
	} else in->status = STAT_OK;

	out->status = in->status;
}