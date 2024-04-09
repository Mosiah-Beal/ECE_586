#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

void printBinary(int n); // used for debugging
int parseTrace(int trace);

int main () {
    char *fileName = "./traces/short.txt";
    FILE *traceFile = fopen(fileName, "r");

    if (traceFile == NULL) {
        printf("Error: Could not open file %s\n", traceFile);
        return 1;
    } else if  (traceFile != NULL) {
        printf("File %s opened successfully\n", traceFile);
    }

    char traceStr[32];
    while (fgets(traceStr, sizeof(traceStr), traceFile) != NULL) {
        // Clean up trace and convert to decimal int value for easier
        traceStr[strcspn(traceStr, "\n")] = 0; // skip newline character
        long trace = strtol(traceStr, NULL, 16);

        #ifdef DEBUG
        printf("TraceStr: %s\n", traceStr);
        printf("Trace Value: %ld\n", trace);
        printBinary(trace);
        #endif

        parseTrace(trace);
    }

    fclose(traceFile);
    return 0;
}

void printBinary(int n) {
    unsigned i;
    for (i = 1 << 31; i > 0; i = i / 2) {
        (n & i) ? printf("1") : printf("0");
    }
    printf("\n");
}

int parseTrace(int trace) {
    int opcode = (trace & 0xFC000000) >> 26;

    #ifdef DEBUG
    printf("Opcode: ");
    printBinary(opcode);
    #endif

    return opcode;
}
