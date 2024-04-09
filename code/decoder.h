#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "instructions.h"

class Decoder
{
    public:
        Decoder();
        int parseInstr(int trace);

        void printReport(); // print the number of times each instruction type was executed
        int typeExecd[4] = {0, 0, 0, 0}; // number of times each instruction type was executed
    private:
        void printBinary(int n); // used for debugging
        int getOpcode(int trace);
        void splitInstruction(int line, instruction inst);
};

#endif // DECODER_H