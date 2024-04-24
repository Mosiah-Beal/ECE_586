/**
 * @file instructions.h
 * @author Lynn (diemhan@pdx.edu)
 * @brief 
 * @version 0.1
 * @date 2024-04-09
 * 
 * @copyright Copyright (c) 2024
 * 
 * This file includes the Instructions class, which is responsible for defining the instruction set. This is used to determine how
 * to parse and execute the instructions.
 */
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <map>
#include <string>

// Used to parse instruction and update type counter
typedef struct {
    std::string name; // for print/debugging
    bool addressMode; // 0 = immediate, 1 = register
    int type;
    /**
     * Type of instruction:
     * 0 = arithmetic
     * 1 = logical
     * 2 = memory access
     * 3 = control flow
     */
} instruction;

class Instructions
{
    public:
        Instructions();
        instruction getInstruction(int opcode);

    private:
        std::map<int, instruction> instructionSet;
        void defineInstSet();
        void setInstruction(std::string instrName, int opcode, int type, bool addressMode);
};

#endif // INSTRUCTIONS_H