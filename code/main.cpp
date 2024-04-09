#include "decoder.h"
#include "instructions.h"

int main (int argc, char* argv[]) {
    Instructions instructions;
    Decoder decoder;
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string fileName = std::string(argv[1]);

    decoder.parseTraceFile(fileName);
    return 0;
}