#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Control Signals
typedef struct {
    unsigned int RegDst;
    unsigned int ALUSrc;
    unsigned int MemtoReg;
    unsigned int RegWrite;
    unsigned int MemRead;
    unsigned int MemWrite;
    unsigned int Branch;
    unsigned int Bcond;
    unsigned int ALUOp;
    unsigned int Jump;
} ControlSignals;

// Operands
typedef struct {
    unsigned int opcode;
	unsigned int rs;
	unsigned int rt;
	unsigned int rd;
	unsigned int shamt;
	unsigned int funct;
	unsigned int imm;
	unsigned int addr;
} Operands;

unsigned int Mux(unsigned int a, unsigned int b, unsigned int cs) //0,1
{
    if(cs==0)
		return a;
	else
		return b;
}

void control_unit(Operands* operands, ControlSignals* cs, unsigned int* pc)
{
    switch (operands->opcode)
    {

    case 0X02:
        printf("j addr:%X\n", operands->addr << 2 | (*pc & 0xF0000000));
        break;

    case 0X03:
        printf("jal addr:%X\n", operands->addr << 2 | (*pc & 0xF0000000));
        break;

    case 0X04:
        printf("beq rs:%d rt:%d imm:%X\n", operands->rs, operands->rt, operands->imm);//b
        break;

    case 0x05:
        printf("bne rs:%d rt:%d imm:%X\n", operands->rs, operands->rt, operands->imm);//bnez
        break;

    case 0X09:
        printf("addiu rs:%d rt:%d imm:%d\n", operands->rs, operands->rt, operands->imm);//li
        cs->ALUOp = 0;
        break;

    case 0X0A:
        printf("slti rs:%d rt:%d imm:%X\n", operands->rs, operands->rt, operands->imm);
        cs->ALUOp = 5;
        break;

    case 0X23:
        printf("lw rs:%d rt:%d imm:%d\n", operands->rs, operands->rt, operands->imm);
        cs->ALUOp = 0;
        break;

    case 0X2B:
        printf("sw rs:%d rt:%d imm:%d\n", operands->rs, operands->rt, operands->imm);
        cs->ALUOp = 0;
        break;

    case 0:
        switch (operands->funct)
        {
        case 0X00:
            printf("sll");//nop
            cs->ALUOp = 6;
            break;

        case 0X02:
            printf("srl");
            cs->ALUOp = 7;
            break;

        case 0X08:
            printf("jr");
            break;

        case 0X20:
            printf("add");
            cs->ALUOp = 0;
            break;

        case 0X21:
            printf("addu");
            cs->ALUOp = 0;
            break;

        case 0X22:
            printf("sub");
            cs->ALUOp = 1;
            break;

        case 0X23:
            printf("subu");
            cs->ALUOp = 1;
            break;

        case 0X24:
            printf("and");
            cs->ALUOp = 2;
            break;

        case 0X25:
            printf("or");//move
            cs->ALUOp = 3;
            break;

        case 0X27:
            printf("nor");
            cs->ALUOp = 4;
            break;

        case 0X2A:
            printf("slt");
            cs->ALUOp = 5;
            break;

        case 0X2B:
            printf("sltu");
            cs->ALUOp = 5;
            break;

        default:
            printf("error");
            break;
        }
        printf("rs: %d  rt: %d  rd: %d  shamt: %d  funct: %d\n", operands->rs, operands->rt, operands->rd, operands->shamt, operands->funct);

    default:
        break;
    }

    unsigned int op0 = 0, op1 = 0, op2 = 0, op3 = 0, op4 = 0, op5 = 0;
    unsigned int rformat = 0, lw = 0, sw = 0, beq = 0, jump = 0;

    if (operands->opcode == 0 | operands->opcode == 0X23 | operands->opcode == 0X2B | operands->opcode == 0X04 | operands->opcode == 0X05 | operands->opcode == 0X02)
    {
        op0 = operands->opcode & 0x1; // 0x1 = 0000 0001
        op1 = (operands->opcode & 0x2) >> 1; // 0x1 = 0000 0010
        op2 = (operands->opcode & 0x4) >> 2; // 0x1 = 0000 0100
        op3 = (operands->opcode & 0x8) >> 3; // 0x1 = 0000 1000
        op4 = (operands->opcode & 0x10) >> 4; // 0x1 = 0001 0000
        op5 = (operands->opcode & 0x20) >> 5; // 0x1 = 0010 0000

        rformat = ((~op0) & 0x1) & ((~op1) & 0x1) & ((~op2) & 0x1) & ((~op3) & 0x1) & ((~op4) & 0x1) & ((~op5) & 0x1);
        lw = op0 & op1 & ((~op2) & 0x1) & ((~op3) & 0x1) & ((~op4) & 0x1) & op5;
        sw = op0 & op1 & ((~op2) & 0x1) & op3 & ((~op4) & 0x1) & op5;
        beq = ((~op0) & 0x1) & ((~op1) & 0x1) & op2 & ((~op3) & 0x1) & ((~op4) & 0x1) & ((~op5) & 0x1);
        jump = ((~op0) & 0x1) & ((~op1) & 0x1) & ((~op2) & 0x1) & ((~op3) & 0x1) & op4 & ((~op5) & 0x1);

        cs->RegDst = rformat;
        cs->ALUSrc = lw | sw;
        cs->MemtoReg = lw;
        cs->RegWrite = rformat | lw;
        cs->MemRead = lw;
        cs->MemWrite = sw;
        cs->Branch = beq;
        cs->Jump = jump;

        if (operands->funct == 0X08)
        {
            cs->RegDst = 0;
            cs->ALUSrc = 0;
            cs->MemtoReg = 0;
            cs->RegWrite = 0;
            cs->MemRead = 0;
            cs->MemWrite = 0;
            cs->Branch = 0;
            cs->Jump = 1;
        }
    }
    else if (operands->opcode == 0X09 | operands->opcode == 0X0A)
    {
        cs->RegDst = 0;
        cs->ALUSrc = 1;
        cs->MemtoReg = 0;
        cs->RegWrite = 1;
        cs->MemRead = 0;
        cs->MemWrite = 0;
        cs->Branch = 0;
        cs->Jump = 0;

    }
    else if (operands->opcode == 0X0A)
    {
        cs->RegDst = 0;
        cs->ALUSrc = 1;
        cs->MemtoReg = 0;
        cs->RegWrite = 0;
        cs->MemRead = 0;
        cs->MemWrite = 0;
        cs->Branch = 0;
        cs->Jump = 0;
    }
    else if (operands->opcode == 0X03)
    {
        cs->RegDst = 1;
        cs->RegWrite = 0;
        cs->Jump = 1;
    }
}

void F1_If(unsigned int* pc, unsigned int* inst_mem, unsigned int* inst) {

    //Read the PC

    //Fetch instruction from memory & Store the fetched instruction
    *inst = 0;
    *inst = (inst_mem[*pc] << 24) + (inst_mem[*pc + 1] << 16) + (inst_mem[*pc + 2] << 8) + inst_mem[*pc + 3];
    //Prepare the next PC
    *pc += 4;
   
}


void F2_Id(unsigned int* inst, ControlSignals* cs, Operands* operands, unsigned int* pc) {

    unsigned int funct = 0;
    unsigned int opcode = 0;
    memset(cs, 0, sizeof(ControlSignals)); // set all control signals to 0

    //a) Fetch the instruction
    //By using pointer

    //b) Decode the opcode
    opcode = (*inst & 0xFC000000) >> 26; // 0xFC000000 = 1111 1100 0000 0000 0000 0000 0000 0000
    operands->opcode= opcode;

    if (opcode == 0)
        funct = *inst & 0x3F; // 0x3F = 0011 1111
    else
        funct = ~0;

    //c) Read register operands
    operands->rs = (*inst & 0x03E00000) >> 21;   // 0x03E00000 = 0000 0011 1110 0000 0000 0000 0000 0000
    operands->rt = (*inst & 0x001F0000) >> 16;   // 0x001F0000 = 0000 0000 0001 1111 0000 0000 0000 0000
    operands->rd = (*inst & 0x0000F800) >> 11;   // 0x0000F800 = 0000 0000 0000 0000 1111 1000 0000 0000

    //d) Prepare operands
    operands->funct = (*inst & 0x0000003F);      // 0x0000003F = 0000 0000 0000 0000 0000 0000 0011 1111
    operands->shamt = (*inst & 0x000007C0) >> 6; // 0x000007C0 = 0000 0000 0000 0000 0000 0111 1100 0000
    operands->addr = (*inst & 0x03FFFFFF);       // 0x03FFFFFF = 0000 0011 1111 1111 1111 1111 1111 1111
    operands->imm = (*inst & 0x0000FFFF);        // 0x0000FFFF = 0000 0000 0000 0000 1111 1111 1111 1111

    //sign extension
    unsigned int signBit = (operands->imm >> 15) & 1; // Extract the sign bit
    if (signBit == 1)
    {
        int mask = -1 << 16; // Create a mask with 1s in the higher bits
        operands->imm = operands->imm | mask; // Perform sign extension
    }

    //e) Generate control signals
    control_unit(operands, cs, pc);
}


void F3_Ex(unsigned int* pc, unsigned int* alu_result,unsigned int* reg, ControlSignals* cs, Operands* operands) {

    //a) Calculation
    unsigned int data1 = reg[operands->rs];
    unsigned int data2 = Mux(reg[operands->rt], operands->imm, cs->ALUSrc);

    //Sign Unsigned 
    if (operands->funct == 0X21 | operands->funct == 0X23 | operands->funct == 0X2B | operands->opcode == 0X09){}
    else
    {
        (int)data1;
        (int)data2;
    }

    switch (cs->ALUOp)
    {
    case 0:
        //add
        *alu_result = data1 + data2;
        break;

    case 1:
        //sub
        *alu_result = data1 - data2;
        break;

    case 2:
        //and
		*alu_result = data1 & data2;
		break;

    case 3:
        //or
        *alu_result = data1 | data2;
        break;

    case 4:
        //nor
        *alu_result = ~(data1 | data2);
        break;

    case 5:
        //slt
        if (data1 < data2)
            *alu_result = 1;
		else
			*alu_result = 0;
        break;

    case 6:
        //shift sll
        *alu_result = data1 << operands->shamt;
        break;

    case 7:
        //shift srl
        *alu_result = data1 >> operands->shamt;
        break;

    default:
        break;
    }
    
    printf("data1: %x     data2:%x\n", data1, data2);
    printf("ALU Result: %x\n", *alu_result);

    //b) Branch Evaluation
    switch (operands->opcode)
    {

    case 0X04:
        if (reg[operands->rs] == reg[operands->rt])
            cs->Bcond = 1;
        else
            cs->Bcond = 0;
        break;

    case 0X05:
        cs->Branch = 1;
        if (reg[operands->rs] != reg[operands->rt])
            cs->Bcond = 1;
        else
            cs->Bcond = 0;
        break;

    default:
        cs->Bcond = 0;
        break;
    }
    //printf("%X %X\n", reg[operands->rs], reg[operands->rt]);
    //printf("bcond: %x branch:%d\n", cs->Bcond, cs->Branch);

}


void F4_Mem(ControlSignals* cs, Operands* operands,unsigned int* read_data, unsigned char* mem, unsigned int* alu_result, unsigned int* reg) {
    
    //a) Data Alignment
    if (*alu_result % 4 != 0) {
        //printf("Error: Memory address is not aligned\n");
        return ;
    }
  
    
    //big endian
    //b) Load Data
    if (cs->MemRead == 1 && cs->MemWrite == 0) {
        *read_data = (mem[*alu_result] << 24) & 0XFF000000 | (mem[*alu_result + 1] << 16) & 0X00FF0000 | (mem[*alu_result + 2] << 8) & 0X0000FF00 | (mem[*alu_result + 3] << 0) & 0X000000FF;
        printf("addr: %X, data: %X %X %X %X, load data: %X\n", *alu_result, mem[*alu_result], mem[*alu_result + 1], mem[*alu_result + 2], mem[*alu_result + 3], *read_data);
    }

    //c) Store Data
    else if (cs->MemRead == 0 && cs->MemWrite == 1) {
        mem[*alu_result] = (reg[operands->rt] & 0XFF000000) >> 24;
        mem[*alu_result + 1] = (reg[operands->rt] & 0X00FF0000) >> 16;
        mem[*alu_result + 2] = (reg[operands->rt] & 0X0000FF00) >> 8;
        mem[*alu_result + 3] = (reg[operands->rt] & 0X000000FF) >> 0;
        printf("addr: %X, data: %X %X %X %X, stored data: %X\n", *alu_result, mem[*alu_result], mem[*alu_result + 1], mem[*alu_result + 2], mem[*alu_result + 3], reg[operands->rt]);
    }

    else if (cs->MemRead == 0 && cs->MemWrite == 0) {
		//printf("No memory access\n");
	}
    else {
		//printf("Error: MemRead and MemWrite cannot be 1 at the same time\n");
	}

}


void F5_Wb(ControlSignals* cs, Operands* operands, unsigned int* pc, unsigned int* read_data, unsigned int* alu_result, unsigned int* reg) {

    //a) register write
    if (operands->opcode == 0 | operands->opcode == 0X09 | operands->opcode == 0X0A | operands->opcode == 0X23) {
        unsigned int write_data = Mux(*alu_result, *read_data, cs->MemtoReg);
        reg[Mux(operands->rt, operands->rd, cs->RegDst)] = write_data;
        printf("reg[%d] write data: %X\n", Mux(operands->rt, operands->rd, cs->RegDst), write_data);
    }

    //b) Update program counter
    if (operands->opcode == 0X03)//jal
    {
        reg[31] = *pc + 4;
        *pc = operands->addr << 2 | (*pc & 0xF0000000);
    }
    else  if (operands->opcode == 0 && operands->funct == 0X08)//jr
    {
        *pc = reg[operands->rs];
        return;
    }
    else
    {
        unsigned int res = Mux(*pc, ((operands->imm << 2) + *pc), cs->Bcond & cs->Branch);
        *pc = Mux(res, operands->addr << 2 | (*pc & 0xF0000000), cs->Jump);
    }
    printf("Next Pc: %X\n", *pc);
}




int main() {
    int bin = 0;
    int length = 0;
    int cycle = 0;

    unsigned int* inst = (unsigned int*)malloc(1 * sizeof(unsigned int));
    unsigned int* pc = (unsigned int*)malloc(1 * sizeof(unsigned int));
    unsigned int* reg = (unsigned int*)malloc(32 * sizeof(unsigned int));
    unsigned char* mem = (unsigned char*)malloc(4294967296 * sizeof(unsigned char));
    unsigned int* alu_result = (unsigned int*)malloc(1 * sizeof(unsigned int));
    unsigned int* read_data = (unsigned int*)malloc(1 * sizeof(unsigned int));

    ControlSignals* cs = (ControlSignals*)malloc(sizeof(ControlSignals));
    Operands* operands = (Operands*)malloc(sizeof(Operands));
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
    *inst = 0;
    *pc = 0;
    memset(mem, 0, 4294967296 * sizeof(unsigned char));
    memset(reg, 0, 32 * sizeof(unsigned int));
    reg[31] = 0XFFFFFFFF;//ra
    reg[29] = 0X1000000;//sp

    //Start the simulation
    while (*pc < length | *pc != 0XFFFFFFFF)
    {
        //Zero registor
        reg[0] = 0;

        printf("%d\n", cycle++);
        printf("pc: %X\t", *pc);
        F1_If(pc, inst_mem, inst);
        printf("inst: %x\n", *inst);
        F2_Id(inst, cs, operands, pc);
        F3_Ex(pc, alu_result, reg, cs, operands);
        F4_Mem(cs, operands, read_data, mem, alu_result, reg);
        F5_Wb(cs, operands, pc, read_data, alu_result, reg);
        printf("=====================================\n");
    }

    // Close the file and free the memory
    fclose(file);
    free(inst_mem);
    free(inst);
    free(pc);
    free(reg);
    free(mem);
    free(alu_result);
    free(read_data);
    free(operands);
    free(cs);

    return 0;
}