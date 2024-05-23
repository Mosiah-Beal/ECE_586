#include "pipeline_s.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

void convertHexToInt(const std::string& hexValue);
std::string concatenateSignedBinaryStrings(int opcode, int Rs, int Rt, int IMM_RD);
void buildInstructions();

int main(int argc, char* argv[]) {
    // Initialize the pipeline
    Pipeline pipeline;

    // buildInstructions();

    // Error checking for command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    // Load trace file 
    std::string fileName = std::string(argv[1]);
    std::ifstream traceFile(fileName);
    
    // Error checking for loading file
    if (!traceFile) {
        std::cerr << "Error: could not open file " << fileName << std::endl;
        return 1;
    }

    std::vector<std::string> fileImage;

    // Read each line of the trace file
    std::string line;
    int i = 0;
    while (std::getline(traceFile, line)) {
        // skip line if it is empty
        if (line.empty()) {
            continue;
        }
        fileImage.push_back(line);
        pipeline.instructionMemory[4*i++] = line;
    }


    // Loop through each instruction in the trace file until HALT is found
    // pipeline.run();

    // Print the execution report
    // pipeline.printExecutionReport();

    // For each instruction in the trace file, print the fields of the instruction
    for(int i = 0; i < fileImage.size(); i++) {
        cout << "Instruction " << i << endl;
        convertHexToInt(fileImage[i]);
        cout << endl;

        // if fileImage[i] == 0x00000000 four times in a row, break
        if (i > 3 && fileImage[i] == "00000000" && fileImage[i-1] == "00000000" && fileImage[i-2] == "00000000" && fileImage[i-3] == "00000000") {
            break;
        }
    }

    return 0;
}


void convertHexToInt(const std::string& hexValue) {

    //convert hex string to binary
    std::string binaryValue;
    for (int i = 0; i < hexValue.size(); i++) {
        switch (toupper(hexValue[i])) {
            case '0': binaryValue += "0000"; break;
            case '1': binaryValue += "0001"; break;
            case '2': binaryValue += "0010"; break;
            case '3': binaryValue += "0011"; break;
            case '4': binaryValue += "0100"; break;
            case '5': binaryValue += "0101"; break;
            case '6': binaryValue += "0110"; break;
            case '7': binaryValue += "0111"; break;
            case '8': binaryValue += "1000"; break;
            case '9': binaryValue += "1001"; break;
            case 'A': binaryValue += "1010"; break;
            case 'B': binaryValue += "1011"; break;
            case 'C': binaryValue += "1100"; break;
            case 'D': binaryValue += "1101"; break;
            case 'E': binaryValue += "1110"; break;
            case 'F': binaryValue += "1111"; break;
            default: cout << "Invalid hex digit" << endl; break;
        }
    }

    // convert the hex strings into their signed integer values
    // opcode is first 6 bits
    std::string opcodeStr = binaryValue.substr(0, 6);
    int opcode = std::stoi(opcodeStr, nullptr, 2);

    // Rs is next 5 bits
    std::string RsStr = binaryValue.substr(6, 5);
    int Rs = std::stoi(RsStr, nullptr, 2);

    // Rt is next 5 bits
    std::string RtStr = binaryValue.substr(11, 5);
    int Rt = std::stoi(RtStr, nullptr, 2);

    // if the opcode is odd, then the last 16 bits are the immediate value
    int IMM_RD;
    if (opcode % 2 == 1) {
        std::string IMMStr = binaryValue.substr(16, 16);
        IMM_RD = std::stol(IMMStr, nullptr, 2);
    } else {
        std::string RDStr = binaryValue.substr(16, 5);
        IMM_RD = std::stoi(RDStr, nullptr, 2);
    }


    // Alert the user if the first bit of the immediate value is 1
    if (IMM_RD & 0x8000) {
        std::cout << "Immediate value is negative" << std::endl;
    }
    
    std::cout << "Opcode: " << opcode << std::endl;
    std::cout << "Rs: " << Rs << std::endl;
    std::cout << "Rt: " << Rt << std::endl;

    if(opcode % 2 == 1) {
        std::cout << "IMM: " << IMM_RD << std::endl;
    } else {
        std::cout << "RD: " << IMM_RD << std::endl;
    }
}



std::string concatenateSignedBinaryStrings(int opcode, int Rs, int Rt, int IMM_RD) {
    // Convert the integers to binary strings
    std::string opcodeStr = std::bitset<6>(opcode).to_string();
    std::string RsStr = std::bitset<5>(Rs).to_string();
    std::string RtStr = std::bitset<5>(Rt).to_string();
    std::string IMM_RDStr;

    // Check if the 4th parameter is IMM or RD
    if (IMM_RD >= 0 && IMM_RD <= 0xFFFF) {
        IMM_RDStr = std::bitset<16>(IMM_RD).to_string();
    } else {
        IMM_RDStr = std::bitset<5>(IMM_RD).to_string() + "00000000000";
    }

    // Concatenate the binary strings
    std::string result = opcodeStr + RsStr + RtStr + IMM_RDStr;

    return result;
}


void buildInstructions() {
        // Test the concatenateSignedBinaryStrings function
    while (true) {
        int opcode, Rs, Rt, IMM_RD;
        std::cout << "Enter opcode: ";
        std::cin >> opcode;
        std::cout << "Enter Rs: ";
        std::cin >> Rs;
        std::cout << "Enter Rt: ";
        std::cin >> Rt;
        std::cout << "Enter IMM_RD: ";
        std::cin >> IMM_RD;

        if (std::cin.fail()) {
            // Input was not a valid integer, exit the loop
            break;
        }

        std::string result = concatenateSignedBinaryStrings(opcode, Rs, Rt, IMM_RD);
        std::cout << "Result: " << result << std::endl;
        std::cout << "Numbers: " << opcode << ", " << Rs << ", " << Rt << ", " << IMM_RD << std::endl;

        // convert binary string to hex
        std::bitset<32> b(result);
        std::cout << "Hex: " << std::hex << b.to_ulong() << std::endl;

        // break the loop if the user enters -1
        std::cout << "Enter -1 to exit: ";
        std::cin >> opcode;
        if (opcode == -1) {
            break;
        }        
    }


}