#include "decoder.h"
#include "instructions.h"

int main (int argc, char* argv[]) {
    Instructions instSet;
    Decoder decoder;

    // Error checking for command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string fileName = std::string(argv[1]);
    std::ifstream traceFile(fileName);
    
    if (!traceFile) {
        std::cerr << "Error: could not open file " << fileName << std::endl;
        return 1;
    }

    std::string nxtInstrStr; // raw value from trace file
    while (std::getline(traceFile, nxtInstrStr)) {
        int nxtInstr = std::stoi(nxtInstrStr, 0, 16); // convert to int for parsing
        // decoder.parseInstr(nxtInstr);

        // End if HALT instruction is found
        if (decoder.parseInstr(nxtInstr) == 0) {
            break;
        }

        #ifdef DEBUG
        printf("Instruction: ");
        printBinary(nxtInstr);
        #endif
    }

    decoder.printReport();

    return 0;
}