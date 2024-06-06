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
#include <sstream>
#include <stdint.h>

//constructor (public) 
//this initializes the Pipeline class
Pipeline::Pipeline(int mode) {


    this->setForwardingFlag(mode);
    
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
    clk = 0; 
    stallCondition = false;
    criticalProblem = false;
     
    

    // clear memory
    changedMemory.clear();

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
    metadataPtr->len = metadata.len;
    if(!stallCondition) 
    	metadataPtr->instructionNumber = instrFetched++; // store instruction number   
    //FIXME: Remove once everything works
   /* if(instrFetched > 650) {
    /   cout << "We have surpassed the expected number of instructions" << endl;
        //criticalProblem = true;
    }*/


    metadataPtr->bin_bitmap = inputBin;   // store binary representation of instruction 
    
    printf("[IF]: Fetching bitmap = %0X\n", inputBin);
    stages[_IF] = *metadataPtr; // store instruction in IF stage
     PC += 4; // increment program counter


}

void Pipeline::IF(string inputBin) {
    
    if(inputBin.length() != 32) {
        // Convert hex string to binary string
        string binaryString = "";
        for (int i = 0; i < (int) inputBin.size(); i++) {
            switch (toupper(inputBin[i])) {
                case '0': binaryString += "0000"; break;
                case '1': binaryString += "0001"; break;
                case '2': binaryString += "0010"; break;
                case '3': binaryString += "0011"; break;
                case '4': binaryString += "0100"; break;
                case '5': binaryString += "0101"; break;
                case '6': binaryString += "0110"; break;
                case '7': binaryString += "0111"; break;
                case '8': binaryString += "1000"; break;
                case '9': binaryString += "1001"; break;
                case 'A': binaryString += "1010"; break;
                case 'B': binaryString += "1011"; break;
                case 'C': binaryString += "1100"; break;
                case 'D': binaryString += "1101"; break;
                case 'E': binaryString += "1110"; break;
                case 'F': binaryString += "1111"; break;
            }
        }
        inputBin = binaryString;
    }
    
    
    
    int opcode = getOpcode(inputBin); // get the opcode from the line
    instr_metadata metadata = getInstruction(opcode);   // Fill the metadata for the instruction
    
    // make a new metadata object which will persist through the stages
    instr_metadata* metadataPtr = new instr_metadata;

    // Copy the metadata to the pointer
    metadataPtr->name = metadata.name;
    metadataPtr->type = metadata.type;
    metadataPtr->addressMode = metadata.addressMode;
    metadataPtr->bitmap = new Bitmap;
    metadataPtr->len = metadata.len;
    if(!stallCondition) 
    	metadataPtr->instructionNumber = instrFetched++; // store instruction number
    
    metadataPtr->originalLine = inputBin;   // store binary representation of instruction 
    
    printf("[IF]: Fetching bitmap = %s\n", inputBin.substr(0, 6).c_str());
    stages[_IF] = *metadataPtr; // store instruction in IF stage
    PC += 4; // increment program counter

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
    cout << "[ID] ";
    printInstruction(metadata);
    // printFields(metadata);

    // Check for hazards
    Hazards();

    // Update busy registers
    setBusyRegs(metadata);

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
    
    cout << "[EX] ";
    printInstruction(metadata);
    // printFields(metadata);

    metadata =  executeInstruction(metadata);
    
    // Update status
    // cout << "Type: " << inst.type << endl;
    typeExecd[metadata.type]++; // update number of times instruction type was executed
    
    #ifdef DEBUG
    printf("(%d) ", PC);
    #endif
    stages[_EX] = metadata;

    
}

//Memory Operations
void Pipeline::MEM(instr_metadata &metadata) {
     
    int tempRegister;
    stringstream registerString;
    string hexString;
	
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
    cout << "[MEM] ";
    printInstruction(metadata);
    // printFields(metadata);

    //Jumps (return)
    if(metadata.bitmap->opcode > 13) {
        cout << " COMPLETE NOP" << endl;
        return;
    }
    
    // LDW
    if (metadata.bitmap->opcode == 12) { 
        if(ALUresult % 4 != 0) {
            cout << "\t\tError: Memory address not aligned" << endl;
            criticalProblem = true;
            metadata.name += " Error" ;
            printFields(metadata);
        }

        string tempString = instructionMemory[ALUresult];
        if(tempString.compare("") == 0) {
            cout << "\t\tError: Memory address not initialized" << endl;
            metadata.name += " Error" ;
            printFields(metadata);
            tempString = "00000000";
        }

        cout << "\t[MEM]: tempStr = " << tempString << endl;

        MDR = stoul(tempString, 0, 16);
        cout << "\t[MEM]: MDR = " << MDR;
        cout << "\t(ADDRESS = " << ALUresult << ")" << endl;
        return;
    }
    
    // STW
    if(metadata.bitmap->opcode == 13) {
	
        if(ALUresult % 4 != 0) {
            cout << "\t\tError: Memory address not aligned" << endl;
            criticalProblem = true;
            metadata.name += " Error" ;
            printFields(metadata);
        }	

	    //Convert int to string
    	tempRegister = registers[metadata.bitmap->rt];


        // Store the string in memory
        instructionMemory[ALUresult] = tempRegister;
        changedMemory[ALUresult] = registers[metadata.bitmap->rt];
        return;
    }

    if (metadata.addressMode == 0) { // Immediate addressing
        // Update the register
        registers[metadata.bitmap->rt] = ALUresult;
        cout << "\t[MEM]: R" << metadata.bitmap->rt << " = " << ALUresult;
        cout << "\t(R" << metadata.bitmap->rt << "=" << registers[metadata.bitmap->rt] << ")" << endl;
    }
    else { // Register addressing
        // Update the register
        registers[metadata.bitmap->rd] = ALUresult;
        cout << "\t[MEM]: R" << metadata.bitmap->rd << " = " << ALUresult;
        cout << "\t(R" << metadata.bitmap->rd << "=" << registers[metadata.bitmap->rd] << ")" << endl;
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

    // Only LDW will reach this stage
    if (metadata.bitmap->opcode == 12) {
    	cout << "[WB]: ";
    	printInstruction(metadata);
        registers[metadata.bitmap->rt] = MDR;
        cout << "\t[WB]: R" << metadata.bitmap->rt << " = " << MDR;
        cout << "\t(R" << metadata.bitmap->rt << "=" << registers[metadata.bitmap->rt] << ")" << endl;
    }
    else
   	cout << "[WB]: NOP" << endl;

 

}

/*-------------------------------------------------------------------------------------------- 
SECTION 2 Helper functions
--------------------------------------------------------------------------------------------*/

// Print Statistics  
void Pipeline::printReport() {
    cout << endl;
    
    int totalInstr = typeExecd[0] + typeExecd[1] + typeExecd[2] + typeExecd[3];
    cout << "Total number of instructions: " << totalInstr << endl;
    cout << "Arithmetic instructions executed: " << typeExecd[0] << endl;
    cout << "Logical instructions executed: " << typeExecd[1] << endl;
    cout << "Memory access instructions executed: " << typeExecd[2] << endl;
    cout << "Control flow instructions executed: " << typeExecd[3] << endl;


    cout << "Program Counter: " << PC << endl;
    for (int i = 0; i < 32; i++) {
        if(registers[i] != 0) {
            cout << "R" << i << ": " << registers[i] << endl;
        }
    }
    cout << "Number of stalls: " << numStalls << endl;
    
    for (auto const& x : changedMemory) {
        cout << "Changed Memory[" << x.first << "]: " << x.second << endl;
    }

    cout << endl;

    
    cout << "Clock Cycles: " << clk << endl;

    cout << endl; 


    // return;

    for (auto const& x : instructionMemory) {
        // After the halt instruction, don't print any memory adresses with values of 00000000
        if(x.second.compare("00000000") == 0) {
            continue;
        }

        cout << "Memory[" << x.first << "]: " << x.second << endl;

        // Check if the memory address is aligned
        if(x.first % 4 != 0) {
            cout << "Error: Memory address not aligned" << endl;
        }

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

void Pipeline::printBusyRegs(void) {

for(int i = 0; i<32; i++) {
	if(busyRegs[i] > 0){
	cout << "\tRegister[" << i << "] Busy for: " << busyRegs[i] << endl;
	}
   }

}


void Pipeline::printInstruction(instr_metadata &metadata) {

    // print the name of the instruction
    cout << "Instruction " << metadata.instructionNumber << ": " << metadata.name;

    // if it is a NOP, print nothing else
    if(metadata.name == "NOP") {
        cout << endl;
        return;
    }

    // based on the opcode, print the rest of the instruction
    switch(metadata.bitmap->opcode) {
        case 0: // ADD
            cout << " R" << metadata.bitmap->rd << ", R" << metadata.bitmap->rs << ", R" << metadata.bitmap->rt << endl;
            break;
        case 1: // ADDI
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 2: // SUB
            cout << " R" << metadata.bitmap->rd << ", R" << metadata.bitmap->rs << ", R" << metadata.bitmap->rt << endl;
            break;
        case 3: // SUBI
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 4: // MUL
            cout << " R" << metadata.bitmap->rd << ", R" << metadata.bitmap->rs << ", R" << metadata.bitmap->rt << endl;
            break;
        case 5: // MULI
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 6: // OR
            cout << " R" << metadata.bitmap->rd << ", R" << metadata.bitmap->rs << ", R" << metadata.bitmap->rt << endl;
            break;
        case 7: // ORI
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 8: // AND
            cout << " R" << metadata.bitmap->rd << ", R" << metadata.bitmap->rs << ", R" << metadata.bitmap->rt << endl;
            break;
        case 9: // ANDI
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 10: // XOR
            cout << " R" << metadata.bitmap->rd << ", R" << metadata.bitmap->rs << ", R" << metadata.bitmap->rt << endl;
            break;
        case 11: // XORI
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 12: // LDW
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 13: // STW
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 14: // BZ
            cout << " R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 15: // BEQ
            cout << " R" << metadata.bitmap->rt << ", R" << metadata.bitmap->rs << ", #" << metadata.bitmap->imm << endl;
            break;
        case 16: // JR
            cout << " R" << metadata.bitmap->rs << endl;
            break;
        case 17: // HALT
            cout << endl;
            break;
        default:
            cout << "Error: Invalid opcode" << endl;
            break;
    }
}

/*-------------------------------------------------------------------------------------------- 
SECTION 3 Pipeline control
--------------------------------------------------------------------------------------------*/

// Insert stall
void Pipeline::stall(void) {

    stages[_ID].name = "NOP";
    numStalls++;
    stallCondition = true;
}

// Flush pipeline after misprediction
void Pipeline::flush(void) {
cout << "\tFLUSH PIPE" << endl;
flushFlag = true; 
// clear the IF stage somehow
clk++;
stages[_ID].name = "NOP";
stages[_IF].name = "NOP";
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

void Pipeline::setBusyRegs(instr_metadata &metadata) {

    if(stallCondition) {
        return;
    }

    if(metadata.len > 5) {
        cout << "Error: Instruction length out of bounds" << endl;
        return;        
    }

    // Set the destination register as busy
    if (metadata.addressMode == 0) { // Immediate addressing
    	if(metadata.bitmap->rt)
        	busyRegs[metadata.bitmap->rt] = metadata.len;
    } else { // Register addressing
    	if(metadata.bitmap->rd)
        	busyRegs[metadata.bitmap->rd] = metadata.len;
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
    
    // Extract the original binary instruction
    string binString = metadata.originalLine;
    string opcodeString = binString.substr(0, 6);
    string rsString = binString.substr(6, 5);
    string rtString = binString.substr(11, 5);
    string rdString = binString.substr(16, 5);
    string immString = binString.substr(16, 16);

    // Set the common fields
    metadata.bitmap->opcode = getOpcode(binString);
    metadata.bitmap->rs = stoi(rsString, 0, 2);
    metadata.bitmap->rt = stoi(rtString, 0, 2);

    if (metadata.addressMode == 0) { // Immediate addressing
        metadata.bitmap->rd = 0; // No destination register for immediate addressing
        metadata.bitmap->imm = stoi(immString, 0, 2);

        // cout << "Before 2's complement:" << endl;
        // cout << "Immediate value: " << metadata.bitmap->imm << endl;
        // cout << "Immediate string: " << immString << endl;

        //Check if the immediate value is negative. If so, invert the bits and add 1 to get 2s complement
        if(immString[0] == '1') {
            metadata.bitmap->imm = ~metadata.bitmap->imm + 1;
            metadata.bitmap->imm &= 0xFFFF;
            metadata.bitmap->imm *= -1;
        }

        // cout << "After 2's complement:" << endl;
        // cout << "Immediate value: " << metadata.bitmap->imm << endl;

  #ifdef DEBUG
printf("%s R%d, R%d, #%d\n", metadata.name.c_str(), metadata.bitmap->rt, metadata.bitmap->rs, metadata.bitmap->imm);
 #endif
    } else { // Register addressing
        metadata.bitmap->rd = stoi(rdString, 0, 2);
        metadata.bitmap->imm = 0; // No immediate value for register addressing

   #ifdef DEBUG
        printf("%s R%d, R%d, R%d\n", metadata.name.c_str(), metadata.bitmap->rd, metadata.bitmap->rs, metadata.bitmap->rt);
    #endif
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
            cout << "\t[EX]: ALUresult = R" << metadata.bitmap->rs << " + " << metadata.bitmap->imm;
            cout << "\t(ADDRESS = " << ALUresult << ")" << endl;
            if(ALUresult % 4 != 0) {
                cout << "\t\tError: Memory address not aligned" << endl;
                criticalProblem = true;
                metadata.name += " Error" ;
                printFields(metadata);
            }
            return metadata;

        case 13: // STW //FIXME: Rs + imm
            ALUresult = registers[metadata.bitmap->rs] + metadata.bitmap->imm;
            cout << "\t[EX]: ALUresult = R" << metadata.bitmap->rs << " + " << metadata.bitmap->imm << " = " << ALUresult << endl;
            if(ALUresult % 4 != 0) {
                cout << "\t\tError: Memory address not aligned" << endl;
                criticalProblem = true;
                metadata.name += " Error" ;
                printFields(metadata);
            }
            return metadata;
        
        // CONTROL FLOW
        case 14: // BZ
            if (registers[metadata.bitmap->rs] == 0) {
                cout << "\t[EX]: Branching from " << PC << " to ";
                cout << PC + metadata.bitmap->imm * 4 << endl;
                PC += metadata.bitmap->imm * 4;
                PC -= 8;    // Decrement the PC by 4 to account for the increment at the end of the cycle		
                metadata.name = "NOP";
		flush();
            }
	    else {
	    cout << "\tBRANCH NOT TAKEN" << endl;
	    metadata.name = "NOP";
	    }
            break;
        case 15: // BEQ
            if (registers[metadata.bitmap->rt] == registers[metadata.bitmap->rs]) {
                cout << "\t[EX]: Branching from " << PC << " to ";
                cout << PC + metadata.bitmap->imm * 4 << endl;
                
                PC += metadata.bitmap->imm * 4;
                PC -= 8;
                metadata.name = "NOP";
		flush();
            }
	    else {
	    cout << "\tBRANCH NOT TAKEN" << endl;
	    metadata.name = "NOP";
	    }
            break;
        case 16: // JR
            cout << "\t[EX]: Jumping from " << PC << " to ";
            cout << registers[metadata.bitmap->rs] << endl;

            printFields(metadata);

            PC = registers[metadata.bitmap->rs];
            metadata.name = "NOP";
	    flush();
            break;
        default:
            cout << "Error: Invalid opcode" << endl;
            metadata.name += " Error" ;
            printFields(metadata);
            break;
    }

    if(metadata.bitmap->opcode < 14) 
    	printf("\t[EX]: ALUresult: %d\n", ALUresult); 
    return metadata;
}

//Error handling for Hazards
void Pipeline::Hazards(void) {
    // The instruction in the decode stage has registers which need to be checked for dependencies
    int rsBusy = 0;
    int rtBusy = 0;
    int rdBusy = 0;

    // verify that R0 is always 0
    if(registers[0] != 0) {
        cout << "\t\t\tError: R0 is not 0" << endl;
    }

    if(!stages[_ID].addressMode){
        rsBusy = busyRegs[stages[_ID].bitmap->rs];
        rtBusy = busyRegs[stages[_ID].bitmap->rt];
    }
    else{
        rsBusy = busyRegs[stages[_ID].bitmap->rs];
        rtBusy = busyRegs[stages[_ID].bitmap->rt];
        rdBusy = busyRegs[stages[_ID].bitmap->rd];
    }

    printBusyRegs();
    // Check if any of the registers are in use
    if (rsBusy > 0 || rtBusy > 0 || rdBusy > 0) {
        if(!stages[_ID].addressMode){
            cout << "\tSTALL FOR: " << max(rsBusy, rtBusy) << " CYCLE(S)" << endl;
            stallCondition = true;
        }
        else {
            cout << "\tSTALL FOR: " << max({rsBusy, rtBusy, rdBusy}) << " CYCLE(S)" << endl;
            stallCondition = true;
        }
        // Stall the pipeline until the busiest register is free
        stall();
    }
    else {
        	stallCondition = false;
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

    if(forwardingFlag) {

    setInstruction("ADD", 0, 0, 1, 0); // ADD
    setInstruction("SUB", 2, 0, 1, 0); // SUB
    setInstruction("MUL", 4, 0, 1, 0); // MUL
    setInstruction("OR", 6, 1, 1, 0); // OR
    setInstruction("AND", 8, 1, 1, 0); // AND
    setInstruction("XOR", 10, 1, 1, 0); // XOR
    setInstruction("LDW", 12, 2, 0, 2); // LDW
    setInstruction("STW", 13, 2, 0, 1); // STW
    setInstruction("ADDI", 1, 0, 0, 0); // ADDI
    setInstruction("SUBI", 3, 0, 0, 0); // SUBI
    setInstruction("MULI", 5, 0, 0, 0); // MULI
    setInstruction("ORI", 7, 1, 0, 0); // ORI
    setInstruction("ANDI", 9, 1, 0, 0); // ANDI
    setInstruction("XORI", 11, 1, 0, 0); // XORI
    setInstruction("BEQ", 15, 3, 0, 0); // BEQ
    setInstruction("BZ", 14, 3, 0, 0); // BZ
    setInstruction("JR", 16, 3, 0, 0); // JR
    setInstruction("HALT", 17, 3, 0, 0); // HALT
    }

    else if (!forwardingFlag) { 
    setInstruction("ADD", 0, 0, 1, 3); // ADD
    setInstruction("SUB", 2, 0, 1, 3); // SUB
    setInstruction("MUL", 4, 0, 1, 3); // MUL
    setInstruction("OR", 6, 1, 1, 3); // OR
    setInstruction("AND", 8, 1, 1, 3); // AND
    setInstruction("XOR", 10, 1, 1, 3); // XOR
    setInstruction("LDW", 12, 2, 0, 3); // LDW
    setInstruction("STW", 13, 2, 0, 3); // STW
    setInstruction("ADDI", 1, 0, 0, 3); // ADDI
    setInstruction("SUBI", 3, 0, 0, 3); // SUBI
    setInstruction("MULI", 5, 0, 0, 3); // MULI
    setInstruction("ORI", 7, 1, 0, 3); // ORI
    setInstruction("ANDI", 9, 1, 0, 3); // ANDI
    setInstruction("XORI", 11, 1, 0, 3); // XORI
    setInstruction("BEQ", 15, 3, 0, 3); // BEQ
    setInstruction("BZ", 14, 3, 0, 3); // BZ
    setInstruction("JR", 16, 3, 0, 3); // JR
    setInstruction("HALT", 17, 3, 0, 3); // HALT
    }
    
    else
    	printf("ERROR selecting mode");
}


/**
 * @brief Define an individual instruction's information
 * 
 * @param instrName the instruction as it appears in assembly code (i.e ADD, ADDI)
 * @param opcode opcode in decimal form (int)
 * @param type 0 = arithmetic, 1 = logical, 2 = memory access, 3 = control flow
 * @param addressMode 0 = immediate, 1 = register
 */
void Pipeline::setInstruction(std::string instrName, int opcode, int type, bool addressMode, int len) {
    instr_metadata* metadata = new instr_metadata;
    metadata->name = instrName;
    metadata->type = type;
    metadata->addressMode = addressMode;
    metadata->len = len;
    // cout << "[DEBUG - setInstruction]: Opcode: " << opcode << endl;
    metadata->bitmap = new Bitmap;
    instructionSet[opcode] = *metadata;
}


//Calculates the opcode of a given instruction
int Pipeline::getOpcode(int trace) {
    int opcode = trace >> 26;
    return opcode;
}

int Pipeline::getOpcode(string trace) {

    // determine if string is 8 (hex) or 32 (binary) bits
    if(trace.length() == 8) {
        
        // opcode is in the upper 6 bits of the binary representation
        int opcode = stoul(trace, 0, 16) >> 26;
        string opcodeStr = trace.substr(0, 2);
        int opcodeInt = stoi(opcodeStr, 0, 16);

        // cout << "[DEBUG - getOpcode]: Opcode (stoul): " << opcode << endl;
        // cout << "[DEBUG - getOpcode]: Opcode (string conversion): " << opcodeInt << endl;
        return opcodeInt;
    }
    else if(trace.length() == 32) {
        // opcode is in the upper 6 bits of the binary representation
        string opcodeStr = trace.substr(0, 6);
        int opcodeInt = stoi(opcodeStr, 0, 2);
        // cout << "[DEBUG - getOpcode]: Opcode (binary): " << opcodeInt << endl;
        return opcodeInt;
    }


        
    return 18; // Return an error code if the opcode is not found
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


void Pipeline::moveStages(string line) {

    clk++;
    decrBusyRegs();
    stages[_WB] = stages[_MEM]; // Pull instruction from MEM to WB
    WB(stages[_WB]);// Writeback the instruction pulled in this stage
    
    stages[_MEM] = stages[_EX]; // Pull instruction from EX to MEM
    MEM(stages[_MEM]); // Memory operation    

    if(stallCondition) {
        stages[_EX].name = "NOP"; // Insert NOP in EX stage if we are stalling
    }
    else {
        stages[_EX] = stages[_ID]; // Pull instruction from ID to EX
    }
    EX(stages[_EX]); // Execute instruction
    
    stages[_ID] = stages[_IF]; // Pull instruction from IF to ID
    ID(stages[_ID]); // Decode instruction
    
    // call IF to fill IF stage if we are not stalling
    if(!stallCondition) {
        if(flushFlag) {
            line = instructionMemory[PC]; // Fetch new instruction
            IF(line);
            flushFlag = false;
            return;
        }
 
        IF(line); // Fetch instruction
 	//decrBusyRegs();       
        std::cout << endl;
        return;    
    }

    //decrBusyRegs();     // Decrement busy registers
    cout << endl;
    moveStages(line);   // Recursively call moveStages until the stall condition is false
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

    uint32_t instruction = stoul(instructionMemory[PC], 0, 16); // should be initialized to 0
    string instr = instructionMemory[PC];
    int mainIndex = 0;
    int lastPC = 0;
    // Move the instruction at the PC counter from the instruction memory to the IF stage
    do{
        // Check for critical problems
        if(criticalProblem) {
            cout << "Critical problem found. Exiting program" << endl;
            break;
        }

        // Print the main index and PC
        cout << "\n[Main]: " << mainIndex++ << "\tPC: " << PC << endl;
        
        // break if HALT instruction is found (IF stage)
        if(!checkHalt(instruction)){
            break;
        }

        moveStages(instr);
        
        // instruction = stoul(instructionMemory[PC], 0, 16); // convert to int for parsing
        instr = instructionMemory[PC];

        // Check if PC is incrementing
        if((lastPC == PC) && mainIndex > 10) {
            cout << "Error: PC is not incrementing" << endl;
            break;
        }

        // Cheat to end execution if we go past what we expect the instruction memory range to be
        if(PC>300 || PC < -4)
        {
            cout << "Error: PC out of bounds" << endl;
            break;
        }
 
        lastPC = PC;

    } while ((PC * 0.25) < (int) instructionMemory.size());   // Continue until HALT is found or PC exceeds the instruction memory range
    cout << "[Main]: HALT instruction found\n\n\n" << endl;
    
    // Empty the pipeline of any remaining instructions
    for(int i = 0; i < 3; i++){
        // moveStages(instruction);
        moveStages(instr);
    	// instruction = stoul(instructionMemory[PC], 0, 16);
        instr = instructionMemory[PC];
    }

    // decrement the typeExecd for arithmetic instructions by 4 since we are flushing by adding 0 to 0
    //typeExecd[0] -= 5;
    PC -= 8;
    clk = clk + 2;
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

void Pipeline::setForwardingFlag(int mode) {
    forwardingFlag = mode;
}
