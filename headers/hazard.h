#ifndef _HAZARD_H_
#define _HAZARD_H_

#include <stdint.h>

#include "instr.h"
#include "pipeline.h"


void pipeCtrlStage(proc_stage_t stage, bool bubble, bool stall);
bool checkMispredHazard(opcode_t execOP, bool execCond);
void handleHazards(opcode_t opcode, opcode_t execOP, bool execCond);

#endif