#ifndef PIPELINE_SIMPLE_H
#define PIPELINE_SIMPLE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

// Define constants for the stages
#define _IF 0
#define _ID 1
#define _EX 2
#define _MEM 3
#define _WB 4

// Holds all register and immediate values for a given instruction
// Depending on format, some fields will be unused
typedef struct {
    int opcode; // op code -- determines operation  (top 6 bits)
    signed int rs;     // source register  (first operand) - 5 bits
    signed int rt;     // temp register    (second operand) - 5 bits
    signed int rd;     // destination register (result) - 5 bits or 0 if not used
    signed int imm;    // immediate value  (for arithmetic/logical) - 16 bits
} InstrInfo;

// Used to parse instruction and update type counter
typedef struct {
    InstrInfo instr;    // Struct for decoded instruction
    int binInstr;       // Binary representation of instruction
    std::string name;   // for print/debugging
    bool addressMode;   // 0 = immediate, 1 = register
    int type;
    /**
     * Type of instruction:
     * 0 = arithmetic
     * 1 = logical
     * 2 = memory access
     * 3 = control flow
     */
} instruction;

class Pipeline {
private:
    int numStalls;    // Number of stalls inserted
    int ALUresult;    // Result of ALU operation
    int MDR;  // Memory Destination Register (load memory MEM stage)
    int PC;   // Program Counter
    int registers[32];  // Registers
    int typeExecd[4];   // Number of times each instruction type was executed
    std::map<int, int> memory; // Memory <address, value>

    std::map<int, instruction> instructionSet; // Instruction set

public:
    Pipeline();
    ~Pipeline();

    void initNOPs(); // Initialize NOPs in stages

    instruction stages;    // Holds instructions in each stage
    std::map<int, int> busyRegs;        // Holds register values in use (stall cycles needed)

private:
    // Stage functions
    void IF(int inputBin);          // Instruction Fetch
    void ID(instruction &inst);     // Decode
    void EX(instruction &inst);     // Execute
    void MEM(instruction &inst);    // Memory
    void WB(instruction &inst);     // Writeback

    // Helper functions
    void printStages();
    void printBusyRegs();
    void printMDR();
    void printReport(); // Print the number of times each instruction type was executed

    // Pipeline control
    void stall(int cycles);         // Insert stall cycles
    void flush();                   // Flush pipeline after misprediction

    // Control functions
    void initBusyRegs();    // Initialize busy registers to 0
    void decrBusyRegs();    // Decrement busy registers by 1 (at end of cycle)

public:
    void moveStages(int bin);       // Move instructions to next stage
    int checkHalt(int bin);         // Determine if we have hit a halt instruction
    void run(instruction &inst);    // Run the pipeline (one instruction at a time for now)
    int getOpcode(int bin);         // Get the opcode from the binary instruction
    void printFields(instruction &inst); // Print the fields of the instruction
    void printExecutionReport(); // New public method
private:
    void parseInstruction(instruction &inst);
    void executeInstruction(instruction &inst);
    void Hazards();            // Check for data hazards
    void printBinary(int n);   // Print binary representation of int
    void defineInstSet();      // Define the instruction set
    void setInstruction(std::string instrName, int opcode, int type, bool addressMode);
    instruction getInstruction(int opcode); // Get instruction by opcode
};

#endif /* PIPELINE_H */

