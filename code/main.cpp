#include "pipeline_s.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int mode = 0;

std::string createBinaryInstruction(int param1, int param2, int param3, int param4);
void printFields(std::string binaryInstruction);
void testBinaryInstructions();

int main(int argc, char* argv[]) {

    // Test binary instructions
    // testBinaryInstructions();
    // return 0;

   if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename> <forwarding>\n";
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
    
    if((argc == 3) && (std::string(argv[2]) == "forwarding")){
    	
    	mode = 1;
    }
   
    
    // Initialize the pipeline
    Pipeline pipeline(mode);
    
    
    std::vector<std::string> fileImage;

    // Read each line of the trace file
    std::string line;
    int i = 0;
    while (std::getline(traceFile, line)) {
        // skip line if it is empty
        if (line.empty()) {
            continue;
        }

        // strip off the 0x (if it exists)
        if (line.substr(0, 2) == "0x") {
            line = line.substr(2);
        }

        // Take only the first 8 characters of the line
        line = line.substr(0, 8);

        // skip line if it is non-hexadecimal
        if (line.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos) {
            continue;
        }

        // Add the line to the file image
        fileImage.push_back(line);
        pipeline.instructionMemory[4*i++] = line;

    }

    // print the instruction memory
    for (auto const& x : pipeline.instructionMemory) {
        std::cout << x.first << ": " << x.second << std::endl;
        
        if(x.second.compare("44000000") == 0) {
            std::cout << "HALT" << std::endl;
            break;
        }
    }

    pipeline.run();
    return 0;

}

#include <bitset>

std::string createBinaryInstruction(int param1, int param2, int param3, int param4) {
    
    std::string binaryString = "";

    // Create bitsets for each parameter
    std::bitset<6> opcode(param1);
    std::bitset<5> rs(param2);
    std::bitset<5> rt(param3);

    // final parameter is either immediate value or destination register
    std::bitset<5> rd(param4);
    std::bitset<16> binaryInstruction(param4);

    // determine if immediate or register addressing
    if (param1 % 2) {
        // Immediate addressing
        binaryString = opcode.to_string() + rs.to_string() + rt.to_string() + binaryInstruction.to_string();
        std::cout << "Immediate: " << binaryString << std::endl;
    } else {
        // Register addressing
        binaryString = opcode.to_string() + rs.to_string() + rt.to_string() + rd.to_string();
    }

    return binaryString;
}


void printFields(std::string binaryInstruction) {
    std::bitset<6> opcode(binaryInstruction.substr(0, 6));
    std::bitset<5> rs(binaryInstruction.substr(6, 5));
    std::bitset<5> rt(binaryInstruction.substr(11, 5));
    std::bitset<5> rd(binaryInstruction.substr(16, 5));
    std::bitset<16> imm(binaryInstruction.substr(16, 16));

    // convert to int
    int opcode1 = opcode.to_ulong();
    int rs1 = rs.to_ulong();
    int rt1 = rt.to_ulong();
    int rd1 = rd.to_ulong();
    int imm1 = imm.to_ulong();
    

    std::cout << "Opcode: " << opcode1 << std::endl;
    std::cout << "rs: " << rs1 << std::endl;
    std::cout << "rt: " << rt1 << std::endl;
    std::cout << "rd: " << rd1 << std::endl;
    std::cout << "imm: " << imm1 << std::endl;
    std::cout << std::endl;
}

void testBinaryInstructions() {

    Pipeline pipeline2(mode);

    // make a new vector of instructions for testing
    std::vector<std::string> testInstructions;
    // testInstructions.push_back(createBinaryInstruction(1, 1, 1, 2));    // ADDI R1, store in R1, imm = 2
    // testInstructions.push_back(createBinaryInstruction(1, 2, 2, 3));    // ADDI R2, store in R2, imm = 3
    // testInstructions.push_back(createBinaryInstruction(5, 1, 3, 2));    // MULTI R1, store in R3, imm = 2
    // testInstructions.push_back(createBinaryInstruction(5, 2, 4, 2));    // MULTI R2, store in R4, imm = 2
    // testInstructions.push_back(createBinaryInstruction(3, 1, 5, 5));    // SUBI R1, store in R5, imm = 5
    // testInstructions.push_back(createBinaryInstruction(0, 1, 2, 6));    // ADD R1, R2, store in R6
    // testInstructions.push_back(createBinaryInstruction(0, 3, 4, 7));    // ADD R3, R4, store in R7


    // Add the natural numbers from 1 to 10 to the registers R1 to R10
    for (int i = 1; i <= 10; i++) {
        testInstructions.push_back(createBinaryInstruction(1, i, i, i));
        // printFields(testInstructions.at(i-1));
    }




    // Print the binary instructions
    for (int i = 0; i < (int) testInstructions.size(); i++) {
        // std::cout << testInstructions.at(i) << std::endl;

        pipeline2.moveStages(std::stoi(testInstructions.at(i), 0, 2));
    }

    pipeline2.printExecutionReport();
    std::cout << "\n\n Clearing pipeline\n\n" << std::endl;

    // Create a nop instruction
    std::string nop = createBinaryInstruction(18, 0, 0, 0);

    for(int i = 0; i < 5; i++) {
        pipeline2.moveStages(std::stoi(nop, 0, 2));
    }

    pipeline2.printExecutionReport();


}
