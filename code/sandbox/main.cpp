#include "pipeline_s.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize the pipeline
    Pipeline pipeline;

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
    while (std::getline(traceFile, line)) {
        // skip line if it is empty
        if (line.empty()) {
            continue;
        }
        fileImage.push_back(line);
    }

    int instrIndex = 0; // index of instruction in file
    int nxtInstr = 0; // next instruction to be processed

    // Loop through each instruction in the trace file until HALT is found
    pipeline.run();

    // Print the execution report
    pipeline.printExecutionReport();

    return 0;
}
