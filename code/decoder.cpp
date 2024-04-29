#include "decoder.h"

#define DEBUG

Decoder::Decoder() {
}

/**
 * @brief Used to check bit-wise operations on ints
 * 
 * @param n int to be printed in binary
 */
void Decoder::printBinary(int n) {
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2) {
        (n & i) ? printf("1") : printf("0");
    }
    printf("\n");
}

/**
 * @brief The highest level function for the decoder. This function takes in a line from the input 
 * file, parses it, and changes the processor's state to match the executed instruction
 * 
 * @param line next instruction (in int) to be decoded
 * @param status class that keeps track of the processor's state, including registers, memory and PC
 * @return int 
 */
int Decoder::nextInstr(int line, Status &status) {
    Instructions instructions;
    InstrInfo execInstr;
    int opcode;
    
    opcode = getOpcode(line); // get the opcode from the line
    instruction inst = instructions.getInstruction(opcode);

    status.typeExecd[inst.type]++; // update number of times instruction type was executed
    status.PC += 4; // increment program counter

    #ifdef DEBUG
    printf("(%d) ", status.PC);
    #endif


    parseInstruction(line, inst, execInstr);
    executeInstruction(execInstr, status);

    if (opcode == 17) {
        return 0; // end program after HALT instruction is found
    } else {
        return 1; // otherwise, continue
    }
}

/**
 * @brief Calculates the opcode of a given instruction
 * 
 * @param trace the encoded instruction to extract the op code from
 * @return int op code
 */
int Decoder::getOpcode(int trace) {
    int opcode = trace >> 26;
    return opcode;
}

/**
 * @brief This instruction parses a given instruction, extracting immediate values, registers to
 * to be operated on, and the operation to be executed
 * 
 * @param line encoded instruction (in int, decimal representation) to be parsed
 * @param inst instruction struct that the "metadata" (type, addressing mode) for the type of instruction (i.e ADD, ADDI)
 * @param execInstr InstrInfo struct that will contain the register and immediate values for a specific instruction (line in the trace file)
 */
void Decoder::parseInstruction(int line, instruction inst, InstrInfo &execInstr) {

    if (inst.addressMode == 0) { // Immediate addressing
        execInstr.opcode = getOpcode(line);
        execInstr.imm = line & 0xFFFF;
        execInstr.rt = (line & 0x1F0000) >> 16;
        execInstr.rs = (line & 0x3E00000) >> 21;

        #ifdef DEBUG
        printf("%s R%d, R%d, #%d\n", inst.name.c_str(), execInstr.rt, execInstr.rs, execInstr.imm);
        #endif
    } else { // Register addressing
        execInstr.opcode = getOpcode(line);
        execInstr.rs = (line & 0x03E00000) >> 21;
        execInstr.rt = (line & 0x001F0000) >> 16;
        execInstr.rd = (line & 0x0000F800) >> 11;

        #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", inst.name.c_str(), execInstr.rd, execInstr.rs, execInstr.rt);
        #endif
    }
}

/**
 * @brief This function is what actually performs the necessary operations to "execute" an instruction (i.e adding
 * values for ADD/ADDI instructions, then changing the processor's registers, PC and state accordingly)
 * 
 * @param execInstr struct containing all the immediate and register values for the given instruction
 * @param status class that keeps track of the machine's state
 */
void Decoder::executeInstruction(InstrInfo &execInstr, Status &status) {
    switch(execInstr.opcode) {
        // REGISTER
        case 0: // ADD
            status.registers[execInstr.rd] = status.registers[execInstr.rs] + status.registers[execInstr.rt];
            break;
        case 2: // SUB
            status.registers[execInstr.rd] = status.registers[execInstr.rs] - status.registers[execInstr.rt];
            break;
        case 4: // MUL
            status.registers[execInstr.rd] = status.registers[execInstr.rs] * status.registers[execInstr.rt];
            break;
        case 6: // OR
            status.registers[execInstr.rd] = status.registers[execInstr.rs] | status.registers[execInstr.rt];
            break;
        case 8: // AND
            status.registers[execInstr.rd] = status.registers[execInstr.rs] & status.registers[execInstr.rt];
            break;
        case 10: // XOR
            status.registers[execInstr.rd] = status.registers[execInstr.rs] ^ status.registers[execInstr.rt];
            break;
        // IMMEDIATE
        case 1: // ADDI
            status.registers[execInstr.rt] = status.registers[execInstr.rs] + execInstr.imm;
            break;
        case 3: // SUBI
            status.registers[execInstr.rt] = status.registers[execInstr.rs] - execInstr.imm;
            break;
        case 5: // MULI
            status.registers[execInstr.rt] = status.registers[execInstr.rs] * execInstr.imm;
            break;
        case 7: // ORI
            status.registers[execInstr.rt] = status.registers[execInstr.rs] | execInstr.imm;
            break;
        case 9: // ANDI
            status.registers[execInstr.rt] = status.registers[execInstr.rs] & execInstr.imm;
            break;
        case 11: // XORI
            status.registers[execInstr.rt] = status.registers[execInstr.rs] ^ execInstr.imm;
            break;
        case 12: // LDW
            status.registers[execInstr.rt] = status.memory[execInstr.imm];
            break;
        case 13: // STW
            status.memory[execInstr.imm] = status.registers[execInstr.rt];
            break;
        case 14: // BZ
            if (status.registers[execInstr.rt] == 0) {
                status.PC += execInstr.imm * 4;
            }
            break;
        case 15: // BEQ
            if (status.registers[execInstr.rt] == status.registers[execInstr.rs]) {
                status.PC += execInstr.imm * 4;
            }
            break;
        case 16: // JR
            status.PC += status.registers[execInstr.rt] * 4;
            break;
    }
}