#include "decoder.h"

#define DEBUG

Decoder::Decoder() {
}

void Decoder::printBinary(int n) {
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2) {
        (n & i) ? printf("1") : printf("0");
    }
    printf("\n");
}

int Decoder::parseInstr(int line) {
    Instructions instructions;
    int opcode;
    
    opcode = getOpcode(line); // get the opcode from the line
    instruction inst = instructions.getInstruction(opcode); // which instruction is it?
    splitInstruction(line, inst); // "execute" the instruction
    
    // End program after HALT instruction is found
    if (opcode == 17) {
        return 0;
    }
    
    return 1; 
}

int Decoder::getOpcode(int trace) {
    int opcode = (trace & 0xFC000000) >> 26;

    #ifdef DEBUG
    // printf("Opcode: ");
    // printBinary(opcode);
    #endif

    return opcode;
}

void Decoder::splitInstruction(int line, instruction inst) {
    typeExecd[inst.type]++; // increment the number of times this instruction type was executed

    if (inst.addressMode == 0) { // Immediate addressing
        int imm = line & 0xFFFF;
        int rt = (line & 0x1F0000) >> 16;
        int rs = (line & 0x3E00000) >> 21;

        #ifdef DEBUG
        printf("%s R%d, R%d, #%d\n", inst.name.c_str(), rt, rs, imm);
        #endif
    } else {
        // register
        int rs = (line & 0x03E00000) >> 21;
        int rt = (line & 0x001F0000) >> 16;
        int rd = (line & 0x0000F800) >> 11;

        #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", inst.name.c_str(), rd, rs, rt);
        #endif
    }
}

void Decoder::printReport() {
    std::cout << "Arithmetic instructions executed: " << typeExecd[0] << std::endl;
    std::cout << "Logical instructions executed: " << typeExecd[1] << std::endl;
    std::cout << "Memory access instructions executed: " << typeExecd[2] << std::endl;
    std::cout << "Control flow instructions executed: " << typeExecd[3] << std::endl;
}