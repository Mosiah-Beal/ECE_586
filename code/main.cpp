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

int main (int argc, char* argv[]) {
    // Initialize objects
    // Decoder decoder;
    // Status status;
    Pipeline pipeline;

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

    string nxtInstrStr; // raw value from trace file
    while (getline(traceFile, nxtInstrStr)) {
        cout << "Next instruction: " << nxtInstrStr << endl;
        // showBitFields(nxtInstrStr); // show bit fields of instruction (for debugging
        // cout << endl;

        int nxtInstr = stoi(nxtInstrStr, 0, 16); // convert to int for parsing

        pipeline.moveStages(nxtInstr); // move stages in pipeline
        // End if HALT instruction is found
        if (pipeline.checkHalt(nxtInstr) == 0) {
            break;
        }
    }

    pipeline.status.printReport();
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

    // make it known if the imm value has a leading 1
    if (imm.at(0) == '1') {
        cout << "[main] Immediate value is negative" << endl;
    }

    // Show the instruction in binary
    // cout << "Binary instruction:" << endl;

    // Show bit fields of instruction as strings
    // cout << "Opcode: " << opcode << endl;
    // cout << "RS: " << rs << endl;
    // cout << "RT: " << rt << endl;
    // cout << "RD: " << rd << endl;
    // cout << "Immediate: " << imm << endl;

    // show them as integers
    // cout << "Opcode: " << stoi(opcode, 0, 2) << endl;
    // cout << "RS: " << stoi(rs, 0, 2) << endl;
    // cout << "RT: " << stoi(rt, 0, 2) << endl;
    // cout << "RD: " << stoi(rd, 0, 2) << endl;
    // cout << "Immediate: " << stoi(imm, 0, 2) << endl;

    // cout << "Integer instruction: " << endl;
    int instr = stoi(instruction, 0, 16);
    
    // show the integer value and the bit fields
    // cout << "Instruction: " << instr << endl;
    // cout << "Opcode: " << (instr >> 26) << endl;
    // cout << "RS: " << ((instr >> 21) & 0x1F) << endl;
    // cout << "RT: " << ((instr >> 16) & 0x1F) << endl;
    // cout << "RD: " << ((instr >> 11) & 0x1F) << endl;
    // cout << "Immediate: " << (instr & 0xFFFF) << endl; 

    testing(imm);

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
