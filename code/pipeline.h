#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vector>
#include <map>
// #include <iostream>
// #include <fstream>

#include "instructions.h"

class Pipeline {
    private:
            int numStalls;    // Number of stalls inserted
            int ALUresult;    // Result of ALU operation
            instruction MDR;  // Memory Destination Register (load memory MEM stage)

    public:
        Pipeline();
        ~Pipeline();

        std::vector<instruction> stages;    // Holds instructions in each stage
        std::map<int, int> busyRegs;        // Holds register values in use (stall cycles needed)
        Instructions instructionSet;        // Initialize instruction set

    private:
        // Stage functions
        void IF(int inputBin);          // Instruction Fetch
        void ID(instruction &inst);     // Decode
        void EX(instruction &inst);     // Execute
        void MEM(instruction &inst);    // Memory
        void WB(instruction &inst);     // Writeback

        // Helper functions
        void printStages(void);
        void printBusyRegs(void);
        void printMDR(void);

        // Pipeline control
        int getOpcode(int bin); // get the opcode from the binary instruction
        void stall(void);       // Insert stall
        void flush(void);       // Flush pipeline after misprediction
        
        // Control functions
        void initBusyRegs(void);    // Initialize busy registers to 0
        void decrBusyRegs(void);    // Decrement busy registers by 1 (at end of cycle)
        void moveStages(int bin);      // Move instructions to next stage


};

#endif /* PIPELINE_H */
