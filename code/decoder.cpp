#include "decoder.h"

// #define DEBUG

Decoder::Decoder() {
}

int Decoder::parseNextLine(std::string fileName) {
    std::ifstream traceFile(fileName);
    
    if (!traceFile) {
        std::cerr << "Error: could not open file " << fileName << std::endl;
        return 1;
    }

    std::string traceStr;
    while (std::getline(traceFile, traceStr)) {
        int trace = std::stoi(traceStr, 0, 16);
        int opcode = getOpcode(trace);
        // instruction inst = instructions.getInstruction(opcode);
        // std::cout << "Type: " << inst.type << ", Address mode: " << inst.addressMode << std::endl;

        #ifdef DEBUG
        printf("Trace: ");
        printBinary(trace);
        #endif
    }
}

void Decoder::printBinary(int n) {
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2) {
        (n & i) ? printf("1") : printf("0");
    }
    printf("\n");
}

int Decoder::getOpcode(int trace) {
    int opcode = (trace & 0xFC000000) >> 26;

    #ifdef DEBUG
    printf("Opcode: ");
    printBinary(opcode);
    #endif

    return opcode;
}