#ifndef PIPELINE_S_H
#define PIPELINE_S_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;

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
} Bitmap;

// Used to parse instruction and update type counter
typedef struct {
    string name;            // for print/debugging
    string originalLine;    // original line from file
    int bin_bitmap ;        // Binary representation of instruction
    int instructionNumber;  // When this instruction was fetched

    Bitmap* bitmap;         // Struct for decoded instruction    
    bool addressMode;       // 0 = immediate, 1 = register
    int len;                // How many cycles this instruction takes
    int type;
    /**
     * Type of instruction:
     * 0 = arithmetic
     * 1 = logical
     * 2 = memory access
     * 3 = control flow
     */
} instr_metadata;

class Pipeline {

private:
 
    //members
    int numStalls;    // Number of stalls inserted
    int ALUresult;    // Result of ALU operation
    int MDR;  // Memory Destination Register (load memory MEM stage)
    int PC;   // Program Counter
    int instrFetched;   // Number of instructions fetched
    int clk;

    bool flushFlag;         // Flag to flush pipeline
    bool stallCondition;    // Flag to stall pipeline
    bool criticalProblem;   // Flag to halt pipeline
    int forwardingFlag;
   

    std::vector<int> registers;  // Registers
    std::vector<int> typeExecd;   // Number of times each instruction type was executed
    
    //maps
    public: 
    std::map<int, int> changedMemory;               // Memory <address, value>
    std::map<int, instr_metadata> instructionSet;   // Instruction set
    std::map<int, std::string> instructionMemory;   // Instruction memory <address, instruction>
    std::map<int, int> opcodeHeatMap;               // <Opcode, number of times executed>

    private:

    // Stage functions
    void IF(int inputBin);          // Instruction Fetch
    void ID(instr_metadata &metadata);     // Decode
    void EX(instr_metadata &metadata);     // Execute
    void MEM(instr_metadata &metadata);    // Memory
    void WB(instr_metadata &metadata);     // Writeback

    // Helper functions
    void printStages();
    void printBusyRegs();   // Print the busy registers
    void printMDR();
    void printReport();     // Print the number of times each instruction type was executed
    void printFields(instr_metadata &metadata); // Print the fields of the instruction
    void printInstruction(instr_metadata &metadata); // Print the instruction

    // Pipeline control
    void stall();                   // Insert stall cycles
    void flush();                   // Flush pipeline after misprediction

    // Control functions
    void initBusyRegs();    // Initialize busy registers to 0
    void setBusyRegs(instr_metadata &metadata); // Set the destination register as busy
    void decrBusyRegs();    // Decrement busy registers by 1 (at end of cycle)
    instr_metadata parseInstruction(instr_metadata &metadata);
    instr_metadata  executeInstruction(instr_metadata &metadata);
    void Hazards();            // Check for data hazards
    void printBinary(int n);   // Print binary representation of int
    void defineInstSet();      // Define the instruction set
    void setInstruction(std::string instrName, int opcode, int type, bool addressMode, int len);
    int getOpcode(int bin);         // Get the opcode from the binary instruction
    instr_metadata getInstruction(int opcode); // Get instruction by opcode
    void internalPipeMove(int sourceIndex, int destIndex); // Move instruction from source to dest stage

    public:
    //constructor + deconstructor
    Pipeline(int mode);
    ~Pipeline();

    //user functions
    void moveStages(int bin);       // Move instructions to next stage
    int checkHalt(int bin);         // Determine if we have hit a halt instruction
    void run();                     // Run the pipeline (one instruction at a time for now)
    void printExecutionReport();    // print public method
    void initNOPs(); // Initialize NOPs in stages
    void setForwardingFlag(int mode);
    //public maps + vectors
    std::vector<instr_metadata> stages;    // Holds instructions in each stage
    std::map<int, int> busyRegs;        // Holds register values in use (sta    ll cycles needed)
};
#endif /* PIPELINE_H */
