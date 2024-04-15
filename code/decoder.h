/**
 * @file decoder.h
 * @author Lynn (diemhan@pdx.edu)
 * @brief 
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 * This file contains the Decoder class, which is responsible for anything related to decoding and executing the instructions.
 * This includes parsing the instructions, determining the opcode, and any operations necessary to change the machine's state
 * per the executed instruction.
 */
#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "instructions.h"
#include "status.h"

// Holds all register and immediate values for a given instruction
// Depending on format, some fields will be unused
typedef struct {
    int opcode; // op code -- determines operation
    int rs;     // source register
    int rt;     // temp register
    int rd;     // destination register
    int imm;    // immediate value
} InstrInfo;


class Decoder
{
    public:
        Decoder();
        int nextInstr(int trace, Status &status);
    private:
        void printBinary(int n); // used for debugging
        int getOpcode(int trace);
        void parseInstruction(int line, instruction inst, InstrInfo &execInstr);
        void executeInstruction(InstrInfo &execInstr, Status &status);
};
#endif // DECODER_H