#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <map>

/**
 * Type of instruction:
 * 0 = arithmetic
 * 1 = logical
 * 2 = memory access
 * 3 = control flow
 */
typedef struct {
    std::string name; // for print/debugging
    int type;
    bool addressMode; // 0 = immediate, 1 = register
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