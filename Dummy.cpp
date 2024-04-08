#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
using namespace std;

// 32 bit instruction
typedef struct Instruction {
    int opcode;     // 6 bits
    int rs;         // 5 bits
    int rt;         // 5 bits
    int rd;     // R-type exclusive (5 bits)
    int imm;    // I-type exclusive (16 bits)
} Instruction_t;

// Typedefine function pointer
typedef void (*FuncPtr)(Instruction_t*);

// Vector of 32 bit instructions to simulate the program
vector<Instruction_t> program;

// Vector of 32 registers to simulate the CPU
vector<uint32_t> registers(32, 0);

// Vector of 5 volatile pointers to hold the instructions in the 5 stage pipeline
vector<Instruction_t*> pipeline(5, nullptr);

// Define the stages of the pipeline
void fetch() {
    // Fetch instruction from memory
}

void decode() {
    // Decode the instruction
}

void execute() {
    // Execute the instruction
}

void memory() {
    // Access memory if needed
}

void writeback() {
    // Write the result back to a register
}

int main() {
    // Create a map of opcodes to function pointers
    std::map<int, FuncPtr> opcode_map;

    // Associate opcodes with functions
    opcode_map[0b000000] = ADD;
    opcode_map[0b000001] = ADDI;
    opcode_map[0b000010] = SUB;
    opcode_map[0b000011] = SUBI;
    opcode_map[0b000100] = MUL;
    opcode_map[0b000101] = MULI;
    opcode_map[0b000110] = OR;
    opcode_map[0b000111] = ORI;
    opcode_map[0b001000] = AND;
    opcode_map[0b001001] = ANDI;
    opcode_map[0b001010] = XOR;
    opcode_map[0b001011] = XORI;
    opcode_map[0b001100] = LDW;
    opcode_map[0b001101] = STW;
    opcode_map[0b001110] = BZ;
    opcode_map[0b001111] = BEQ;
    opcode_map[0b010000] = JR;
    opcode_map[0b010001] = HALT;

    // Simulate the program 
    for (auto instruction : program) {
        // Fetch the instruction
        pipeline[0] = &instruction;

        // Decode the instruction
        pipeline[1] = pipeline[0];
        pipeline[0] = nullptr;

        // Execute the instruction
        pipeline[2] = pipeline[1];
        pipeline[1] = nullptr;

        // Access memory if needed
        pipeline[3] = pipeline[2];
        pipeline[2] = nullptr;

        // Write the result back to a register
        pipeline[4] = pipeline[3];
        pipeline[3] = nullptr;

        // Call the function associated with the opcode
        opcode_map[instruction.opcode](pipeline[4]);
    }

    return 0;
}

/***************************
 * Arithmetic Instructions *
 **************************/

void ADD(Instruction_t* instruction) {
    // Add the values in registers rs and rt and store the result in rd    
}

void ADDI(Instruction_t* instruction) {
    // Add the value in register rs to the immediate value and store the result in rt
}

void SUB(Instruction_t* instruction) {
    // Subtract the value in register rs from the value in register rt and store the result in rd
}

void SUBI(Instruction_t* instruction) {
    // Subtract the immediate value from the value in register rs and store the result in rt
}

void MUL(Instruction_t* instruction) {
    // Multiply the values in registers rs and rt and store the result in rd
}

void MULI(Instruction_t* instruction) {
    // Multiply the value in register rs by the immediate value and store the result in rt
}


/************************
 * Logical Instructions *
 ***********************/

void OR(Instruction_t* instruction) {
    // Perform a bitwise OR on the values in registers rs and rt and store the result in rd
}

void ORI(Instruction_t* instruction) {
    // Perform a bitwise OR on the value in register rs and the immediate value and store the result in rt
}

void AND(Instruction_t* instruction) {
    // Perform a bitwise AND on the values in registers rs and rt and store the result in rd
}

void ANDI(Instruction_t* instruction) {
    // Perform a bitwise AND on the value in register rs and the immediate value and store the result in rt
}

void XOR(Instruction_t* instruction) {
    // Perform a bitwise XOR on the values in registers rs and rt and store the result in rd
}

void XORI(Instruction_t* instruction) {
    // Perform a bitwise XOR on the value in register rs and the immediate value and store the result in rt
}


/***********************
 * Memory Instructions *
 ***********************/

void LDW(Instruction_t* instruction) {
    // Load a word from memory into register rt
}

void STW(Instruction_t* instruction) {
    // Store the value in register rt to memory
}


/************************
 * Control Instructions *
 ***********************/

void BZ(Instruction_t* instruction) {
    // Branch if the value in register rs is zero
}

void BEQ(Instruction_t* instruction) {
    // Branch if the values in registers rs and rt are equal
}

void JR(Instruction_t* instruction) {
    // Jump to the address in register rs
}

void HALT(Instruction_t* instruction) {
    // Halt the program
}


/************************
 * Simulator Components *
 ***********************/

void fetch() {
    // Fetch instruction from memory
}

void decode() {
    // Decode the instruction
}

void execute() {
    // Execute the instruction
}

void memory() {
    // Access memory if needed
}

void writeback() {
    // Write the result back to a register
}

void trace_reader() {
    //Reads the memory image and passes the next instruction to the instruction decoder
}

void instruction_decoder() {
    // Interprets instruction type, determines the source and destination registers
}

void functional_simulator() {
    // Simulates instruction behavior, keeps track of register and memory state changes
}

void pipeline_simulator() {
    // Keeps track of current clock cycle
    // Maintains track of instruction in each pipeline stage in each cycle
    // Identify different sources of stalls and hazards
    // Propagates instructions from one pipeline stage to next
}
