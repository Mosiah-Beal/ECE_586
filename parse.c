#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INSTRUCTIONS 1000
#define NUM_REGISTERS 32
#define ZERO 0
#define AT 1
#define V0 2
#define V1 3
#define A0 4
#define A1 5
#define A2 6
#define A3 7
#define T0 8
#define T1 9
#define T2 10
#define T3 11
#define T4 12
#define T5 13
#define T6 14
#define T7 15
#define T8 24
#define T9 25
#define S0 16
#define S1 17
#define S2 18
#define S3 19
#define S4 20
#define S5 21
#define S6 22
#define S7 23
#define K0 26
#define K1 27
#define GP 28
#define SP 29
#define FP 30
#define RA 31 

typedef struct {
 unsigned int opcode;
 unsigned int rs;
 unsigned int rt;
 unsigned int rd;
 unsigned int shift;
 unsigned int funct;
} rtype_t;

typedef struct {
    unsigned int opcode;
    unsigned int rs;
    unsigned int rt;
    unsigned int i_address;
} itype_t;

typedef struct {
  unsigned int opcode;
  unsigned int j_address;
} jtype_t;

typedef struct {
    rtype_t* rtype_instr;
    itype_t* itype_instr;
    jtype_t* jtype_instr;
} instruction_t;


unsigned int registers[NUM_REGISTERS];


void ADD (instruction_t* instruction){

registers[instuction->rtype_t->rd] = registers[instruction->rtype_t->rs] + registers[instruction->rtype_t->rt];

}


void parseFile(const char* filename, long* instructions) {
	FILE* file = fopen(filename, "r");
	if (file == NULL){
	printf("Failed to open file: %s\n", filename);
	}

    char hex[33];
   
    int i = 0;
    
    while (fscanf(file, "%32s", hex) == 1) {
	instructions[i] = strtol(hex, NULL, 16);
	printf("instruction: %lx\n", instructions[i]);
	i++;
    }

 }

instruction_t* populateStruct(long* bin) {
    unsigned int mask;
    instruction_t instruction;
    instruction.rtype_instr = malloc(sizeof(rtype_t));
    instruction.itype_instr = malloc(sizeof(itype_t));
    instruction.jtype_instr = malloc(sizeof(jtype_t));
     
    int i = 0;

    // R-Type
    mask = 0xFC000000; // Mask for opcode (6 bits)
    instruction.rtype_instr->opcode = (mask & bin[i]) >> 26;
    mask = 0x03E00000; // Mask for rs (5 bits)
    instruction.rtype_instr->rs = (mask & bin[i]) >> 21;
    mask = 0x001F0000; // Mask for rt (5 bits)
    instruction.rtype_instr->rt = (mask & bin[i]) >> 16;
    mask = 0x0000F800; // Mask for rd (5 bits)
    instruction.rtype_instr->rd = (mask & bin[i]) >> 11;
    mask = 0x000007C0; // Mask for shift (5 bits)
    instruction.rtype_instr->shift = (mask & bin[i]) >> 6;
    mask = 0x0000003F; // Mask for funct (6 bits)
    instruction.rtype_instr->funct = mask & bin[i];

    // I-Type
    mask = 0xFC000000; // Mask for opcode (6 bits)
    instruction.itype_instr->opcode = (mask & bin[i]) >> 26;
    mask = 0x03E00000; // Mask for rs (5 bits)
    instruction.itype_instr->rs = (mask & bin[i]) >> 21;
    mask = 0x001F0000; // Mask for rt (5 bits)
    instruction.itype_instr->rt = (mask & bin[i]) >> 16;
    mask = 0x0000FFFF; // Mask for i_address (16 bits)
    instruction.itype_instr->i_address = mask & bini1];

    // J-Type
    mask = 0xFC000000; // Mask for opcode (6 bits)
    instruction.jtype_instr->opcode = (mask & bin[i]) >> 26;
    mask = 0x03FFFFFF; // Mask for j_address (26 bits)
    instruction.jtype_instr->j_address = mask & bin[i];

    printf("R-Type Opcode: %X\n", instruction.rtype_instr->opcode);
    printf("I-Type Opcode: %X\n", instruction.itype_instr->opcode);
    printf("J-Type Opcode: %X\n", instruction.jtype_instr->opcode);
    free(instruction.rtype_instr);
    free(instruction.itype_instr);
    free(instruction.jtype_instr);

    return instruction; 
}



int main() {
long instructions[MAX_INSTRUCTIONS];
parseFile("instructions.txt", instructions);
populateStruct(instructions);
return 0;
}


  
/*
void hexToBin(char hex, char* bin) {
    switch (hex) {
        case '0': strcpy(bin, "0000"); break;
        case '1': strcpy(bin, "0001"); break;
        case '2': strcpy(bin, "0010"); break;
        case '3': strcpy(bin, "0011"); break;
        case '4': strcpy(bin, "0100"); break;
        case '5': strcpy(bin, "0101"); break;
        case '6': strcpy(bin, "0110"); break;
        case '7': strcpy(bin, "0111"); break;
        case '8': strcpy(bin, "1000"); break;
        case '9': strcpy(bin, "1001"); break;
        case 'A': case 'a': strcpy(bin, "1010"); break;
        case 'B': case 'b': strcpy(bin, "1011"); break;
        case 'C': case 'c': strcpy(bin, "1100"); break;
        case 'D': case 'd': strcpy(bin, "1101"); break;
        case 'E': case 'e': strcpy(bin, "1110"); break;
        case 'F': case 'f': strcpy(bin, "1111"); break;
        default: strcpy(bin, ""); break;
    }
}

void parseFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    char hex[33];
    while (fscanf(file, "%32s", hex) == 1) {
        char bin[129] = "";
        for (int i = 0; i < 32; i++) {
            char binDigit[5];
            hexToBin(hex[i], binDigit);
            strcat(bin, binDigit);
        }

        instruction_t instruction;
        instruction.rtype_instr = malloc(sizeof(rtype_t));
        instruction.itype_instr = malloc(sizeof(itype_t));
        instruction.jtype_instr = malloc(sizeof(jtype_t));

        int pos = 0;

        strncpy(instruction.rtype_instr->opcode, bin + pos, 6); pos += 6;
        strncpy(instruction.rtype_instr->rs, bin + pos, 5); pos += 5;
        strncpy(instruction.rtype_instr->rt, bin + pos, 5); pos += 5;
        strncpy(instruction.rtype_instr->rd, bin + pos, 5); pos += 5;
        strncpy(instruction.rtype_instr->shift, bin + pos, 5); pos += 5;
        strncpy(instruction.rtype_instr->funct, bin + pos, 6); pos += 6;

        strncpy(instruction.itype_instr->opcode, bin + pos, 6); pos += 6;
        strncpy(instruction.itype_instr->rs, bin + pos, 5); pos += 5;
        strncpy(instruction.itype_instr->rt, bin + pos, 5); pos += 5;
        strncpy(instruction.itype_instr->i_address, bin + pos, 16); pos += 16;

        strncpy(instruction.jtype_instr->opcode,bin + pos, 6); pos += 6;
        strncpy(instruction.jtype_instr->j_address, bin + pos, 26); pos += 26;

        
        printf("R-Type Opcode: %s\n", instruction.rtype_instr->opcode);
	printf("I-Type Opcode: %s\n", instruction.itype_instr->opcode);
	printf("J-Type Opcode; %s\n", instruction.jtype_instr->opcode);
       
        free(instruction.rtype_instr);
        free(instruction.itype_instr);
        free(instruction.jtype_instr);
    }

    fclose(file);
}

int main() {
    parseFile("instructions.txt");
    return 0;
}
*/ 
