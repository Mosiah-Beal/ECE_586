/**
 * @file main.cpp
 * @author Lynn Nguyen (diemhan@pdx.edu)
 * @brief Functional MIPS simulator
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
// #include "decoder.h"
#include "instructions.h"
#include "status.h"
#include "pipeline.h"
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

void testing(string bin);
void showBitFields(string instruction);
string makeBin(string hex);
void listOpcodes(void);

int main (int argc, char* argv[]) {
    // Initialize the pipeline
    Pipeline pipeline;
    vector<string> fileImage;

    // Error checking for command line arguments
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    // Load trace file 
    string fileName = string(argv[1]);
    ifstream traceFile(fileName);
    
    // Error checking for loading file
    if (!traceFile) {
        cerr << "Error: could not open file " << fileName << endl;
        return 1;
    }

    // Read each line of the trace file
    string line;
    while (getline(traceFile, line)) {

        // skip line if it is empty
        if (line.empty()) {
            continue;
        }

        // cout << "Not empty" << endl;

        // strip out leading 0x if present
        if (line.substr(0, 2) == "0x") {
            line = line.substr(2);
            // cout << "Stripped 0x" << endl;
        }

        // skip line if it is non-hexadecimal
        if (line.find_first_not_of("0123456789ABCDEF") != string::npos) {
            continue;
        }

        // cout << "Hexadecimal" << endl;

        // cout << "Line: " << line << endl;
        fileImage.push_back(line);
    }

    // print each line of the trace file
    for (int i = 0; i < (int) fileImage.size(); i++) {
        //cout << fileImage.at(i) << endl;

        int nxtInstr = stoi(fileImage.at(i), 0, 16); // convert to int for parsing

        // show the opcode of the instruction added using the friendly name
        string opcodeName = pipeline.instructionSet.getInstruction(pipeline.getOpcode(nxtInstr)).name;
        cout << "Instruction [" << i << "]: " << fileImage.at(i) << " (" << opcodeName << ")" << endl;
        // showBitFields(fileImage.at(i)); // show bit fields of instruction (for debugging)
        


    }

    cout << endl;


    string nxtInstrStr; // raw value from trace file
    int instrIndex = 0; // index of instruction in file
    int nxtInstr = 0; // integer value of instruction

    // Loop through each instruction in the trace file until HALT is found
    do {
        int nxtInstr = stoi(fileImage.at(instrIndex++), 0, 16); // convert to int for parsing
        // showBitFields(fileImage.at(instrIndex-1)); // show bit fields of instruction (for debugging)
        cout << "[Main]: " << instrIndex-1 << endl;
        pipeline.moveStages(nxtInstr); // move stages in pipeline
    }
    while(pipeline.checkHalt(nxtInstr) && instrIndex < (int) fileImage.size());

    pipeline.status.printReport();
    // listOpcodes();
    return 0;
}

void showBitFields(string instruction) {
    // Temporary string to hold binary representation
    string binary = "";
    string instructionBinary = "";
    cout << "Hex instruction: " << instruction << endl;

    // Expand hex to binary by converting each hex digit to binary and concatenating
    for(int i = 0; i < (int) instruction.length(); i++) {
        binary = makeBin(instruction.substr(i, 1));
        instructionBinary += binary;
    }

    // Show binary representation (every 4 bits separated by a space)
    for(int i = 0; i < (int) instructionBinary.length(); i++) {
        cout << instructionBinary.at(i);
        if ((i + 1) % 4 == 0) {
            cout << " ";

            // if we are at the end of the string, print a new line
            if (i == (int) instructionBinary.length() - 1) {
                cout << endl;
            }
        }
    }

    // Extract bit fields from instruction
    string opcode = instructionBinary.substr(0, 6);
    string rs = instructionBinary.substr(6, 5);
    string rt = instructionBinary.substr(11, 5);
    string rd = instructionBinary.substr(16, 5);
    string imm = instructionBinary.substr(16, 16);

    // Show bit fields of instruction as strings
    // cout << "Opcode: " << opcode << endl;
    // cout << "RS: " << rs << endl;
    // cout << "RT: " << rt << endl;
    // cout << "RD: " << rd << endl;
    // cout << "Immediate: " << imm << endl;

    // make it known if the imm value has a leading 1
    if (imm.at(0) == '1') {
        // cout << "[main] Immediate value is negative" << endl;
    }

    // Show the instruction in binary
    // cout << "Binary instruction:" << endl;

    int opcodeInt = stoi(opcode, 0, 2);
    int rsInt = stoi(rs, 0, 2);
    int rtInt = stoi(rt, 0, 2);
    int rdInt = stoi(rd, 0, 2);
    int immInt = stoi(imm, 0, 2);

    // show them as integers
    // cout << "Opcode: " << opcodeInt << endl;
    cout << "RS: " << rsInt << endl;
    cout << "RT: " << rtInt << endl;
    cout << "RD: " << rdInt << endl;
    cout << "Immediate: " << immInt << endl;

    // cout << "Integer instruction: " << endl;
    int instr = stoi(instruction, 0, 16);
    instr += 0;
    
    // testing(imm);
    cout << endl;
}


string makeBin(string hex) {
    string binary = "";
    switch (toupper(hex.at(0))) {
        case '0':
            binary += "0000";
            break;
        case '1':
            binary += "0001";
            break;
        case '2':
            binary += "0010";
            break;
        case '3':
            binary += "0011";
            break;
        case '4':
            binary += "0100";
            break;
        case '5':
            binary += "0101";
            break;
        case '6':
            binary += "0110";
            break;
        case '7':
            binary += "0111";
            break;
        case '8':
            binary += "1000";
            break;
        case '9':
            binary += "1001";
            break;
        case 'A':
            binary += "1010";
            break;
        case 'B':
            binary += "1011";
            break;
        case 'C':
            binary += "1100";
            break;
        case 'D':
            binary += "1101";
            break;
        case 'E':
            binary += "1110";
            break;
        case 'F':
            binary += "1111";
            break;
        default:
            cout << "Invalid hex digit" << endl;
    }
    return binary;
}

/**
 * @brief Given a binary string, print the different representations
 *  prints unsigned int, signed int, and 2's complement
 *  also prints the number of bits and what the half value is
 *  (half value being the value that splits the range of the binary string)
 *
 * @param bin binary string to test
 */
void testing(string bin) {

    // find the number of bits in the binary string
    int numBits = bin.length();

    // find the half value of the binary string
    int halfValue = 1 << (numBits - 1);

    // Convert the binary string to an unsigned int
    int unsignedInt = stoi(bin, 0, 2);

    // Convert the binary string to a signed int
    int signedInt = unsignedInt;

    // Check if the most significant bit is 1 (negative number)
    if (bin.at(0) == '1') {
        // subtract the MSB from the rest of the number and invert the sign
        signedInt -= 1 << numBits;
        signedInt *= -1;
    }

    // Convert the unsigned value to 2's complement (if necessary)
    int Complement_2s = (unsignedInt > halfValue-1) ? ~unsignedInt + 1 : unsignedInt;
   
    cout << "Binary: " << bin << endl;
    cout << "Number of bits: " << numBits << endl;
    cout << "Half value: " << halfValue << endl;
    cout << "Unsigned int: " << unsignedInt << endl;
    cout << "Signed int: " << signedInt << endl;
    cout << "2's complement: " << Complement_2s << endl;
    
}


void listOpcodes(void) {
    // List of opcodes as hex values
    cout << "ADD: 0x00" << endl;
    cout << "SUB: 0x02" << endl;
    cout << "MUL: 0x04" << endl;
    cout << "OR: 0x06" << endl;
    cout << "AND: 0x08" << endl;
    cout << "XOR: 0x0A" << endl;
    cout << "LDW: 0x0C" << endl;
    cout << "STW: 0x0D" << endl;
    cout << "ADDI: 0x01" << endl;
    cout << "SUBI: 0x03" << endl;
    cout << "MULI: 0x05" << endl;
    cout << "ORI: 0x07" << endl;
    cout << "ANDI: 0x09" << endl;
    cout << "XORI: 0x0B" << endl;
    cout << "BEQ: 0x0F" << endl;
    cout << "BZ: 0x0E" << endl;
    cout << "JR: 0x10" << endl;
    cout << "HALT: 0x11" << endl;

}