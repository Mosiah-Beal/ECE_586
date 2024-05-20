/**
Table of Contents 
Section 1 Stage functions line 36
IF line 38
ID line 
EX line

Section 2 Helper functions
Section 3 Pipeline control
Section 4 Control functions
Section 5 user functions
**/


#include "pipeline_s.h"
#include <iostream>
using namespace std;
#include <vector>

//constructor (public) 
//this initializes the Pipeline class
Pipeline::Pipeline() {
    defineInstSet();
    numStalls = 0;      // Initialize number of stalls to 0
    stages.resize(5);   // 5 stages in pipeline
    initBusyRegs();     // Initialize busy registers
    initNOPs();         // Initialize NOPs in stages
    
}

//deconstructor (public)
//this will delete the Pipeline class
Pipeline::~Pipeline() {
}


/***************************************
NOTE: SECTIONS 1-4 are PRIVATE functions
****************************************/

/*--------------------------------------------------------------------------------------------
SECTION 1 Stage functions
--------------------------------------------------------------------------------------------*/

// Instruction Fetch
void Pipeline::IF(int inputBin) {
    int opcode = getOpcode(inputBin); // get the opcode from the line
    instr_metadata metadata = getInstruction(opcode);   // Fill the metadata for the instruction
    metadata.bin_bitmap = inputBin;   // store binary representation of instruction
    printf("bitmap = %x\n", inputBin);
    stages[_IF] = metadata; // store instruction in IF stage
}

//Instruction Decode
void Pipeline::ID(instr_metadata &metadata) {
    // check for NOP (initialization)
    if (metadata.name == "NOP") {
        cout << "[ID]: NOP" << endl;
        return;
    }
    
    metadata = parseInstruction(metadata);
    stages[_ID] = metadata;
    printFields(metadata);
   #ifdef DEBUG
        // Display the fields of the instruction
        cout << "\t\tOpcode: " << metadata.bitmap.opcode << endl;
        cout << "\t\tRS: " << metadata.bitmap.rs << endl;
        cout << "\t\tRT: " << metadata.bitmap.rt << endl;
        if(metadata.addressMode == 0) {
            cout << "\t\tIMM: " << metadata.bitmap.imm << endl;
        } else {
            cout << "\t\tRD: " << metadata.bitmap.rd << endl;
        }
   #endif

}

//Execute
void Pipeline::EX(instr_metadata &metadata) {
    // Check for nop
    if (metadata.name == "NOP") {
        cout << "[EX]: NOP" << endl;
        return;
    }
    
    // Check for hazards
    // Hazards();

    // Execute the instruction
    metadata =  executeInstruction(metadata);

    // Update status
    // cout << "Type: " << inst.type << endl;
    typeExecd[metadata.type]++; // update number of times instruction type was executed
    PC += 4; // increment program counter

    #ifdef DEBUG
    printf("(%d) ", PC);
    #endif
    stages[_EX] = metadata;
    printFields(metadata);
    
}

//Memory Operations
void Pipeline::MEM(instr_metadata &metadata) {
    // Check for nop
    if (metadata.name == "NOP") {
        cout << "[MEM]: NOP" << endl;
        return;
    }

    printFields(metadata);

    if (metadata.bitmap.opcode == 12) { // LDW
        cout << "[MEM] MDR: " << MDR << endl;
        MDR = ALUresult;
    }

    if (metadata.addressMode == 0) { // Immediate addressing
        registers[metadata.bitmap.rt] = ALUresult;
    } else { // Register addressing
        cout << "[MEM] ALUresult: " << ALUresult << endl;
        registers[metadata.bitmap.rd] = ALUresult;
    }

    cout << "[MEM] Instruction: " << metadata.name << endl;
    printFields(metadata);
}

//Write Back
void Pipeline::WB(instr_metadata &metadata) {
    // Check for nop
    if (metadata.name == "NOP") {
        cout << "[WB]: NOP" << endl;
        return;
    }

    if (metadata.bitmap.opcode == 12) {
        memory[metadata.bitmap.rt] = memory[MDR];
    }

    cout << "[WB] Instruction: " << metadata.name << endl;
}

/*-------------------------------------------------------------------------------------------- 
SECTION 2 Helper functions
--------------------------------------------------------------------------------------------*/

// Print Statistics  
void Pipeline::printReport() {
    std::cout << std::endl << "Arithmetic instructions executed: " << typeExecd[0] << std::endl;
    std::cout << "Logical instructions executed: " << typeExecd[1] << std::endl;
    std::cout << "Memory access instructions executed: " << typeExecd[2] << std::endl;
    std::cout << "Control flow instructions executed: " << typeExecd[3] << std::endl;

    std::cout << "Program Counter: " << PC << std::endl;
    
    for (int i = 0; i < 32; i++) {
        std::cout << "R" << i << ": " << registers[i] << std::endl;
    }

    for (auto const& x : memory) {
        std::cout << "Memory[" << x.first << "]: " << x.second << std::endl;
    }
    std::cout << std::endl;

}


//print register fields
 void Pipeline::printFields(instr_metadata &metadata) {
     cout << "Instruction: " << metadata.name << endl;
     cout << "Opcode: " << metadata.bitmap.opcode << endl;
     cout << "RS: " << metadata.bitmap.rs << endl;
     cout << "RT: " << metadata.bitmap.rt << endl;
     cout << "RD: " << metadata.bitmap.rd << endl;
     cout << "IMM: " << metadata.bitmap.imm << endl;
 }


/*-------------------------------------------------------------------------------------------- 
SECTION 3 Pipeline control
--------------------------------------------------------------------------------------------*/

// Insert stall
void Pipeline::stall(int cycles) {

    for(int i = 0; i < cycles; i++) {
        //TODO: Insert NOP(s) into stages
        numStalls++;
    }
}

// Flush pipeline after misprediction
void Pipeline::flush(void) {
    // TODO: Implement
    std::cout << "Not implemented" << std::endl;

}

/*-------------------------------------------------------------------------------------------- 
SECTION 4 Control functions
--------------------------------------------------------------------------------------------*/

//initialize busy registers to 0
void Pipeline::initBusyRegs(void) {
    for (int i = 0; i < 32; i++) {
        busyRegs[i] = 0;
    }
}

//decrement busy registers by 1 (at end of cycle)
void Pipeline::decrBusyRegs(void) {
    for (int i = 0; i < 32; i++) {
        if (busyRegs[i] > 0) {
            busyRegs[i]--;
        }
    }
}

//Decode Instruction
instr_metadata Pipeline::parseInstruction(instr_metadata &metadata) {
    if (metadata.addressMode == 0) { // Immediate addressing
        metadata.bitmap.opcode = getOpcode(metadata.bin_bitmap);
        metadata.bitmap.rs = (metadata.bin_bitmap & 0x3E00000) >> 21;
        metadata.bitmap.rt = (metadata.bin_bitmap & 0x1F0000) >> 16;
        metadata.bitmap.rd = 0; // No destination register for immediate addressing
        metadata.bitmap.imm = metadata.bin_bitmap & 0xFFFF;

//Check if the immediate value is negative. If so, invert the bits and add 1 to get 2s complement
        if (metadata.bitmap.imm & 0x8000) {
            metadata.bitmap.imm = ~metadata.bitmap.imm + 1;
        }

       // #ifdef DEBUG
printf("%s R%d, R%d, #%d\n", metadata.name.c_str(), metadata.bitmap.rt, metadata.bitmap.rs, metadata.bitmap.imm);
       // #endif
    } else { // Register addressing
        metadata.bitmap.opcode = getOpcode(metadata.bin_bitmap);
        metadata.bitmap.rs = (metadata.bin_bitmap & 0x03E00000) >> 21;
        metadata.bitmap.rt = (metadata.bin_bitmap & 0x001F0000) >> 16;
        metadata.bitmap.rd = (metadata.bin_bitmap & 0x0000F800) >> 11;
        metadata.bitmap.imm = 0; // No immediate value for register addressing

       // #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", metadata.name.c_str(), metadata.bitmap.rd, metadata.bitmap.rs, metadata.bitmap.rt);
       // #endif
    }

    // Sanity check if any of the registers are out of bounds
    int sanityCheck = 0;
    if(metadata.bitmap.rs > 31) {
        cout << "Error: Register RS: " << metadata.bitmap.rs << " out of bounds" << endl;
        sanityCheck += 1;
    }
    if(metadata.bitmap.rt > 31) {
        cout << "Error: Register RT: " << metadata.bitmap.rt << " out of bounds" << endl;
        sanityCheck += 2;
    }
    if(metadata.bitmap.rd > 31) {
        cout << "Error: Register RD: " << metadata.bitmap.rd << " out of bounds" << endl;
        sanityCheck += 4;
    }
    if(metadata.bitmap.imm > 65535) {
        cout << "Error: Immediate value " << metadata.bitmap.imm << " out of bounds" << endl;
        sanityCheck += 8;
    }

    if(sanityCheck > 0){
        printFields(metadata);
        cout << endl;
        return metadata;
    }

    // Otherwise
    cout << "[ID]: Instruction: " << metadata.name << endl;
    return metadata;
}

//execute instruction
instr_metadata Pipeline::executeInstruction(instr_metadata &metadata) {
    // Check for hazards
    // Hazards();    
    switch(metadata.bitmap.opcode) {
        // REGISTER
        case 0: // ADD
            ALUresult = registers[metadata.bitmap.rs] + registers[metadata.bitmap.rt]; 
            break;
        case 2: // SUB
            ALUresult = registers[metadata.bitmap.rs] - registers[metadata.bitmap.rt];
            break;
        case 4: // MUL
            ALUresult = registers[metadata.bitmap.rs] * registers[metadata.bitmap.rt];
            break;
        case 6: // OR
            ALUresult = registers[metadata.bitmap.rs] | registers[metadata.bitmap.rt];
            break;
        case 8: // AND
            ALUresult = registers[metadata.bitmap.rs] & registers[metadata.bitmap.rt];
            break;
        case 10: // XOR
            ALUresult = registers[metadata.bitmap.rs] ^ registers[metadata.bitmap.rt];
            break;
        
        // IMMEDIATE
        case 1: // ADDI
            ALUresult = registers[metadata.bitmap.rs] + metadata.bitmap.imm;
            break;
        case 3: // SUBI
            ALUresult = registers[metadata.bitmap.rs] - metadata.bitmap.imm;
            break;
        case 5: // MULI
            ALUresult = registers[metadata.bitmap.rs] * metadata.bitmap.imm;
            break;
        case 7: // ORI
            ALUresult = registers[metadata.bitmap.rs] | metadata.bitmap.imm;
            break;
        case 9: // ANDI
            ALUresult = registers[metadata.bitmap.rs] & metadata.bitmap.imm;
            break;
        case 11: // XORI
            ALUresult = registers[metadata.bitmap.rs] ^ metadata.bitmap.imm;
            break;
        case 12: // LDW
            ALUresult = memory[metadata.bitmap.imm];
            break;
        case 13: // STW
            ALUresult = registers[metadata.bitmap.rt];
            break;
        
        // CONTROL FLOW
        case 14: // BZ
            if (registers[metadata.bitmap.rt] == 0) {
                PC += metadata.bitmap.imm * 4;
            }
            break;
        case 15: // BEQ
            if (registers[metadata.bitmap.rt] == registers[metadata.bitmap.rs]) {
                PC += metadata.bitmap.imm * 4;
            }
            break;
        case 16: // JR
            PC += registers[metadata.bitmap.rt] * 4;
            break;
        default:
            cout << "Error: Invalid opcode" << endl;
            metadata.name += " Error" ;
            printFields(metadata);
            break;
    }

    cout << "[EX]: Instruction: " << metadata.name << endl;
    printf("ALUresult: %d\n", ALUresult); 
    return metadata;
}

//Error handling for Hazards
void Pipeline::Hazards(void) {

 // The instruction in the decode stage has registers which need to be checked for dependencies
    int rsBusy = busyRegs[stages[_ID].bitmap.rs];
    int rtBusy = busyRegs[stages[_ID].bitmap.rt];
    int rdBusy = busyRegs[stages[_ID].bitmap.rd];
    
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

//used to check bit-wise operations on ints. int n to be printed in binary
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

//Adds all possible/legal instructions and their addressing mode/type
void Pipeline::defineInstSet() {
    /**
     * Type of instruction:
     * 0 = arithmetic
     * 1 = logical
     * 2 = memory access
     * 3 = control flow
     * 
     * Address mode:
     * 0 = immediate, 1 = register
     */
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
 * @brief Define an individual instruction's information
 * 
 * @param instrName the instruction as it appears in assembly code (i.e ADD, ADDI)
 * @param opcode opcode in decimal form (int)
 * @param type 0 = arithmetic, 1 = logical, 2 = memory access, 3 = control flow
 * @param addressMode 0 = immediate, 1 = register
 */
void Pipeline::setInstruction(std::string instrName, int opcode, int type, bool addressMode) {
    instr_metadata metadata;
    metadata.name = instrName;
    metadata.type = type;
    metadata.addressMode = addressMode;
    metadata.bitmap = Bitmap();
    instructionSet[opcode] = metadata;
}


//Calculates the opcode of a given instruction
int Pipeline::getOpcode(int trace) {
    int opcode = trace >> 26;
    return opcode;
}

/**
 * @brief Based on an instruction's op code, this function determines what instruction it is, as 
 * well as its addressing mode and type
 * 
 * @param opcode the instruction's op code
 * @return instruction -- struct with information about the instruction's name, addressing mode and type
 */
instr_metadata Pipeline::getInstruction(int opcode) {
    if (instructionSet.count(opcode) > 0) {
        return instructionSet.at(opcode);
	        
    } else {
        // Handle the case where the opcode does not exist in the map
        // For example, you can return a default instruction
        instr_metadata metadata_default;
        metadata_default.type = -1;
        metadata_default.addressMode = -1;
        return metadata_default;
    }
}

/**********************************
SECTION 5 are PUBLIC functions
***********************************/

/*-------------------------------------------------------------------------------------------- 
SECTION 5 User functions
--------------------------------------------------------------------------------------------*/


void Pipeline::moveStages(int line) {

    WB(stages[4]);// Writeback the instruction waiting in this stage (pulled from MEM last cycle 
    stages[4] = stages[3]; // Pull instruction from MEM to WB
    //cout << "WB: " << stages[4].name << endl;

    MEM(stages[3]); // Memory operation
    stages[3] = stages[2]; // Pull instruction from EX to MEM
    //cout << "MEM: " << stages[3].name << endl;

    EX(stages[2]); // Execute instruction
    stages[2] = stages[1]; // Pull instruction from ID to EX
    //cout << "EX: " << stages[2].name << endl;
    
    ID(stages[1]); // Decode instruction 
    stages[1] = stages[0]; // Pull instruction from IF to ID
    //cout << "ID: " << stages[1].name << endl;
    
    // call IF to fill IF stage
    IF(line);
    //cout << "IF: " << stages[0].name << endl << endl;
    cout << endl;
}

// checks to see if a halt instruction is found
int Pipeline::checkHalt(int bin) {
    
    if (getOpcode(bin) == 17) {
        return 0; // end program after HALT instruction is found
    }
    else {
        return 1; // otherwise, continue
    }
}

void Pipeline::run(instr_metadata &metadata) {

// TODO: implement functionality 

}

//print statistics from private function 
void Pipeline::printExecutionReport() {
    printReport();
}

//initialize pipeline stages to NOPs
void Pipeline::initNOPs(void) {
    for (int i = 0; i < 5; i++) {
        stages[i].name = "NOP";
        stages[i].bitmap.opcode = 18;
        stages[i].bitmap.rs = 0;
        stages[i].bitmap.rt = 0;
        stages[i].bitmap.rd = 0;
        stages[i].bitmap.imm = 0;
    }
}

