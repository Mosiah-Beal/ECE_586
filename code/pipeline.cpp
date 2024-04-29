

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
    return trace >> 26;
}

/**
 * @brief Fetch instruction from memory
 * 
 */
void Pipeline::IF(int inputBin) {
    int opcode = getOpcode(inputBin); // get the opcode from the line
    instruction inst = instructionSet.getInstruction(opcode);
    stages[0] = inst;
}

