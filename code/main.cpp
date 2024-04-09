#include "decoder.h"
#include "instructions.h"
#include "status.h"

int main (int argc, char* argv[]) {
    // Initialize objects
    Instructions instSet;
    Decoder decoder;
    Status status;

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

    std::string nxtInstrStr; // raw value from trace file
    while (std::getline(traceFile, nxtInstrStr)) {
        int nxtInstr = std::stoi(nxtInstrStr, 0, 16); // convert to int for parsing

        // End if HALT instruction is found
        if (decoder.nextInstr(nxtInstr, status) == 0) {
            break;
        }
    }

    status.printReport();
    return 0;
}