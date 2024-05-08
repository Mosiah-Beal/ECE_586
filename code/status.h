#ifndef STATUS_H
#define STATUS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <map>

class Status {
    public:
        Status();
        void printReport(); // print the number of times each instruction type was executed
	void incrementClk();
	int getClk() const;
        int PC;             // program counter
        int registers[32];  // registers
        int typeExecd[4];   // number of times each instruction type was executed
        std::map<int, int> memory; // memory <address, value>
    private:
    int clk;
};

#endif // STATUS_H
