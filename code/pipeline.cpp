

#include "pipeline.h"
#include "instructions.h"
#include <iostream>
#include <vector>

Pipeline::Pipeline() {
    numStalls = 0;      // Initialize number of stalls to 0
    stages.resize(5);   // 5 stages in pipeline
    initBusyRegs();     // Initialize busy registers
}

Pipeline::~Pipeline() {
}
<<<<<<< Updated upstream
=======
void Pipeline::initNOPs(void) {
    for (int i = 0; i < 5; i++) {
        stages[i].name = "NOP";
        stages[i].instr.opcode = 18;
        stages[i].instr.rs = 0;
        stages[i].instr.rt = 0;
        stages[i].instr.rd = 0;
        stages[i].instr.imm = 0;
    }
}


>>>>>>> Stashed changes

/**
 * @brief Initialize busy registers to 0
 * 
 */
void Pipeline::initBusyRegs(void) {
    for (int i = 0; i < 32; i++) {
        busyRegs[i] = 0;
    }
}

/**
 * @brief Decrement busy registers by 1 (at end of cycle)
 * 
 */
void Pipeline::decrBusyRegs(void) {
    for (int i = 0; i < 32; i++) {
        if (busyRegs[i] > 0) {
            busyRegs[i]--;
        }
    }
}

/**
 * @brief Move instructions to next stage
 * 
 */
void Pipeline::moveStages(int line) {
<<<<<<< Updated upstream
    stages[4] = stages[3]; // Move MEM to WB
    stages[3] = stages[2]; // Move EX to MEM
    stages[2] = stages[1]; // Move ID to EX
    stages[1] = stages[0]; // Move IF to ID
=======

    WB(stages[4]); // Writeback the instruction waiting in this stage (pulled from MEM last cycle)
    stages[4].assign(stages[3]); // Pull instruction from MEM to WB
    //cout << "WB: " << stages[4].name << endl;

    MEM(stages[3]); // Memory operation 
    stages[3].assign(stages[2]); // Pull instruction from EX to MEM
    //cout << "MEM: " << stages[3].name << endl;

    EX(stages[2]); // Execute instruction
    stages[2].assign(stages[1]); // Pull instruction from ID to EX
    //cout << "EX: " << stages[2].name << endl;
    
    ID(stages[1]); // Decode instruction
    stages[1].assign(stages[0]); // Pull instruction from IF to ID
    //cout << "ID: " << stages[1].name << endl;
>>>>>>> Stashed changes
    
    // call IF to fill IF stage
    IF(line);
}

/**
 * @brief Insert stall
 * 
 */
void Pipeline::stall(void) {
    numStalls++;
    // insert NOP(s) into stages

}

/**
 * @brief Flush pipeline after misprediction
 * 
 */
void Pipeline::flush(void) {
    // TODO: Implement
    std::cout << "Not implemented" << std::endl;

}

int Pipeline::getOpcode(int trace) {
    // Mask the top 6 bits to get the opcode
    int opcode = (trace & 0xFC000000) >> 26;
    return opcode;
}

/**
 * @brief Fetch instruction from memory
 * 
 */
void Pipeline::IF(int inputBin) { 
    int opcode = getOpcode(inputBin); // get the opcode from the line
    instruction inst = instructionSet.getInstruction(opcode);
    stages[0] = inst;
    status.typeExecd[inst.type]++; // update number of times instruction type was executed
    status.PC += 4; // increment program counter
    
}

void Pipeline::ID(int inputBin, instruction &inst) {

    if (inst.addressMode == 0) { // Immediate addressing
        inst.instr.opcode = getOpcode(inputBin);
        inst.instr.imm = inputBin & 0xFFFF;
        inst.instr.rt = (inputBin & 0x1F0000) >> 16;
        inst.instr.rs = (inputBin & 0x3E00000) >> 21;

        #ifdef DEBUG
        printf("%s R%d, R%d, #%d\n", inst.name.c_str(), inst.instr.rt, inst.instr.rs, inst.instr.imm);
        #endif
        
    } else { // Register addressing
      	inst.instr.opcode = getOpcode(inputBin);
        inst.instr.rs = (inputBin & 0x03E00000) >> 21;
        inst.instr.rt = (inputBin & 0x001F0000) >> 16;
        inst.instr.rd = (inputBin & 0x0000F800) >> 11;

        #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", inst.name.c_str(), inst.instr.rd, inst.instr.rs, inst.instr.rt);
        #endif
    }
<<<<<<< Updated upstream
   
=======

    // Sanity check if any of the registers are out of bounds
    int sanityCheck = 0;
    if(inst.instr.rs > 31) {
        cout << "Error: Register RS: " << inst.instr.rs << " out of bounds" << endl;
        // inst.name = "NOP";
        // exit(1);
        sanityCheck += 1;
    }
    if(inst.instr.rt > 31) {
        cout << "Error: Register RT: " << inst.instr.rt << " out of bounds" << endl;
        // inst.name = "NOP";
        // exit(1);
        sanityCheck += 2;
    }
    if(inst.instr.rd > 31) {
        cout << "Error: Register RD: " << inst.instr.rd << " out of bounds" << endl;
        // inst.name = "NOP";
        // exit(1);
        sanityCheck += 4;
    }
    if(inst.instr.imm > 65535) {
        cout << "Error: Immediate value " << inst.instr.imm << " out of bounds" << endl;
        // inst.name = "NOP";
        // exit(1);
        sanityCheck += 8;
    }

    if(sanityCheck > 0){
        printFields(inst);
        cout << endl;
        return;
    }

    // Otherwise
    cout << "[ID]: Instruction: " << inst.name << endl;
    stages[_ID] = inst;
    printFields(inst);
>>>>>>> Stashed changes
}

void Pipeline::EX(instruction &inst, Status &status) {
    switch(inst.instr.opcode) {
        // REGISTER
        case 0: // ADD
            ALUresult = status.registers[inst.instr.rs] + status.registers[inst.instr.rt]; 
	 break;
        case 2: // SUB
            ALUresult = status.registers[inst.instr.rs] - status.registers[inst.instr.rt];
            break;
        case 4: // MUL
            ALUresult = status.registers[inst.instr.rs] * status.registers[inst.instr.rt];
            break;
        case 6: // OR
            ALUresult = status.registers[inst.instr.rs] | status.registers[inst.instr.rt];
            break;
        case 8: // AND
            ALUresult = status.registers[inst.instr.rs] & status.registers[inst.instr.rt];
            break;
        case 10: // XOR
            ALUresult = status.registers[inst.instr.rs] ^ status.registers[inst.instr.rt];
            break;
        // IMMEDIATE
        case 1: // ADDI
            ALUresult = status.registers[inst.instr.rs] + inst.instr.imm;
            break;
        case 3: // SUBI
            ALUresult = status.registers[inst.instr.rs] - inst.instr.imm;
            break;
        case 5: // MULI
            ALUresult = status.registers[inst.instr.rs] * inst.instr.imm;
            break;
        case 7: // ORI
            ALUresult = status.registers[inst.instr.rs] | inst.instr.imm;
            break;
        case 9: // ANDI
            ALUresult = status.registers[inst.instr.rs] & inst.instr.imm;
            break;
        case 11: // XORI
            ALUresult = status.registers[inst.instr.rs] ^ inst.instr.imm;
            break;
        case 12: // LDW
            ALUresult = status.memory[inst.instr.imm];
            break;
        case 13: // STW
            ALUresult = status.registers[inst.instr.rt];
            break;
        case 14: // BZ
            if (status.registers[inst.instr.rs] == 0) {
                status.PC += inst.instr.imm * 4;
            }
            break;
        case 15: // BEQ
            if (status.registers[inst.instr.rt] == status.registers[inst.instr.rs]) {
                status.PC += inst.instr.imm * 4;
            }
            break;
        case 16: // JR
<<<<<<< Updated upstream
            status.PC += status.registers[inst.instr.rs] * 4;
=======
            status.PC += status.registers[inst.instr.rt] * 4;
            break;
        default:
            cout << "Error: Invalid opcode" << endl;
            inst.name += " Error" ;
            printFields(inst);
>>>>>>> Stashed changes
            break;
    }
}

void Pipeline::MEM(instruction &inst) {
    // Assign the result to the appropriate register
    if (inst.addressMode) {
        status.registers[inst.instr.rd] = ALUresult;
    } else{
        status.registers[inst.instr.rt] = ALUresult;
    }
    
    if(inst.instr.opcode == 12) {
    
    status.memory[MDR] = ALUresult;
    
    }
    
}


void Pipeline::WB(instruction &inst) {

if(inst.instr.opcode == 12) {

status.registers[inst.instr.rt] = status.memory[MDR];

}
else{
//nop
}
}


