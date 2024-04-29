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
#include "decoder.h"
#include "instructions.h"
#include "status.h"
#include "pipeline.h"
using namespace std;

int main (int argc, char* argv[]) {
    // Initialize objects
    Decoder decoder;
    Status status;
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
        int nxtInstr = stoi(nxtInstrStr, 0, 16); // convert to int for parsing

        pipeline.moveStages(nxtInstr); // move stages in pipeline
        // End if HALT instruction is found
        if (decoder.nextInstr(nxtInstr, status) == 0) {
            break;
        }
    }

    status.printReport();
    return 0;
}