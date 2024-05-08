

#include "pipeline.h"
#include "instructions.h"
#include <iostream>

Pipeline::Pipeline() {
    numStalls = 0;      // Initialize number of stalls to 0
    stages.resize(5);   // 5 stages in pipeline
    initBusyRegs();     // Initialize busy registers
}

Pipeline::~Pipeline() {
}

/**
 * @brief Initialize busy registers to 0
 * 
 */
void Pipeline::initBusyRegs(void) {
    for (int i = 0; i < 32; i++) {
        busyRegs[i] = 0;
    }
}

/**
 * @brief Decrement busy registers by 1 (at end of cycle)
 * 
 */
void Pipeline::decrBusyRegs(void) {
    for (int i = 0; i < 32; i++) {
        if (busyRegs[i] > 0) {
            busyRegs[i]--;
        }
    }
}

/**
 * @brief Move instructions to next stage
 * 
 */
void Pipeline::moveStages(int line) {
    stages[4] = stages[3]; // Move MEM to WB
    stages[3] = stages[2]; // Move EX to MEM
    stages[2] = stages[1]; // Move ID to EX
    stages[1] = stages[0]; // Move IF to ID
    
    // call IF to fill IF stage
    IF(line);
}

/**
 * @brief Insert stall
 * 
 */
void Pipeline::stall(void) {
    numStalls++;
    // insert NOP(s) into stages

}

/**
 * @brief Flush pipeline after misprediction
 * 
 */
void Pipeline::flush(void) {
    // TODO: Implement
    std::cout << "Not implemented" << std::endl;

}

int Pipeline::getOpcode(int trace) {
    // Mask the top 6 bits to get the opcode
    int opcode = (trace & 0xFC000000) >> 26;
    return opcode;
}

/**
 * @brief Fetch instruction from memory
 * 
 */
void Pipeline::IF(int inputBin) { 
    int opcode = getOpcode(inputBin); // get the opcode from the line
    instruction inst = instructionSet.getInstruction(opcode);
    stages[0] = inst;
    status.typeExecd[inst.type]++; // update number of times instruction type was executed
    status.PC += 4; // increment program counter
    
}

void Pipeline::ID(int inputBin, instruction &inst) {

    if (inst.addressMode == 0) { // Immediate addressing
        inst.instr.opcode = getOpcode(inputBin);
        inst.instr.imm = inputBin & 0xFFFF;
        inst.instr.rt = (inputBin & 0x1F0000) >> 16;
        inst.instr.rs = (inputBin & 0x3E00000) >> 21;

        #ifdef DEBUG
        printf("%s R%d, R%d, #%d\n", inst.name.c_str(), inst.instr.rt, inst.instr.rs, inst.instr.imm);
        #endif
        
    } else { // Register addressing
      	inst.instr.opcode = getOpcode(inputBin);
        inst.instr.rs = (inputBin & 0x03E00000) >> 21;
        inst.instr.rt = (inputBin & 0x001F0000) >> 16;
        inst.instr.rd = (inputBin & 0x0000F800) >> 11;

        #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", inst.name.c_str(), inst.instr.rd, inst.instr.rs, inst.instr.rt);
        #endif
    }
   
}

void Pipeline::EX(instruction &inst, Status &status) {
    switch(inst.instr.opcode) {
        // REGISTER
        case 0: // ADD
            ALUresult = status.registers[inst.instr.rs] + status.registers[inst.instr.rt];
            break;
        case 2: // SUB
            ALUresult = status.registers[inst.instr.rs] - status.registers[inst.instr.rt];
            break;
        case 4: // MUL
            ALUresult = status.registers[inst.instr.rs] * status.registers[inst.instr.rt];
            break;
        case 6: // OR
            ALUresult = status.registers[inst.instr.rs] | status.registers[inst.instr.rt];
            break;
        case 8: // AND
            ALUresult = status.registers[inst.instr.rs] & status.registers[inst.instr.rt];
            break;
        case 10: // XOR
            ALUresult = status.registers[inst.instr.rs] ^ status.registers[inst.instr.rt];
            break;
        // IMMEDIATE
        case 1: // ADDI
            ALUresult = status.registers[inst.instr.rs] + inst.instr.imm;
            break;
        case 3: // SUBI
            ALUresult = status.registers[inst.instr.rs] - inst.instr.imm;
            break;
        case 5: // MULI
            ALUresult = status.registers[inst.instr.rs] * inst.instr.imm;
            break;
        case 7: // ORI
            ALUresult = status.registers[inst.instr.rs] | inst.instr.imm;
            break;
        case 9: // ANDI
            ALUresult = status.registers[inst.instr.rs] & inst.instr.imm;
            break;
        case 11: // XORI
            ALUresult = status.registers[inst.instr.rs] ^ inst.instr.imm;
            break;
        case 12: // LDW
            ALUresult = status.memory[inst.instr.imm];
            break;
        case 13: // STW
            ALUresult = status.registers[inst.instr.rt];
            break;
        case 14: // BZ
            if (status.registers[inst.instr.rs] == 0) {
                status.PC += inst.instr.imm * 4;
            }
            break;
        case 15: // BEQ
            if (status.registers[inst.instr.rt] == status.registers[inst.instr.rs]) {
                status.PC += inst.instr.imm * 4;
            }
            break;
        case 16: // JR
            status.PC += status.registers[inst.instr.rs] * 4;
            break;
    }
}

void Pipeline::MEM(instruction &inst) {
    // Assign the result to the appropriate register
    if (inst.addressMode) {
        status.registers[inst.instr.rd] = ALUresult;
    } else{
        status.registers[inst.instr.rt] = ALUresult;
    }
    
    if(inst.instr.opcode == 12) {
    
    status.memory[MDR] = ALUresult;
    
    }
    
}


void Pipeline::WB(instruction &inst) {

if(inst.instr.opcode == 12) {

status.registers[inst.instr.rt] = status.memory[MDR];

}
else{
//nop
}
}


