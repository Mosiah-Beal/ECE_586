#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vector>
#include <map>
// #include <iostream>
// #include <fstream>

#include "instructions.h"
#include "status.h"

// Define constants for the stages
#define _IF 0
#define _ID 1
#define _EX 2
#define _MEM 3
#define _WB 4



class Pipeline {
    private:
            int numStalls;    // Number of stalls inserted
            int ALUresult;    // Result of ALU operation
            int MDR;  // Memory Destination Register (load memory MEM stage)

    public:
        Pipeline();
        ~Pipeline();

        void initNOPs(); // Initialize NOPs in stages

        std::vector<instruction> stages;    // Holds instructions in each stage
        std::map<int, int> busyRegs;        // Holds register values in use (stall cycles needed)
        Instructions instructionSet;        // Initialize instruction set
        Status status;                      // Holds status of machine

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
        void stall(int cycles);         // Insert stall cycles
        void flush(void);               // Flush pipeline after misprediction
        
        // Control functions
        void initBusyRegs(void);    // Initialize busy registers to 0
        void decrBusyRegs(void);    // Decrement busy registers by 1 (at end of cycle)
    public:
        void moveStages(int bin);       // Move instructions to next stage
        int checkHalt(int bin);         // Determine if we have hit a halt instruction
        void run(instruction &inst);    // Run the pipeline (one instruction at a time for now)
        int getOpcode(int bin);         // get the opcode from the binary instruction
        void printFields(instruction &inst); // Print the fields of the instruction

    private:
        void parseInstruction(instruction &inst);
        void executeInstruction(instruction &inst);
        void Hazards(void);            // Check for data hazards
        void printBinary(int n);       // Print binary representation of int
        
};

#endif /* PIPELINE_H */
