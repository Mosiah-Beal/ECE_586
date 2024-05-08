#include "status.h"

Status::Status() {
    std::fill_n(typeExecd, 4, 0);
    std::fill_n(registers, 32, 0);
    PC = 0;
    clk = 1;
    memory.clear();
}

void Status::printReport() {
    std::cout << std::endl << "Arithmetic instructions executed: " << typeExecd[0] << std::endl;
    std::cout << "Logical instructions executed: " << typeExecd[1] << std::endl;
    std::cout << "Memory access instructions executed: " << typeExecd[2] << std::endl;
    std::cout << "Control flow instructions executed: " << typeExecd[3] << std::endl;

    std::cout << "Program Counter: " << PC << std::endl;
    
    std::cout << "Clock Count: " << getClk() << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "R" << i << ": " << registers[i] << std::endl;
    }

    for (auto const& x : memory) {
        std::cout << "Memory[" << x.first << "]: " << x.second << std::endl;
    }
    std::cout << std::endl;

}


void Status::incrementClk() {
clk++;
}

int Status::getClk() const {
return clk;
}
