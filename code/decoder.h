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
        int parseTraceFile(std::string fileName);
    private:
        void printBinary(int n); // used for debugging
        int getOpcode(int trace);
};

#endif // DECODER_H