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
    setInstruction("ADD", 0, 0, 1); // ADD
    setInstruction("SUB", 2, 0, 1); // SUB
    setInstruction("MUL", 4, 0, 1); // MUL
    setInstruction("OR", 6, 1, 1); // OR
    setInstruction("AND", 8, 1, 1); // AND
    setInstruction("XOR", 10, 1, 1); // XOR

    setInstruction("LDW", 12, 2, 0); // LDW
    setInstruction("STW", 13, 2, 0); // STW
    setInstruction("ADDI", 1, 0, 0); // ADDI
    setInstruction("SUBI", 3, 0, 0); // SUBI
    setInstruction("MULI", 5, 0, 0); // MULI
    setInstruction("ORI", 7, 1, 0); // ORI
    setInstruction("ANDI", 9, 1, 0); // ANDI
    setInstruction("XORI", 11, 1, 0); // XORI
    setInstruction("BEQ", 15, 3, 0); // BEQ
    setInstruction("BZ", 14, 3, 0); // BZ
    setInstruction("JR", 16, 3, 0); // JR
    setInstruction("HALT", 17, 3, 0); // HALT
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

void Instructions::setInstruction(std::string instrName, int opcode, int type, bool addressMode) {
    instruction inst;
    inst.name = instrName;
    inst.type = type;
    inst.addressMode = addressMode;

    instructionSet[opcode] = inst;
}
