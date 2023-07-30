#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Control Signals
typedef struct {
    //Excution/Address Calculation
    unsigned int RegDst;
    unsigned int ALUSrc;
    unsigned int ALUOp;

    //Memory Access
    unsigned int Branch;
    unsigned int MemRead;
    unsigned int MemWrite;
    unsigned int Jump;

    //Write Back
    unsigned int MemtoReg;
    unsigned int RegWrite;

    unsigned int stall;
} ControlSignals;

//IF/ID Register
typedef struct {
    unsigned int in;
    unsigned int savePC;
    unsigned int inst;
} IF_ID;

//ID/EX Register
typedef struct {
    unsigned int in;
    ControlSignals* cs;
    unsigned int savePC;
    unsigned int rt;
    unsigned int rd;
    unsigned int imm;
    unsigned int readData1;
    unsigned int readData2;

    unsigned int shamt;
    unsigned int funct;
    unsigned int addr;
    unsigned int opcode;
} ID_EX;

//EX/MEM Register
typedef struct {
    unsigned int in;
	ControlSignals* cs;
	unsigned int savePC;
	unsigned int aluResult;
    unsigned int zero;
    unsigned int writeData;
	unsigned int writeRedister;
    unsigned int opcode;
    unsigned int jumpAddr;
} EX_MEM;

//MEM/WB Register
typedef struct {
    unsigned int in;
	ControlSignals* cs;
	unsigned int readData;
    unsigned int aluResult;
	unsigned int writeRedister;
    unsigned int opcode;
} MEM_WB;

unsigned int Mux(unsigned int a, unsigned int b, unsigned int cs) //0,1
{
    if(cs==0)
		return a;
	else
		return b;
}
void Control_Unit(ID_EX* id_ex, unsigned int opcode, unsigned int* reg)
{
    switch (opcode)
    {

    case 0X02:
        printf("  type: J, opcode: 0X%X (Jump), addr:0X%X\n", opcode, id_ex->addr << 2 | (id_ex->savePC & 0xF0000000));
        break;

    case 0X03:
        printf("  type: J, opcode: 0X%X (Jump and Link), addr:0X%X\n", opcode, id_ex->addr << 2 | (id_ex->savePC & 0xF0000000));
        break;

    case 0X04:
        printf("  type: I, opcode: 0X%X (Beq), rs: 0X%X (R[%d]=0X%X), rt: 0X%X (R[%d]=0X%X), imm: 0X%X\n", opcode, id_ex->readData1, id_ex->readData1, reg[id_ex->readData1], id_ex->rt, id_ex->rt, reg[id_ex->rt], id_ex->imm);//b
        id_ex->cs->ALUOp = 8;
        break;

    case 0x05:
        printf("  type: I, opcode: 0X%X (Bne), rs: 0X%X (R[%d]=0X%X), rt: 0X%X (R[%d]=0X%X), imm: 0X%X\n", opcode, id_ex->readData1, id_ex->readData1, reg[id_ex->readData1], id_ex->rt, id_ex->rt, reg[id_ex->rt], id_ex->imm);//b//bnez
        id_ex->cs->ALUOp = 8;
        break;

    case 0X09:
        printf("  type: I, opcode: 0X%X (Addiu), rs: 0X%X (R[%d]=0X%X), rt: 0X%X (R[%d]=0X%X), imm: 0X%X\n", opcode, id_ex->readData1, id_ex->readData1, reg[id_ex->readData1], id_ex->rt, id_ex->rt, reg[id_ex->rt], id_ex->imm);//li
        id_ex->cs->ALUOp = 0;
        break;

    case 0X0A:
        printf("  type: I, opcode: 0X%X (Slti), rs: 0X%X (R[%d]=0X%X), rt: 0X%X (R[%d]=0X%X), imm: 0X%X\n", opcode, id_ex->readData1, id_ex->readData1, reg[id_ex->readData1], id_ex->rt, id_ex->rt, reg[id_ex->rt], id_ex->imm);
        id_ex->cs->ALUOp = 5;
        break;

    case 0X23:
        printf("  type: I, opcode: 0X%X (Lw), rs: 0X%X (R[%d]=0X%X), rt: 0X%X (R[%d]=0X%X), imm: 0X%X\n", opcode, id_ex->readData1, id_ex->readData1, reg[id_ex->readData1], id_ex->rt, id_ex->rt, reg[id_ex->rt], id_ex->imm);
        id_ex->cs->ALUOp = 0;
        break;

    case 0X2B:
        printf("  type: I, opcode: 0X%X (Sw), rs: 0X%X (R[%d]=0X%X), rt: 0X%X (R[%d]=0X%X), imm: 0X%X\n", opcode, id_ex->readData1, id_ex->readData1, reg[id_ex->readData1], id_ex->rt, id_ex->rt, reg[id_ex->rt], id_ex->imm);
        id_ex->cs->ALUOp = 0;
        break;

    case 0:
        printf("  type: R, opcode: 0X%X (", opcode);
        switch (id_ex->funct)
        {
        case 0X00:
            printf("Sll");//nop
            id_ex->cs->ALUOp = 6;
            break;

        case 0X02:
            printf("Srl");
            id_ex->cs->ALUOp = 7;
            break;

        case 0X08:
            printf("Jr");
            break;

        case 0X20:
            printf("Add");
            id_ex->cs->ALUOp = 0;
            break;

        case 0X21:
            printf("Addu");
            id_ex->cs->ALUOp = 0;
            break;

        case 0X22:
            printf("Sub");
            id_ex->cs->ALUOp = 1;
            break;

        case 0X23:
            printf("Subu");
            id_ex->cs->ALUOp = 1;
            break;

        case 0X24:
            printf("And");
            id_ex->cs->ALUOp = 2;
            break;

        case 0X25:
            printf("Or");//move
            id_ex->cs->ALUOp = 3;
            break;

        case 0X27:
            printf("Nor");
            id_ex->cs->ALUOp = 4;
            break;

        case 0X2A:
            printf("Slt");
            id_ex->cs->ALUOp = 5;
            break;

        case 0X2B:
            printf("Sltu");
            id_ex->cs->ALUOp = 5;
            break;

        default:
            printf("error");
            break;
        }
        printf("), rs: 0X%X (R[%d]=0X%X), rt: 0X%X (R[%d]=0X%X), rd: 0X%X (R[%d]=0X%X), shamt: 0X%X, funct: 0X%X\n", id_ex->readData1, id_ex->readData1, reg[id_ex->readData1], id_ex->readData2, id_ex->readData2, reg[id_ex->readData2], id_ex->rd, id_ex->rd, reg[id_ex->rd], id_ex->shamt, id_ex->funct);

    default:
        break;
    }

    unsigned int op0 = 0, op1 = 0, op2 = 0, op3 = 0, op4 = 0, op5 = 0;
    unsigned int rformat = 0, lw = 0, sw = 0, beq = 0, jump = 0;

    if (opcode == 0 | opcode == 0X23 | opcode == 0X2B | opcode == 0X04 | opcode == 0X05 | opcode == 0X02)
    {
        op0 = opcode & 0x1;           // 0x1 = 0000 0001
        op1 = (opcode & 0x2) >> 1;    // 0x1 = 0000 0010
        op2 = (opcode & 0x4) >> 2;    // 0x1 = 0000 0100
        op3 = (opcode & 0x8) >> 3;    // 0x1 = 0000 1000
        op4 = (opcode & 0x10) >> 4;   // 0x1 = 0001 0000
        op5 = (opcode & 0x20) >> 5;   // 0x1 = 0010 0000

        rformat = ((~op0) & 0x1) & ((~op1) & 0x1) & ((~op2) & 0x1) & ((~op3) & 0x1) & ((~op4) & 0x1) & ((~op5) & 0x1);
        lw = op0 & op1 & ((~op2) & 0x1) & ((~op3) & 0x1) & ((~op4) & 0x1) & op5;
        sw = op0 & op1 & ((~op2) & 0x1) & op3 & ((~op4) & 0x1) & op5;
        beq = ((~op0) & 0x1) & ((~op1) & 0x1) & op2 & ((~op3) & 0x1) & ((~op4) & 0x1) & ((~op5) & 0x1);
        jump = ((~op0) & 0x1) & ((~op1) & 0x1) & ((~op2) & 0x1) & ((~op3) & 0x1) & op4 & ((~op5) & 0x1);

        id_ex->cs->RegDst = rformat;
        id_ex->cs->ALUSrc = lw | sw;
        id_ex->cs->MemtoReg = lw;
        id_ex->cs->RegWrite = rformat | lw;
        id_ex->cs->MemRead = lw;
        id_ex->cs->MemWrite = sw;
        id_ex->cs->Branch = beq;
        id_ex->cs->Jump = jump;

        if (id_ex->funct == 0X08)
        {
            id_ex->cs->RegDst = 0;
            id_ex->cs->ALUSrc = 0;
            id_ex->cs->MemtoReg = 0;
            id_ex->cs->RegWrite = 0;
            id_ex->cs->MemRead = 0;
            id_ex->cs->MemWrite = 0;
            id_ex->cs->Branch = 0;
            id_ex->cs->Jump = 1;
        }

        if (opcode == 0X05)
            id_ex->cs->Branch = 0;
    }
    else if (opcode == 0X09 | opcode == 0X0A)
    {
        id_ex->cs->RegDst = 0;
        id_ex->cs->ALUSrc = 1;
        id_ex->cs->MemtoReg = 0;
        id_ex->cs->RegWrite = 1;
        id_ex->cs->MemRead = 0;
        id_ex->cs->MemWrite = 0;
        id_ex->cs->Branch = 0;
        id_ex->cs->Jump = 0;

    }
    else if (opcode == 0X0A)
    {
        id_ex->cs->RegDst = 0;
        id_ex->cs->ALUSrc = 1;
        id_ex->cs->MemtoReg = 0;
        id_ex->cs->RegWrite = 0;
        id_ex->cs->MemRead = 0;
        id_ex->cs->MemWrite = 0;
        id_ex->cs->Branch = 0;
        id_ex->cs->Jump = 0;
    }
    else if (opcode == 0X03)
    {
        id_ex->cs->RegDst = 1;
        id_ex->cs->RegWrite = 1;
        id_ex->cs->Jump = 1;
        id_ex->cs->ALUSrc = 1;
    }
}
void copyIFID(IF_ID* if_id) {
    if_id[1].in = if_id[0].in;
    if_id[1].inst = if_id[0].inst;
    if_id[1].savePC = if_id[0].savePC;

    memset(if_id, 0, sizeof(IF_ID));
}
void copyIDEX(ID_EX* id_ex) {
    id_ex[1].in = id_ex[0].in;
    id_ex[1].cs = id_ex[0].cs;
    id_ex[1].savePC = id_ex[0].savePC;
    id_ex[1].rt = id_ex[0].rt;
    id_ex[1].rd = id_ex[0].rd;
    id_ex[1].imm = id_ex[0].imm;
    id_ex[1].readData1 = id_ex[0].readData1;
    id_ex[1].readData2 = id_ex[0].readData2;

    id_ex[1].shamt = id_ex[0].shamt;
    id_ex[1].funct = id_ex[0].funct;
    id_ex[1].addr = id_ex[0].addr;
    id_ex[1].opcode = id_ex[0].opcode;

    memset(id_ex, 0, sizeof(ID_EX));
}
void copyEXMEM(EX_MEM* ex_mem) {
	ex_mem[1].in = ex_mem[0].in;
	ex_mem[1].cs = ex_mem[0].cs;
	ex_mem[1].savePC = ex_mem[0].savePC;
    ex_mem[1].aluResult = ex_mem[0].aluResult;
    ex_mem[1].zero = ex_mem[0].zero;
    ex_mem[1].writeData = ex_mem[0].writeData;
    ex_mem[1].writeRedister = ex_mem[0].writeRedister;
	ex_mem[1].opcode = ex_mem[0].opcode;
    ex_mem[1].jumpAddr = ex_mem[0].jumpAddr;

    memset(ex_mem, 0, sizeof(EX_MEM));
}
void copyMEMWB(MEM_WB* mem_wb) {
	mem_wb[1].in = mem_wb[0].in;
	mem_wb[1].cs = mem_wb[0].cs;
	mem_wb[1].readData = mem_wb[0].readData;
    mem_wb[1].aluResult = mem_wb[0].aluResult;
	mem_wb[1].writeRedister = mem_wb[0].writeRedister;
	mem_wb[1].opcode = mem_wb[0].opcode;

    memset(mem_wb, 0, sizeof(MEM_WB));
}

void F1_If(unsigned int* pc, unsigned int* inst_mem, int* stall,IF_ID* if_id) {

    printf(" [IF]\n");

    //Read the PC
    if_id[0].in = 1;

    //Fetch instruction from memory & Store the fetched instruction
    if_id[0].inst = (inst_mem[*pc] << 24) + (inst_mem[*pc + 1] << 16) + (inst_mem[*pc + 2] << 8) + inst_mem[*pc + 3];
    printf("  [IM] PC:0X%X -> 0X%X\n", *pc, if_id[0].inst);

    //Prepare the next PC
    *pc += 4;
    if_id[0].savePC = *pc;
    printf("  [PC Update] PC <- 0X%X = 0X%X+4\n", *pc, *pc - 4);
}

void F2_Id(unsigned int* pc, unsigned int* reg, int* stall, IF_ID* if_id, ID_EX* id_ex, EX_MEM* ex_mem) {

    printf(" [ID]\n");

    if (if_id->in != 1)
        return;

    id_ex[0].in = 1;
    id_ex[0].savePC = if_id[1].savePC;


    unsigned int funct = 0;
    ControlSignals* cs = (ControlSignals*)malloc(sizeof(ControlSignals));
    memset(cs, 0, sizeof(ControlSignals)); // set all control signals to 0
    id_ex[0].cs = cs;

    //a) Fetch the instruction
    //By using pointer

    //b) Decode the opcode
    id_ex[0].opcode = (if_id[1].inst & 0xFC000000) >> 26; // 0xFC000000 = 1111 1100 0000 0000 0000 0000 0000 0000


    if (id_ex[0].opcode == 0)
        funct = if_id[1].inst & 0x3F; // 0x3F = 0011 1111
    else
        funct = ~0;

    //c) Read register operands
    id_ex[0].readData1 = (if_id[1].inst & 0x03E00000) >> 21;   // 0x03E00000 = 0000 0011 1110 0000 0000 0000 0000 0000
    id_ex[0].readData2 = (if_id[1].inst & 0x001F0000) >> 16;   // 0x001F0000 = 0000 0000 0001 1111 0000 0000 0000 0000
    id_ex[0].rt = (if_id[1].inst & 0x001F0000) >> 16;   // 0x001F0000 = 0000 0000 0001 1111 0000 0000 0000 0000
    id_ex[0].rd = (if_id[1].inst & 0x0000F800) >> 11;   // 0x0000F800 = 0000 0000 0000 0000 1111 1000 0000 0000
    //d) Prepare operands
    id_ex[0].funct = funct;
    id_ex[0].shamt = (if_id[1].inst & 0x000007C0) >> 6; // 0x000007C0 = 0000 0000 0000 0000 0000 0111 1100 0000
    id_ex[0].addr = (if_id[1].inst & 0x03FFFFFF);       // 0x03FFFFFF = 0000 0011 1111 1111 1111 1111 1111 1111
    id_ex[0].imm = (if_id[1].inst & 0x0000FFFF);        // 0x0000FFFF = 0000 0000 0000 0000 1111 1111 1111 1111

    //sign extension
    unsigned int signBit = (id_ex[0].imm >> 15) & 1; // Extract the sign bit
    if (signBit == 1)
    {
        int mask = -1 << 16; // Create a mask with 1s in the higher bits
        id_ex[0].imm = id_ex[0].imm | mask; // Perform sign extension
    }

    //d) Data Hazard Detection
    printf("%d\n", *stall);
    if (*stall != 0)
    {
        printf("  [Data Hazard] Stall k\n");
        memset(&id_ex[0], 0, sizeof(ID_EX));
        id_ex[0].cs = cs;
        id_ex[0].in = 1;
        id_ex[0].cs->stall = 1;
        *stall = *stall - 1;
    }
    else
    {
        if (id_ex[0].opcode == 0 || id_ex[0].opcode >= 0X04) // R-type or I-type
        {
            if (id_ex[1].opcode == 0 || id_ex[1].opcode >= 0x04)
            {
                if ((id_ex[1].rd == id_ex[0].readData1 || id_ex[1].rd == id_ex[0].readData2) && id_ex[1].rd != 0 && id_ex[1].opcode == 0)
                {
                    printf("  [Data Hazard] Stall m1\n");
                    *stall = 1;
                    *pc -= 8;
                    memset(&id_ex[0], 0, sizeof(ID_EX));
                    id_ex[0].cs = cs;
                    id_ex[0].in = 1;
                    id_ex[0].cs->stall = 1;
                }
                
                else if ((id_ex[1].rt == id_ex[0].readData1 || id_ex[1].rt == id_ex[0].readData2) && id_ex[1].rt != 0 && id_ex[1].opcode >= 0x04)
                {
                    printf("  [Data Hazard] Stall m2\n");
                    *stall = 1;
                    *pc -= 8;
                    memset(&id_ex[0], 0, sizeof(ID_EX));
                    id_ex[0].cs = cs;
                    id_ex[0].in = 1;
                    id_ex[0].cs->stall = 1;
                }
            }
            
            if (ex_mem[1].opcode == 0 || ex_mem[1].opcode >= 0x04)
            {
                printf("%d %d %d\n", ex_mem[1].writeRedister, id_ex[0].readData1, id_ex[0].readData2);
                if ((ex_mem[1].writeRedister == id_ex[0].readData1 || ex_mem[1].writeRedister == id_ex[0].readData2) && ex_mem[1].writeRedister != 0)
                {
                    printf("  [Data Hazard] Stall e\n");
                    printf("%d\n", ex_mem[1].writeRedister);
                    *stall = 0;
                    *pc -= 4;
                    memcpy(&if_id[0], &if_id[1], sizeof(IF_ID));
                    memset(&id_ex[0], 0, sizeof(ID_EX));
                    id_ex[0].cs = cs;
                    id_ex[0].in = 1;
                    id_ex[0].cs->stall = 1;
                }
            }
        }
    }

    if ((id_ex[0].opcode == 0x04 || id_ex[0].opcode == 0x05) && *stall==0)
    {
        printf("  [Data Hazard] Stall b\n");
        *stall = 3;
        *pc -= 12;
    }
    
    if (id_ex[0].opcode == 0x02 || id_ex[0].opcode == 0x03)
    {

    }

    //e) Generate control signals
    Control_Unit(&id_ex[0], id_ex->opcode, reg);
}

void F3_Ex(unsigned int* pc,unsigned int* reg, ID_EX* id_ex, EX_MEM* ex_mem) {

    printf(" [EX]\n");
    
    if (id_ex->in != 1)
        return;
        
    ex_mem->in = 1;
    ex_mem->cs = id_ex->cs;
    ex_mem->opcode = id_ex->opcode;

    //a) ALU execution
    unsigned int data1 = reg[id_ex->readData1];
    unsigned int data2 = Mux(reg[id_ex->readData2], id_ex->imm, id_ex->cs->ALUSrc);

    //Sign Unsigned 
    if (id_ex->funct == 0X21 || id_ex->funct == 0X23 || id_ex->funct == 0X2B || id_ex->opcode == 0X09){}
    else
    {
        (int)data1;
        (int)data2;
    }

    switch (id_ex->cs->ALUOp)
    {
    case 0:
        //add
        ex_mem->aluResult = data1 + data2;
        break;

    case 1:
        //sub
        ex_mem->aluResult = data1 - data2;
        break;

    case 2:
        //and
        ex_mem->aluResult = data1 & data2;
		break;

    case 3:
        //or
        ex_mem->aluResult = data1 | data2;
        break;

    case 4:
        //nor
        ex_mem->aluResult = ~(data1 | data2);
        break;

    case 5:
        //slt
        if (data1 < data2) 
        {
            ex_mem->aluResult = 1;
        }
		else
        {
            ex_mem->aluResult = 0;
        }
        break;

    case 6:
        //shift sll
        ex_mem->aluResult = data1 << id_ex->shamt;
        break;

    case 7:
        //shift srl
        ex_mem->aluResult = data1 >> id_ex->shamt;
        break;

    case 8:
        if (data1 == data2)
        {
            ex_mem->zero = 1 ^ (1 ^ id_ex->cs->Branch);//beq 1 ? 1->1   bne 1 ? 0->0
        }
        else
        {
            ex_mem->zero = 1 ^ (0 ^ id_ex->cs->Branch);//beq 0 ? 1->0   bne 0 ? 0->1
		}
        id_ex->cs->Branch = 1;
        break;

    default:
        break;
    }

    //b) Write register
    ex_mem->writeRedister = Mux(id_ex->rt, id_ex->rd, id_ex->cs->RegDst);
    ex_mem->writeData = reg[id_ex->readData2];
    ex_mem->jumpAddr = id_ex->addr << 2 | (*pc & 0xF0000000);
}

void F4_Mem(unsigned char* mem, unsigned int* reg, unsigned int* pc, EX_MEM* ex_mem, MEM_WB* mem_wb) {
    
    printf(" [MEM]\n");
    if (ex_mem->in != 1)
		return;
    mem_wb->in = 1;
    mem_wb->cs = ex_mem->cs;
    mem_wb->opcode = ex_mem->opcode;
    mem_wb->writeRedister = ex_mem->writeRedister;
    mem_wb->aluResult = ex_mem->aluResult;

    //a) Data Alignment
    if (ex_mem->aluResult % 4 == 0) {
        //printf("Error: Memory address is not aligned\n");
        //return ;
    }

    //big endian
    //b) Load Data
    if (mem_wb->cs->MemRead == 1 && mem_wb->cs->MemWrite == 0) {
        mem_wb->readData = (mem[ex_mem->aluResult]<<24) & 0XFF000000 | (mem[ex_mem->aluResult + 1]<<16) & 0X00FF0000 | (mem[ex_mem->aluResult + 2]<<8) & 0X0000FF00 | (mem[ex_mem->aluResult + 3]<<0) & 0X000000FF;
        printf("  load r[%d] <- 0x%X\n", ex_mem->writeRedister, mem[ex_mem->aluResult + 1], mem[ex_mem->aluResult + 2], mem[ex_mem->aluResult + 3], mem_wb->readData);
    }
    //c) Store Data
    else if (mem_wb->cs->MemRead == 0 && mem_wb->cs->MemWrite == 1) {
        mem[ex_mem->aluResult] = (ex_mem->writeData & 0XFF000000) >> 24;
        mem[ex_mem->aluResult + 1] = (ex_mem->writeData & 0X00FF0000) >> 16;
        mem[ex_mem->aluResult + 2] = (ex_mem->writeData & 0X0000FF00) >> 8;
        mem[ex_mem->aluResult + 3] = (ex_mem->writeData & 0X000000FF) >> 0;
        printf("  store 0x%x -> 0x%x\n", ex_mem->writeData, ex_mem->aluResult);
    }
    else {
		printf("  None\n");
	}

    //d) Branch
    if (ex_mem->cs->Jump == 1 & ex_mem->cs->RegWrite == 1 & ex_mem->opcode != 0)//jal
    {
        reg[31] = *pc + 4;
        *pc = ex_mem->jumpAddr;
    }
    else  if (ex_mem->opcode == 0 && ex_mem->cs->Jump == 1)//jr
    {
        *pc = reg[31];
    }
    else
    {
        unsigned int res = Mux(*pc, ex_mem->savePC, ex_mem->cs->Branch & ex_mem->zero);
        *pc = Mux(res, ex_mem->jumpAddr, ex_mem->cs->Jump);
    }

    if (ex_mem->cs->stall != 0)
    {
        printf("  [Data Hazard] Stall\n");
    }
}

void F5_Wb(unsigned int* reg, MEM_WB* mem_wb) {

    printf(" [WB]\n");
    if (mem_wb->in != 1)
        return;
    mem_wb->in = 1;

    //a) register write
    if (mem_wb->cs->RegWrite == 1 & mem_wb->cs->Jump != 1)
    {
        reg[mem_wb->writeRedister] = Mux(mem_wb->aluResult, mem_wb->readData, mem_wb->cs->MemtoReg);
        if (mem_wb->cs->MemtoReg == 0)
        {
            printf("  a R[%d]<-0X%X\n", mem_wb->writeRedister, mem_wb->aluResult);
        }
        else if (mem_wb->cs->MemtoReg == 1)
        {
            printf("  r R[%d]<-0X%X\n", mem_wb->writeRedister, mem_wb->readData);
		}
    }
    else
    {
        printf("  None\n");
    }

    if (mem_wb->cs->stall != 0)
    {
        printf("  [Data Hazard] Stall\n");
    }

    free(mem_wb->cs);
}




int main() {
    int bin = 0;
    int length = 0;
    int cycle = 0;


    unsigned int* pc = (unsigned int*)malloc(1 * sizeof(unsigned int));
    unsigned int* reg = (unsigned int*)malloc(32 * sizeof(unsigned int));
    unsigned char* mem = (unsigned char*)malloc(4294967296 * sizeof(unsigned char));
    int* stall = (int*)malloc(1 * sizeof(int));


    IF_ID* if_id = (IF_ID*)malloc(sizeof(IF_ID) * 2);
    ID_EX* id_ex = (ID_EX*)malloc(sizeof(ID_EX) * 2);
    EX_MEM* ex_mem = (EX_MEM*)malloc(sizeof(EX_MEM) * 2);
    MEM_WB* mem_wb = (MEM_WB*)malloc(sizeof(MEM_WB) * 2);

    memset(if_id, 0, sizeof(IF_ID) * 2);
    memset(id_ex, 0, sizeof(ID_EX) * 2);
    memset(ex_mem, 0, sizeof(EX_MEM) * 2);
    memset(mem_wb, 0, sizeof(MEM_WB) * 2);

    FILE* file;

    // Open the binary file for reading
    if (fopen_s(&file, "input2.bin", "rb") != 0 || file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory to store the file content
    // Inst mem bigendian
    unsigned int* inst_mem = (unsigned int*)malloc(file_size * sizeof(unsigned int));
    if (inst_mem == NULL) {
        printf("Error allocating memory.\n");
        fclose(file);
        return 1;
    }

    while ((bin = fgetc(file)) != EOF)
    {
        inst_mem[length++] = bin;
    }

    //Initialize 
    *pc = 0;
    *stall = 0;
    memset(mem, 0, 4294967296 * sizeof(unsigned char));
    memset(reg, 0, 32 * sizeof(unsigned int));
    reg[31] = 0XFFFFFFFF;//ra
    reg[29] = 0X1000000;//sp

    //Start the simulation
    while (*pc < length)//*pc != 0XFFFFFFFF)
    {
        //Zero registor
        reg[0] = 0;

        printf("Cycle[%d]\n", cycle++);
        reg[0] = 0;
        F1_If(pc, inst_mem, stall, if_id);
        reg[0] = 0;
        F2_Id(pc, reg, stall, if_id, id_ex, ex_mem);
        reg[0] = 0;
        F3_Ex(pc, reg, &id_ex[1], &ex_mem[0]);
        reg[0] = 0;
        F4_Mem(mem, reg, pc, &ex_mem[1], &mem_wb[0]);
        reg[0] = 0;
        F5_Wb(reg, &mem_wb[1]);

        copyIFID(if_id);
        copyIDEX(id_ex);
        copyEXMEM(ex_mem);
        copyMEMWB(mem_wb);

        printf("=====================================\n");
    }

    printf("%d", reg[2]);

    // Close the file and free the memory
    fclose(file);
    free(inst_mem);
    free(pc);
    free(reg);
    free(mem);

    return 0;

}