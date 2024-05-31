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

    // cout << "Pipeline object created" << endl;
    defineInstSet();
    numStalls = 0;      // Initialize number of stalls to 0
    stages.resize(5);   // 5 stages in pipeline

    // cout << "Initializing registers and NOPS" << endl;
    initBusyRegs();     // Initialize busy registers
    initNOPs();         // Initialize NOPs in stages

    // Initialize the members
    ALUresult = 0;
    MDR = 0;
    PC = 0;
    instrFetched = 0;
    

    // clear memory
    memory.clear();

    // Initialize registers
    registers.resize(32);

    // Set all registers to 0
    for (int i = 0; i < 32; i++) {
        registers[i] = 0;
    }

    // Initialize typeExecd
    typeExecd.resize(4);

    // Set all types to 0
    for (int i = 0; i < 4; i++) {
        typeExecd[i] = 0;
    }

    
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
    
    // make a new metadata object which will persist through the stages
    instr_metadata* metadataPtr = new instr_metadata;

    // Copy the metadata to the pointer
    metadataPtr->name = metadata.name;
    metadataPtr->type = metadata.type;
    metadataPtr->addressMode = metadata.addressMode;
    metadataPtr->bitmap = new Bitmap;

    metadataPtr->instructionNumber = instrFetched++; // store instruction number   
    metadataPtr->bin_bitmap = inputBin;   // store binary representation of instruction 
    
    printf("bitmap = %x\n", inputBin);
    stages[_IF] = *metadataPtr; // store instruction in IF stage
}

//Instruction Decode
void Pipeline::ID(instr_metadata &metadata) {
    // check for NOP (initialization)
    if (metadata.name == "NOP") {
        cout << "[ID]: NOP" << endl;
        return;
    }

    if(metadata.name == "ERROR") {
        cout << "[ID]: ERROR" << endl;
        return;
    }

    // Decode the instruction and fill the metadata
    metadata = parseInstruction(metadata);

    cout << "[ID] Instruction: " << metadata.name << endl;
    printFields(metadata);

    // Put the filled metadata back into the stage
    stages[_ID] = metadata;
    
   #ifdef DEBUG
        // Display the fields of the instruction
        cout << "\t\tOpcode: " << metadata.bitmap->opcode << endl;
        cout << "\t\tRS: " << metadata.bitmap->rs << endl;
        cout << "\t\tRT: " << metadata.bitmap->rt << endl;
        if(metadata.addressMode == 0) {
            cout << "\t\tIMM: " << metadata.bitmap->imm << endl;
        } else {
            cout << "\t\tRD: " << metadata.bitmap->rd << endl;
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
    
    if(metadata.name == "ERROR") {
        cout << "[ID]: ERROR" << endl;
        return;
    }
    
    cout << "[EX] Instruction: " << metadata.name << endl;
    printFields(metadata);

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

    
}

//Memory Operations
void Pipeline::MEM(instr_metadata &metadata) {
    // Check for nop
    if (metadata.name == "NOP") {
        cout << "[MEM]: NOP" << endl;
        return;
    }

    if(metadata.name == "ERROR") {
        cout << "[ID]: ERROR" << endl;
        return;
    }

    // If it is not a NOP, print the fields
    cout << "[MEM] Instruction: " << metadata.name << endl;
    printFields(metadata);

    //Jumps (return)
    if(metadata.bitmap->opcode > 13) {
        return;
    }

    // LDW
    if (metadata.bitmap->opcode == 12) { 
        MDR = memory[ALUresult];
        cout << "\t[MEM] MDR: " << MDR << endl;
        return;
    }

    // STW
    if(metadata.bitmap->opcode == 13) { 
        memory[ALUresult] = registers[metadata.bitmap->rt];
        cout << "\t[MEM] Memory[" << ALUresult << "]: " << memory[ALUresult] << endl;
        return;
    }

    if (metadata.addressMode == 0) { // Immediate addressing

        cout << "\t[MEM] ALUresult: " << ALUresult << endl;
        registers[metadata.bitmap->rt] = ALUresult;
        cout << "\t[MEM] RT: " << metadata.bitmap->rt << " = " << registers[metadata.bitmap->rt] << endl;
    } 
    else { // Register addressing
        cout << "\t[MEM] ALUresult: " << ALUresult << endl;
        registers[metadata.bitmap->rd] = ALUresult;
        cout << "\t[MEM] RD: " << metadata.bitmap->rd << " = " << registers[metadata.bitmap->rd] << endl;
    }
}

//Write Back
void Pipeline::WB(instr_metadata &metadata) {
    // Check for nop
    if (metadata.name == "NOP") {
        cout << "[WB]: NOP" << endl;
        return;
    }

    if(metadata.name == "ERROR") {
        cout << "[ID]: ERROR" << endl;
        return;
    }

    cout << "[WB] Instruction: " << metadata.name << endl;

    // Only LDW will reach this stage
    if (metadata.bitmap->opcode == 12) {
        cout << "\t[WB] MDR: " << MDR << endl;
        registers[metadata.bitmap->rt] = MDR;
        cout << "\t[WB] Memory[" << metadata.bitmap->rt << "]: " << memory[metadata.bitmap->rt] << endl;
    }



}

/*-------------------------------------------------------------------------------------------- 
SECTION 2 Helper functions
--------------------------------------------------------------------------------------------*/

// Print Statistics  
void Pipeline::printReport() {
    cout << std::endl << "Arithmetic instructions executed: " << typeExecd[0] << endl;
    cout << "Logical instructions executed: " << typeExecd[1] << endl;
    cout << "Memory access instructions executed: " << typeExecd[2] << endl;
    cout << "Control flow instructions executed: " << typeExecd[3] << endl;

    cout << "Program Counter: " << PC << endl;
    cout << "Instructions fetched: " << instrFetched << endl;
    
    for (int i = 0; i < 32; i++) {
        if(registers[i] != 0) {
            cout << "R" << i << ": " << registers[i] << endl;
        }
    }

    for (auto const& x : memory) {
        cout << "Memory[" << x.first << "]: " << x.second << endl;
    }
    cout << endl;

}


//print register fields
 void Pipeline::printFields(instr_metadata &metadata) {

    // Print the metadata fields
    cout << "\tInstruction " << metadata.instructionNumber << ": " << metadata.name << endl;
    cout << "\tOpcode: " << metadata.bitmap->opcode << endl;

    // Don't print the fields if it is a NOP or an error
    if(metadata.name == "NOP" || metadata.name == "ERROR") {
        return;
    }

    // Print the bitmap fields and what the registers are
    cout << "\tRS: R" << metadata.bitmap->rs << " = " << registers[metadata.bitmap->rs] << " (R" << metadata.bitmap->rs << ")" << endl;
    cout << "\tRT: R" << metadata.bitmap->rt << " = " << registers[metadata.bitmap->rt] << " (R" << metadata.bitmap->rt << ")" << endl;
    cout << "\tRD: R" << metadata.bitmap->rd << " = " << registers[metadata.bitmap->rd] << " (R" << metadata.bitmap->rd << ")" << endl;
    cout << "\tIMM: " << metadata.bitmap->imm << endl;
 }


/*-------------------------------------------------------------------------------------------- 
SECTION 3 Pipeline control
--------------------------------------------------------------------------------------------*/

// Insert stall
void Pipeline::stall(int cycles) {

    instr_metadata* metadata = new instr_metadata;
    metadata->name = "NOP";


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

/**
 * @brief Internal Pipeline Move
 * 
 * 
 * 
 */
void Pipeline::internalPipeMove(int sourceIndex, int destIndex) {

    // If the op code is NOP, only copy the name and opcode
    if (stages[sourceIndex].name == "NOP") {
        stages[destIndex].name = stages[sourceIndex].name;
        stages[destIndex].bitmap->opcode = stages[sourceIndex].bitmap->opcode;
        return;
    }

    // Copy the bitmap data
    stages[destIndex].bitmap->rd = stages[sourceIndex].bitmap->rd;
    stages[destIndex].bitmap->rs = stages[sourceIndex].bitmap->rs;
    stages[destIndex].bitmap->rt = stages[sourceIndex].bitmap->rt;
    stages[destIndex].bitmap->opcode = stages[sourceIndex].bitmap->opcode;
    stages[destIndex].bitmap->imm = stages[sourceIndex].bitmap->imm;

    // Copy the metadata
    stages[destIndex].name = stages[sourceIndex].name;
    stages[destIndex].addressMode = stages[sourceIndex].addressMode;
    stages[destIndex].type = stages[sourceIndex].type;
    stages[destIndex].bin_bitmap = stages[sourceIndex].bin_bitmap;

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
        metadata.bitmap->opcode = getOpcode(metadata.bin_bitmap);
        metadata.bitmap->rs = (metadata.bin_bitmap & 0x3E00000) >> 21;
        metadata.bitmap->rt = (metadata.bin_bitmap & 0x1F0000) >> 16;
        metadata.bitmap->rd = 0; // No destination register for immediate addressing
        metadata.bitmap->imm = metadata.bin_bitmap & 0xFFFF;

//Check if the immediate value is negative. If so, invert the bits and add 1 to get 2s complement
        if (metadata.bitmap->imm & 0x8000) {
            metadata.bitmap->imm = ~metadata.bitmap->imm + 1;
        }

       // #ifdef DEBUG
printf("%s R%d, R%d, #%d\n", metadata.name.c_str(), metadata.bitmap->rt, metadata.bitmap->rs, metadata.bitmap->imm);
       // #endif
    } else { // Register addressing
        metadata.bitmap->opcode = getOpcode(metadata.bin_bitmap);
        metadata.bitmap->rs = (metadata.bin_bitmap & 0x03E00000) >> 21;
        metadata.bitmap->rt = (metadata.bin_bitmap & 0x001F0000) >> 16;
        metadata.bitmap->rd = (metadata.bin_bitmap & 0x0000F800) >> 11;
        metadata.bitmap->imm = 0; // No immediate value for register addressing

       // #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", metadata.name.c_str(), metadata.bitmap->rd, metadata.bitmap->rs, metadata.bitmap->rt);
       // #endif
    }

    // Sanity check if any of the registers are out of bounds
    int sanityCheck = 0;
    if(metadata.bitmap->rs > 31) {
        cout << "Error: Register RS: " << metadata.bitmap->rs << " out of bounds" << endl;
        sanityCheck += 1;
    }
    if(metadata.bitmap->rt > 31) {
        cout << "Error: Register RT: " << metadata.bitmap->rt << " out of bounds" << endl;
        sanityCheck += 2;
    }
    if(metadata.bitmap->rd > 31) {
        cout << "Error: Register RD: " << metadata.bitmap->rd << " out of bounds" << endl;
        sanityCheck += 4;
    }
    if(metadata.bitmap->imm > 65535) {
        cout << "Error: Immediate value " << metadata.bitmap->imm << " out of bounds" << endl;
        sanityCheck += 8;
    }

    if(sanityCheck > 0){
        printFields(metadata);
        cout << endl;
        return metadata;
    }

    // Otherwise
    return metadata;
}

//execute instruction
instr_metadata Pipeline::executeInstruction(instr_metadata &metadata) {
    // Check for hazards
    // Hazards();    

    switch(metadata.bitmap->opcode) {
        // REGISTER
        case 0: // ADD
            ALUresult = registers[metadata.bitmap->rs] + registers[metadata.bitmap->rt]; 
            cout << "\t[EX]: R" << metadata.bitmap->rd << " = R" << metadata.bitmap->rs << " + R" << metadata.bitmap->rt << endl;
            break;
        case 2: // SUB
            ALUresult = registers[metadata.bitmap->rs] - registers[metadata.bitmap->rt];
            cout << "\t[EX]: R" << metadata.bitmap->rd << " = R" << metadata.bitmap->rs << " - R" << metadata.bitmap->rt << endl;
            break;
        case 4: // MUL
            ALUresult = registers[metadata.bitmap->rs] * registers[metadata.bitmap->rt];
            cout << "\t[EX]: R" << metadata.bitmap->rd << " = R" << metadata.bitmap->rs << " * R" << metadata.bitmap->rt << endl;
            break;
        case 6: // OR
            ALUresult = registers[metadata.bitmap->rs] | registers[metadata.bitmap->rt];
            cout << "\t[EX]: R" << metadata.bitmap->rd << " = R" << metadata.bitmap->rs << " | R" << metadata.bitmap->rt << endl;
            break;
        case 8: // AND
            ALUresult = registers[metadata.bitmap->rs] & registers[metadata.bitmap->rt];
            cout << "\t[EX]: R" << metadata.bitmap->rd << " = R" << metadata.bitmap->rs << " & R" << metadata.bitmap->rt << endl;
            break;
        case 10: // XOR
            ALUresult = registers[metadata.bitmap->rs] ^ registers[metadata.bitmap->rt];
            cout << "\t[EX]: R" << metadata.bitmap->rd << " = R" << metadata.bitmap->rs << " ^ R" << metadata.bitmap->rt << endl;
            break;
        
        // IMMEDIATE
        case 1: // ADDI
            ALUresult = registers[metadata.bitmap->rs] + metadata.bitmap->imm;
            cout << "\t[EX]: R" << metadata.bitmap->rt << " = R" << metadata.bitmap->rs << " + " << metadata.bitmap->imm << endl;
            break;
        case 3: // SUBI
            ALUresult = registers[metadata.bitmap->rs] - metadata.bitmap->imm;
            cout << "\t[EX]: R" << metadata.bitmap->rt << " = R" << metadata.bitmap->rs << " - " << metadata.bitmap->imm << endl;
            break;
        case 5: // MULI
            ALUresult = registers[metadata.bitmap->rs] * metadata.bitmap->imm;
            cout << "\t[EX]: R" << metadata.bitmap->rt << " = R" << metadata.bitmap->rs << " * " << metadata.bitmap->imm << endl;
            break;
        case 7: // ORI
            ALUresult = registers[metadata.bitmap->rs] | metadata.bitmap->imm;
            cout << "\t[EX]: R" << metadata.bitmap->rt << " = R" << metadata.bitmap->rs << " | " << metadata.bitmap->imm << endl;
            break;
        case 9: // ANDI
            ALUresult = registers[metadata.bitmap->rs] & metadata.bitmap->imm;
            cout << "\t[EX]: R" << metadata.bitmap->rt << " = R" << metadata.bitmap->rs << " & " << metadata.bitmap->imm << endl;
            break;
        case 11: // XORI
            ALUresult = registers[metadata.bitmap->rs] ^ metadata.bitmap->imm;
            cout << "\t[EX]: R" << metadata.bitmap->rt << " = R" << metadata.bitmap->rs << " ^ " << metadata.bitmap->imm << endl;
            break;
        
        case 12: // LDW //FIXME: Rs + imm
            ALUresult = registers[metadata.bitmap->rs] + metadata.bitmap->imm;
            cout << "\t[EX]: R" << metadata.bitmap->rs << " = Memory[" << metadata.bitmap->imm << "]" << endl;
            break;
        case 13: // STW //FIXME: Rs + imm
            ALUresult = registers[metadata.bitmap->rs] + metadata.bitmap->imm;
            cout << "\t[EX]: Memory[" << metadata.bitmap->imm << "] = R" << metadata.bitmap->rt << endl;
            break;
        
        // CONTROL FLOW
        case 14: // BZ
            if (registers[metadata.bitmap->rt] == 0) {
                PC += metadata.bitmap->imm * 4;
                cout << "\t[EX]: Branching to " << PC << endl;
            }
            break;
        case 15: // BEQ
            if (registers[metadata.bitmap->rt] == registers[metadata.bitmap->rs]) {
                PC += metadata.bitmap->imm * 4;
                cout << "\t[EX]: Branching to " << PC << endl;
            }
            break;
        case 16: // JR
            PC += registers[metadata.bitmap->rt] * 4;
            cout << "\t[EX]: Jumping to " << PC << endl;
            break;
        default:
            cout << "Error: Invalid opcode" << endl;
            metadata.name += " Error" ;
            printFields(metadata);
            break;
    }

    printf("\t[EX]: ALUresult: %d\n", ALUresult); 
    return metadata;
}

//Error handling for Hazards
void Pipeline::Hazards(void) {

 // The instruction in the decode stage has registers which need to be checked for dependencies
    int rsBusy = busyRegs[stages[_ID].bitmap->rs];
    int rtBusy = busyRegs[stages[_ID].bitmap->rt];
    int rdBusy = busyRegs[stages[_ID].bitmap->rd];
    
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
    instr_metadata* metadata = new instr_metadata;
    metadata->name = instrName;
    metadata->type = type;
    metadata->addressMode = addressMode;
    // cout << "[DEBUG - setInstruction]: Opcode: " << opcode << endl;
    metadata->bitmap = new Bitmap;
    instructionSet[opcode] = *metadata;
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
        metadata_default.name = "ERROR";
        metadata_default.type = -1;
        metadata_default.addressMode = -1;
        cout << "[ERROR - getInstruction]: Opcode not found in instruction set" << endl;
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

    stages[_WB] = stages[_MEM]; // Pull instruction from MEM to WB
    WB(stages[_WB]);// Writeback the instruction pulled in this stage
    
    stages[_MEM] = stages[_EX]; // Pull instruction from EX to MEM
    MEM(stages[_MEM]); // Memory operation    

    stages[_EX] = stages[_ID]; // Pull instruction from ID to EX
    EX(stages[_EX]); // Execute instruction
    
    stages[_ID] = stages[_IF]; // Pull instruction from IF to ID
    ID(stages[_ID]); // Decode instruction
    
    // call IF to fill IF stage


    if(!stallCondition) {
        IF(line);
        cout << endl;
        return;    
    }
    moveStages(line);
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

void Pipeline::run() {

    int instruction = stoi(instructionMemory[PC]); // should be initialized to 0
    int mainIndex = 0;
    int lastPC = 0;
    // Move the instruction at the PC counter from the instruction memory to the IF stage
    do{
        std::cout << "[Main]: " << mainIndex++ << std::endl;
        cout << "PC: " << PC << endl;
        
        // break if HALT instruction is found
        if(!checkHalt(instruction)){
            break;
        }

        if(mainIndex > 500) {
            cout << "Error: PC exceeded instruction memory range" << endl;
            break;
        }

        moveStages(instruction);
        instruction = stoi(instructionMemory[PC], 0, 16); // convert to int for parsing

        // Check if PC is incrementing
        if((lastPC == PC) && mainIndex > 10) {
            cout << "Error: PC is not incrementing" << endl;
            break;
        }
        
        lastPC = PC;

    } while ((PC * 0.25) < (int) instructionMemory.size());   // Continue until HALT is found or PC exceeds the instruction memory range
    std::cout << "[Main]: HALT instruction found\n";
    
    // stall 5 cycles to flush the pipeline
    printExecutionReport();
}

//print statistics from private function 
void Pipeline::printExecutionReport() {
    printReport();
}

//initialize pipeline stages to NOPs
void Pipeline::initNOPs(void) {
    for (int i = 0; i < 5; i++) {
        // cout << "Initializing NOP in stage " << i << endl;
        stages[i].name = "NOP";

        // Allocate memory for the bitmap of instruction
        stages[i].bitmap = new Bitmap;
        stages[i].bitmap->opcode = 18;
        stages[i].bitmap->rs = 0;
        stages[i].bitmap->rt = 0;
        stages[i].bitmap->rd = 0;
        stages[i].bitmap->imm = 0;
    }

}

