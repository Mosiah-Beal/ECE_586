

#include "pipeline.h"
#include "instructions.h"
#include <iostream>

using namespace std;

Pipeline::Pipeline() {
    numStalls = 0;      // Initialize number of stalls to 0
    stages.resize(5);   // 5 stages in pipeline
    // initalize all stages to NOP
    for (int i = 0; i < 5; i++) {
        stages[i].name = "NOP";
    }
    initBusyRegs();     // Initialize busy registers
    
}

Pipeline::~Pipeline() {
}

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
 * The pipeline moves from the back to the front, so the Writeback stage executes first, then the Memory stage, etc.
 * After each stage has executed, the next instruction is pulled forward in preparation for the next cycle.
 * 
 */
void Pipeline::moveStages(int line) {

    WB(stages[4]); // Writeback the instruction waiting in this stage (pulled from MEM last cycle)
    stages[4] = stages[3]; // Pull instruction from MEM to WB
    cout << "WB: " << stages[4].name << endl;

    MEM(stages[3]); // Memory operation 
    stages[3] = stages[2]; // Pull instruction from EX to MEM
    cout << "MEM: " << stages[3].name << endl;

    EX(stages[2]); // Execute instruction
    stages[2] = stages[1]; // Pull instruction from ID to EX
    cout << "EX: " << stages[2].name << endl;
    
    ID(stages[1]); // Decode instruction
    stages[1] = stages[0]; // Pull instruction from IF to ID
    cout << "ID: " << stages[1].name << endl;
    
    // call IF to fill IF stage
    IF(line);
    cout << "IF: " << stages[0].name << endl;
}

void Pipeline::Hazards(void) {

    // The instruction in the decode stage has registers which need to be checked for dependencies
    int rsBusy = busyRegs[stages[_ID].instr.rs];
    int rtBusy = busyRegs[stages[_ID].instr.rt];
    int rdBusy = busyRegs[stages[_ID].instr.rd];
    
    // Check if any of the registers are in use
    if (rsBusy > 0 || rtBusy > 0 || rdBusy > 0) {
        printf("RS: %d, RT: %d, RD: %d\n", rsBusy, rtBusy, rdBusy);
        
        // Stall the pipeline until the busiest register is free

        // Assume RS at first
        int maxBusy = rsBusy;
        if (rtBusy > maxBusy) {
            maxBusy = rtBusy;
            // If RT is busier than RS, set maxBusy to RT
            // Now check if RD is busier than RT
            if (rdBusy > maxBusy) {
                maxBusy = rdBusy;
            }
        }
        else if(rdBusy > maxBusy) {
            maxBusy = rdBusy;
        }
        stall(maxBusy);
    } 

}



/**
 * @brief Insert stall
 * 
 */
void Pipeline::stall(int cycles) {

    for(int i = 0; i < cycles; i++) {
        //TODO: Insert NOP(s) into stages
        numStalls++;
    }
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
    return trace >> 26;
}

/**
 * @brief Fetch instruction from memory
 * 
 */
void Pipeline::IF(int inputBin) {
    int opcode = getOpcode(inputBin); // get the opcode from the line
    instruction inst = instructionSet.getInstruction(opcode);   // Fill the metadata for the instruction
    inst.binInstr = inputBin;   // store binary representation of instruction
    stages[0] = inst;

}

void Pipeline::parseInstruction(instruction &inst) {
    if (inst.addressMode == 0) { // Immediate addressing
        inst.instr.opcode = getOpcode(inst.binInstr);
        inst.instr.rs = (inst.binInstr & 0x3E00000) >> 21;
        inst.instr.rt = (inst.binInstr & 0x1F0000) >> 16;
        inst.instr.imm = inst.binInstr & 0xFFFF;

        // Check if the immediate value is negative. If so, invert the bits and add 1 to get 2s complement
        if (inst.instr.imm & 0x8000) {
            inst.instr.imm = ~inst.instr.imm + 1;
        }

        #ifdef DEBUG
        printf("%s R%d, R%d, #%d\n", inst.name.c_str(), inst.instr.rt, inst.instr.rs, inst.instr.imm);
        #endif
    } else { // Register addressing
        inst.instr.opcode = getOpcode(inst.binInstr);
        inst.instr.rs = (inst.binInstr & 0x03E00000) >> 21;
        inst.instr.rt = (inst.binInstr & 0x001F0000) >> 16;
        inst.instr.rd = (inst.binInstr & 0x0000F800) >> 11;

        #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", inst.name.c_str(), inst.instr.rd, inst.instr.rs, inst.instr.rt);
        #endif
    }
}

void Pipeline::executeInstruction(instruction &inst) {
    // Check for hazards
    // Hazards();    
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
        
        // CONTROL FLOW
        case 14: // BZ
            if (status.registers[inst.instr.rt] == 0) {
                status.PC += inst.instr.imm * 4;
            }
            break;
        case 15: // BEQ
            if (status.registers[inst.instr.rt] == status.registers[inst.instr.rs]) {
                status.PC += inst.instr.imm * 4;
            }
            break;
        case 16: // JR
            status.PC += status.registers[inst.instr.rt] * 4;
            break;
    }

}

void Pipeline::ID(instruction &inst) {
    parseInstruction(inst);

    // Display the fields of the instruction
    cout << "\t\tOpcode: " << inst.instr.opcode << endl;
    cout << "\t\tRS: " << inst.instr.rs << endl;
    cout << "\t\tRT: " << inst.instr.rt << endl;
    if(inst.addressMode == 0) {
        cout << "\t\tIMM: " << inst.instr.imm << endl;
    } else {
        cout << "\t\tRD: " << inst.instr.rd << endl;
    }

}

void Pipeline::EX(instruction &inst) {
    // Check for nop
    if (inst.name == "NOP") {
        return;
    }
    
    // Check for hazards
    // Hazards();

    // Execute the instruction
    executeInstruction(inst);

    // Update status
    std::cout << "Type: " << inst.type << std::endl;
    status.typeExecd[inst.type]++; // update number of times instruction type was executed
    status.PC += 4; // increment program counter

    #ifdef DEBUG
    printf("(%d) ", status.PC);
    #endif

}

void Pipeline::MEM(instruction &inst) {
    // Check for nop
    if (inst.name == "NOP") {
        return;
    }

    if (inst.instr.opcode == 12) { // LDW
        cout << "[MEM] MDR: " << MDR << endl;
        MDR = ALUresult;
    }

    if (inst.addressMode == 0) { // Immediate addressing
        status.registers[inst.instr.rt] = ALUresult;
    } else { // Register addressing
        status.registers[inst.instr.rd] = ALUresult;
    }
}

void Pipeline::WB(instruction &inst) {
    // Check for nop
    if (inst.name == "NOP") {
        return;
    }

    if (inst.instr.opcode == 12) {
        status.memory[inst.instr.rt] = status.memory[MDR];
    }
}

/**
 * @brief Used to check bit-wise operations on ints
 * 
 * @param n int to be printed in binary
 */
void Pipeline::printBinary(int n) {
    unsigned i;
    unsigned count = 0;
    for (i = 1 << 31; i > 0; i = i / 2) {
        (n & i) ? printf("1") : printf("0");
        
        // Add spaces every 4 bits for readability
        if(++count % 4 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}


/**
 * @brief The highest level function for the decoder. This function takes in a line from the input 
 * file, parses it, and changes the processor's state to match the executed instruction
 * 
 * @param line next instruction (in int) to be decoded
 * @param status class that keeps track of the processor's state, including registers, memory and PC
 * @return int 
 */
int Pipeline::checkHalt(int bin) {
    
    if (getOpcode(bin) == 17) {
        return 0; // end program after HALT instruction is found
    }
    else {
        return 1; // otherwise, continue
    }
}

void Pipeline::run(instruction &inst) {
    /**
    // End if HALT instruction is found
    if (checkHalt(stages[0].binInstr) == 0) {
        return;
    }

    // Move instructions to next stage
    moveStages(stages[0].binInstr);

    // Execute instruction
    EX(stages[2]);

    // Move instructions to next stage
    moveStages(stages[0].binInstr);

    // Memory
    MEM(stages[3]);

    // Move instructions to next stage
    moveStages(stages[0].binInstr);

    // Writeback
    WB(stages[4]);

    // Move instructions to next stage
    moveStages(stages[0].binInstr);

    // Increment program counter
    status.PC += 4;
    */

}