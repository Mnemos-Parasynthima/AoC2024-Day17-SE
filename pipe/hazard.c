#include "machine.h"
#include "hazard.h"

extern machine_t guest;


void pipeCtrlStage(proc_stage_t stage, bool bubble, bool stall) {
	pipe_reg_t* pipe;

	switch (stage) {
		case FETCH:
			pipe = fetchP;
			break;
		case DECODE:
			pipe = decodeP;
			break;
		case EXECUTE:
			pipe = execP;
			break;
		case MEMORY:
			pipe = memP;
			break;
		case WRITE:
			pipe = writeP;
			break;
		default:
			break;
	}

	if (bubble && stall) {
		printf("Error: cannot bubble and stall at same time!\n");
		pipe->ctrl = CTRL_ERROR;
	}

	if (pipe->ctrl == CTRL_ERROR) return;

	if (bubble) pipe->ctrl = CTRL_BUBBLE;
	else if (stall) pipe->ctrl = CTRL_STALL;
	else pipe->ctrl = CTRL_LOAD;
}

bool checkMispredHazard(opcode_t execOP, bool execCond) {
	// printf("execOP is %d, execCond is %d\n", execOP, execCond);

	// assumption is condval is false (branch was taken b/c reg a != 0)
	if (execOP == OP_JNZ && execCond) return true;
	return false;
}


void handleHazards(opcode_t opcode, opcode_t execOP, bool execCond) {
	pipeCtrlStage(FETCH, false, false);
	pipeCtrlStage(DECODE, false, false);
	pipeCtrlStage(EXECUTE, false, false);
	pipeCtrlStage(MEMORY, false, false);
	pipeCtrlStage(WRITE, false, false);


	if (checkMispredHazard(execOP, execCond)) {
		// printf("\nMispredicted! Bubbling DECODE and EXECUTE!\n");
		pipeCtrlStage(DECODE, true, false);
		pipeCtrlStage(EXECUTE, true, false);

		fetchIn->nextIP = execOut->seqIP;
		// printf("Setting fetchIn->nextIP to memOut->seqIP: %d\n", fetchIn->nextIP);
	}
}