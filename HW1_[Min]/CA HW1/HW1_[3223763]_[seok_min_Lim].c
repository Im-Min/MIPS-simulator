#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct instruction
{
    enum opcodes opcode;
    int jump;//target line num
    int present;//present line num
    int r[3];//register's name
    int registers[13];//0-9 nomal register 10-12 number to register
};


enum opcodes
{
    ADD = 0,
    SUB,
    MUL,
    DIV,
    MOV,
    LW,
    SW,
    RST,
    SLT,
    BEQ,
    BNE,
    JMP
};

//all register
void add(struct instruction* inst);
void sub(struct instruction* inst);
void mul(struct instruction* inst);
void dIv(struct instruction* inst);
void mov(struct instruction* inst);
void lw(struct instruction* inst);
void sw(struct instruction* inst);
void rst(struct instruction* inst);
void slt(struct instruction* inst);
void beq(struct instruction* inst);
void bne(struct instruction* inst);
void jmp(struct instruction* inst);


void buffer2Inst(struct instruction* inst, char* buffer);

void process(struct instruction* inst);

int lineNum(char* input);


int main() {
    FILE* pFile = NULL;
    const char* filename = "example.txt";//filename
    const char* mode = "r";//file open type
    char buffer[256];

    //inst initialize
    struct instruction* inst;
    inst = malloc(sizeof(struct instruction));//구조체 포인터
    memset(inst->registers, -1, sizeof(int) * 13);//structure memmory allocation
    inst->jump = 0;
    errno_t err = fopen_s(&pFile, filename, mode);

    if (err != 0) {
        // error handling
    }

    if (pFile == NULL) {
        // error handling
    }

    
    while (fgets(buffer, sizeof(buffer), pFile) != NULL) {

        if (inst->jump != 0) {
            inst->jump--;
            printf("skip\n");
            continue;
        }
        if(buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = ' ';
        printf("%s", buffer);
        inst->opcode = -1;
        memset(inst->r, -1, sizeof(int) * 3);

        printf("\n");
        buffer2Inst(inst, buffer);
        process(inst);
        printf("\n");
    }

    fclose(pFile);
    
    return 0;
}


void buffer2Inst(struct instruction* inst, char* buffer) {

    char* token = NULL, * saveptr = NULL;
    char* inputArr[5];
    char* opcodes[] = { "ADD", "SUB", "MUL", "DIV", "MOV", "LW", "SW", "RST", "SLT", "BEQ", "BNE", "JMP" };

    int num_opcodes = sizeof(opcodes) / sizeof(opcodes[0]);
    int seat = 0;

    token = strtok_s(buffer, " ", &saveptr);//string passiing

    while (token != NULL) {
        inputArr[seat] = token;
        token = strtok_s(NULL, " ", &saveptr);
        seat++;
    }

    inst->present = lineNum(inputArr[0]);

    for (int i = 2; i < seat; i++)
    {
        if (inputArr[i][0] == '0')//if num
        {
            int number = (int)strtol(inputArr[i], NULL, 0);
            inst->r[i - 2] = i - 2 + 10;
            inst->registers[i - 2 + 10] = number;
        }
        else if (inputArr[i][0] == 'r')//if register
            inst->r[i - 2] = inputArr[i][1] - 48;

        else if (inputArr[i][0] == 'S')
            continue;

        else// error
        {
            printf("error input is not correct");
            return;
        }
    }

    for (int i = 0; i < num_opcodes; i++) {
        if (strcmp(inputArr[1], opcodes[i]) == 0)
        {
            inst->opcode = i;
        }
    }
}


void process(struct instruction* inst)
{
    int origin = inst->registers[inst->r[0]];

    //분류후 실행
    switch (inst->opcode)
    {
    case 0:
        add(inst);
        printf("ADD r[%d]: %d -> %d\n", inst->r[0], origin, inst->registers[inst->r[0]]);
        break;
    case 1:
        sub(inst);
        printf("SUB r[%d]: %d -> %d\n", inst->r[0], origin, inst->registers[inst->r[0]]);
        break;
    case 2:
        mul(inst);
        printf("MUL r[%d]: %d -> %d\n", inst->r[0], origin, inst->registers[inst->r[0]]);
        break;
    case 3:
        dIv(inst);
        printf("DIV r[%d]: %d -> %d\n", inst->r[0], origin, inst->registers[inst->r[0]]);
        break;
    case 4:
        mov(inst);
        printf("MOV r[%d]: %d -> %d\n", inst->r[0], origin, inst->registers[inst->r[0]]);
        break;
    case 5:
        lw(inst);
        printf("LW r[%d]: %d -> %d\n", inst->r[0], origin, inst->registers[inst->r[0]]);
        break;
    case 6:
        sw(inst);
        printf("SW r[%d]: %d \n", inst->r[0], inst->registers[inst->r[0]]);
        break;
    case 7:
        for (int i = 0; i < 10; i++)
        {
            if (inst->registers[i] != - 1) {
                printf("r[%d]: %d -> %d\n", i, inst->registers[i], -1);
            }
        }
        printf("\n");
        rst(inst);
        break;
    case 8:
        slt(inst);
        printf("SLT r[%d]: %d -> %d\n", inst->r[0], origin, inst->registers[inst->r[0]]);
        break;
    case 9:
        beq(inst);
        break;
    case 10:
        bne(inst);
        break;
    case 11:
        jmp(inst);
        break;
    default:
        printf("invalid opcode");
        break;
    }

}


int lineNum(char* input)
{
    int line = 0;
    int i = 0;

    while (input[i] != ':')
    {
        i++;
    }

    i--;

    for (int j = 0; i >= 0; i--, j++)
    {
        line += (input[i] - 48) * (int)pow(10, j);
    }

    return line;
}


void add(struct instruction* inst)
{
    inst->registers[inst->r[0]] = inst->registers[inst->r[1]] + inst->registers[inst->r[2]];
}
void sub(struct instruction* inst) {
    inst->registers[inst->r[0]] = inst->registers[inst->r[1]] - inst->registers[inst->r[2]];
}
void mul(struct instruction* inst) {
    inst->registers[inst->r[0]] = inst->registers[inst->r[1]] * inst->registers[inst->r[2]];
}
void dIv(struct instruction* inst) {
    inst->registers[inst->r[0]] = inst->registers[inst->r[1]] / inst->registers[inst->r[2]];
}
void mov(struct instruction* inst) {
    inst->registers[inst->r[0]] = inst->registers[inst->r[1]];
}
void lw(struct instruction* inst) {
    inst->registers[inst->r[0]] = inst->registers[inst->r[1]];
}
void sw(struct instruction* inst) {
}
void rst(struct instruction* inst) {
    memset(inst->registers, -1, sizeof(int) * 13);
}
void slt(struct instruction* inst) {
    if (inst->registers[inst->r[1]] < inst->registers[inst->r[2]])
        inst->registers[inst->r[0]] = 1;
    else
        inst->registers[inst->r[0]] = 0;
}
void beq(struct instruction* inst) {
    if (inst->registers[inst->r[0]] == inst->registers[inst->r[1]]) {
        inst->jump = inst->registers[inst->r[2]] - inst->present - 1;
        printf("BEQ\n");
    }
    else
        printf("skip BEQ\n");
}
void bne(struct instruction* inst) {
    if (inst->registers[inst->r[0]] != inst->registers[inst->r[1]]) {
        inst->jump = inst->registers[inst->r[2]] - inst->present - 1;
        printf("BNE\n");
    }
    else
        printf("skip BNE\n");
}
void jmp(struct instruction* inst) {
    printf("JMP\n");
    inst->jump = inst->registers[inst->r[0]] - inst->present - 1;
}