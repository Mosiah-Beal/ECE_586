#include "instructions.h"

Instructions::Instructions() {
    defineInstSet();
}

void Instructions::defineInstSet() {
    /**
     * Type of instruction:
     * 0 = arithmetic
     * 1 = logical
     * 2 = memory access
     * 3 = control flow
     * 
     * Address mode:
     * 0 = immediate, 1 = register
     */
    setInstruction(0, 0, 1); // ADD
    setInstruction(2, 0, 1); // SUB
    setInstruction(4, 0, 1); // MUL
    setInstruction(6, 1, 1); // OR
    setInstruction(8, 1, 1); // AND
    setInstruction(10, 1, 1); // XOR

    setInstruction(12, 2, 0); // LDW
    setInstruction(13, 2, 0); // STW
    setInstruction(1, 0, 0); // ADDI
    setInstruction(3, 0, 0); // SUBI
    setInstruction(5, 0, 0); // MULI
    setInstruction(7, 1, 0); // ORI
    setInstruction(9, 1, 0); // ANDI
    setInstruction(11, 1, 0); // XORI
    setInstruction(15, 3, 0); // BEQ
    setInstruction(14, 3, 0); // BZ
    setInstruction(16, 3, 0); // JR
    setInstruction(17, 3, 0); // HALT
}

instruction Instructions::getInstruction(int opcode) {
    if (instructionSet.count(opcode) > 0) {
        return instructionSet.at(opcode);
    } else {
        // Handle the case where the opcode does not exist in the map
        // For example, you can return a default instruction
        instruction defaultInst;
        defaultInst.type = -1;
        defaultInst.addressMode = -1;
        return defaultInst;
    }
}

void Instructions::setInstruction(int opcode, int type, bool addressMode) {
    instruction inst;
    inst.type = type;
    inst.addressMode = addressMode;

    instructionSet[opcode] = inst;
}