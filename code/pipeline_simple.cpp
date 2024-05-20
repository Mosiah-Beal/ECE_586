#include "pipeline_simple.h"

/**
 * @brief Construct a new Pipeline::Pipeline object
 */
Pipeline::Pipeline() {
    numStalls = 0;ls
    
    ALUresult = 0;
    MDR = 0;
    PC = 0;
    std::fill_n(registers, 32, 0);
    std::fill_n(typeExecd, 4, 0);
    memory.clear();
    defineInstSet();
}

/**f
 * @brief Destroy the Pipeline::Pipeline object
 */
Pipeline::~Pipeline() {}

/**
 * @brief Initialize NOPs in stages
 */
void Pipeline::initNOPs() {
    // Implementation for initializing NOPs
}

/**
 * @brief Define the instruction set
 */
void Pipeline::defineInstSet() {
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

/**
 * @brief Set an instruction in the instruction set
 * 
 * @param instrName the instruction as it appears in assembly code (i.e ADD, ADDI)
 * @param opcode opcode in decimal form (int)
 * @param type 0 = arithmetic, 1 = logical, 2 = memory access, 3 = control flow
 * @param addressMode 0 = immediate, 1 = register
 */
void Pipeline::setInstruction(std::string instrName, int opcode, int type, bool addressMode) {
    instruction inst;
    inst.name = instrName;
    inst.type = type;
    inst.addressMode = addressMode;
    inst.instr = InstrInfo();
    instructionSet[opcode] = inst;
}

/**
 * @brief Get instruction by opcode
 * 
 * @param opcode the instruction's op code
 * @return instruction -- struct with information about the instruction's name, addressing mode and type
 */
instruction Pipeline::getInstruction(int opcode) {
    if (instructionSet.count(opcode) > 0) {
        return instructionSet.at(opcode);
    } else {
        instruction defaultInst;
        defaultInst.type = -1;
        defaultInst.addressMode = -1;
        return defaultInst;
    }
}

/**
 * @brief Print the number of times each instruction type was executed
 */
void Pipeline::printReport() {
    std::cout << std::endl << "Execution Report:" << std::endl;
    std::cout << "Arithmetic instructions executed: " << typeExecd[0] << std::endl;
    std::cout << "Logical instructions executed: " << typeExecd[1] << std::endl;
    std::cout << "Memory access instructions executed: " << typeExecd[2] << std::endl;
    std::cout << "Control flow instructions executed: " << typeExecd[3] << std::endl;

    std::cout << "Program Counter: " << PC << std::endl;
    
    std::cout << "Registers:" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "R" << i << ": " << registers[i] << std::endl;
    }

    std::cout << "Memory Contents:" << std::endl;
    for (const auto& entry : memory) {
        std::cout << "Memory[" << entry.first << "]: " << entry.second << std::endl;
    }
    std::cout << std::endl;
}

void Pipeline::printExecutionReport() {
    printReport();
}

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
    
    // call IF to fill IF stage
    IF(line);
    //cout << "IF: " << stages[0].name << endl << endl;
    cout << endl;
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
    stages[_IF] = inst; // store instruction in IF stage

}

void Pipeline::parseInstruction(instruction &inst) {
    if (inst.addressMode == 0) { // Immediate addressing
        inst.instr.opcode = getOpcode(inst.binInstr);
        inst.instr.rs = (inst.binInstr & 0x3E00000) >> 21;
        inst.instr.rt = (inst.binInstr & 0x1F0000) >> 16;
        inst.instr.rd = 0; // No destination register for immediate addressing
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
        inst.instr.imm = 0; // No immediate value for register addressing

        #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", inst.name.c_str(), inst.instr.rd, inst.instr.rs, inst.instr.rt);
        #endif
    }

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
        default:
            cout << "Error: Invalid opcode" << endl;
            inst.name += " Error" ;
            printFields(inst);
            break;
    }

    cout << "[EX]: Instruction: " << inst.name << endl;
}

void Pipeline::ID(instruction &inst) {
    // check for NOP (initialization)
    if (inst.name == "NOP") {
        cout << "[ID]: NOP" << endl;
        return;
    }
    
    parseInstruction(inst);

    #ifdef DEBUG
        // Display the fields of the instruction
        cout << "\t\tOpcode: " << inst.instr.opcode << endl;
        cout << "\t\tRS: " << inst.instr.rs << endl;
        cout << "\t\tRT: " << inst.instr.rt << endl;
        if(inst.addressMode == 0) {
            cout << "\t\tIMM: " << inst.instr.imm << endl;
        } else {
            cout << "\t\tRD: " << inst.instr.rd << endl;
        }
    #endif

}

void Pipeline::EX(instruction &inst) {
    // Check for nop
    if (inst.name == "NOP") {
        cout << "[EX]: NOP" << endl;
        return;
    }
    
    // Check for hazards
    // Hazards();

    // Execute the instruction
    executeInstruction(inst);

    // Update status
    // cout << "Type: " << inst.type << endl;
    status.typeExecd[inst.type]++; // update number of times instruction type was executed
    status.PC += 4; // increment program counter

    #ifdef DEBUG
    printf("(%d) ", status.PC);
    #endif

}

void Pipeline::MEM(instruction &inst) {
    // Check for nop
    if (inst.name == "NOP") {
        cout << "[MEM]: NOP" << endl;
        return;
    }

    printFields(inst);

    if (inst.instr.opcode == 12) { // LDW
        cout << "[MEM] MDR: " << MDR << endl;
        MDR = ALUresult;
    }

    if (inst.addressMode == 0) { // Immediate addressing
        status.registers[inst.instr.rt] = ALUresult;
    } else { // Register addressing
        cout << "[MEM] ALUresult: " << ALUresult << endl;
        status.registers[inst.instr.rd] = ALUresult;
    }

    cout << "[MEM] Instruction: " << inst.name << endl;
}

void Pipeline::WB(instruction &inst) {
    // Check for nop
    if (inst.name == "NOP") {
        cout << "[WB]: NOP" << endl;
        return;
    }

    if (inst.instr.opcode == 12) {
        status.memory[inst.instr.rt] = status.memory[MDR];
    }

    cout << "[WB] Instruction: " << inst.name << endl;
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


void Pipeline::printFields(instruction &inst) {
    cout << "Instruction: " << inst.name << endl;
    cout << "Opcode: " << inst.instr.opcode << endl;
    cout << "RS: " << inst.instr.rs << endl;
    cout << "RT: " << inst.instr.rt << endl;
    cout << "RD: " << inst.instr.rd << endl;
    cout << "IMM: " << inst.instr.imm << endl;
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




