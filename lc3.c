#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#define MEMORY_MAX (1 << 16)
uint16_t memory[MEMORY_MAX];
enum        /*Registers*/
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};

uint16_t reg[R_COUNT];

enum        /*OpCodes*/
{
    OP_BR = 0, /* branch */     //Av
    OP_ADD,    /* add  */       //Av
    OP_LD,     /* load */  
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */        //Av
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */        //Av
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

enum        /*Condition Flags*/
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}



/* WRITE ALL FUNCTIONS HERE */
void ADD();
void AND();
void NOT();
void BR();
void JMP();
void JSR();
void LD();
void LDR();
void LEA();
void ST();
void STI();
void STR();
void BAD_OPCODE();
void Shutdown();


int main(int argc, const char* argv[])

{
    if (argc<2)
    {
        /* This line is to show usage string */
        printf("lc3 [image-file1]...\n");
        exit(2);
    }

    for(int j=1;j<argc;++j)
    {
        if(!read_image(argv[j]))
        {
            printf("Failed to load image: %s\n",argv[j]);
            exit(1);
        }
    }
    
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();
    /* since exactly one condition flag should be set at any given time, set the Z flag */
    reg[R_COND] = FL_ZRO;
    /* set the PC to starting position */
    /* 0x3000 is the default */
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    int running=1;
    while (running)
    {
        //Fetch
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >>12;
        switch(op)
        {
            case OP_ADD:
                ADD();
                break;
            case OP_AND:
                AND();
                break;
            case OP_NOT:
                NOT();
                break;
            case OP_BR:
                BR();
                break;
            case OP_JMP:
                JMP();
                break;
            case OP_JSR:
                JSR();
                break;
            case OP_LD:
                LD();
                break;
            case OP_LDI:
                LDI();
                break;
            case OP_LDR:
                LDR();
                break;
            case OP_LEA:
                LEA();
                break;
            case OP_ST:
                ST();
                break;
            case OP_STI:
                STI();
                break;
            case OP_STR:
                STR();
                break;
            case OP_TRAP:
                TRAP();
                break;
            case OP_RES:
            case OP_RTI:
            default:
                BAD_OPCODE();
                break;
        }
    }

    Shutdown();
}

void ADD()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0x7;
    /* first operand (SR1) */
    uint16_t r1 = (instr >> 6) & 0x7;
    /* whether we are in immediate mode */
    uint16_t imm_flag = (instr >> 5) & 0x1;

    if (imm_flag)
    {
        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
        reg[r0] = reg[r1] + imm5;
    }
    else
    {
        uint16_t r2 = instr & 0x7;
        reg[r0] = reg[r1] + reg[r2];
    }

    update_flags(r0);
}

void AND()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t imm_flag = (instr >> 5) & 0x1;

    if (imm_flag)
    {
        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
        reg[r0] = reg[r1] & imm5;
    }
    else
    {
        uint16_t r2 = instr & 0x7;
        reg[r0] = reg[r1] & reg[r2];
    }
    update_flags(r0);
}

void NOT()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;

    reg[r0] = ~reg[r1];
    update_flags(r0);
}

void BR()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    uint16_t cond_flag = (instr >> 9) & 0x7;
    if (cond_flag & reg[R_COND])
    {
        reg[R_PC] += pc_offset;
    }
}

void JMP()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    /* Also handles RET */
    uint16_t r1 = (instr >> 6) & 0x7;
    reg[R_PC] = reg[r1];
}

void JSR()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t long_flag = (instr >> 11) & 1;
    reg[R_R7] = reg[R_PC];
    if (long_flag)
    {
        uint16_t long_pc_offset = sign_extend(instr & 0x7FF, 11);
        reg[R_PC] += long_pc_offset;  /* JSR */
    }
    else
    {
        uint16_t r1 = (instr >> 6) & 0x7;
        reg[R_PC] = reg[r1]; /* JSRR */
    }
}

void LD()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    reg[r0] = mem_read(reg[R_PC] + pc_offset);
    update_flags(r0);
}

void LDR()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t offset = sign_extend(instr & 0x3F, 6);
    reg[r0] = mem_read(reg[r1] + offset);
    update_flags(r0);
}

void LEA()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    reg[r0] = reg[R_PC] + pc_offset;
    update_flags(r0);
}

void ST()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    mem_write(reg[R_PC] + pc_offset, reg[r0]);
}

void STI()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
    mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
}

void STR()
{
    uint16_t instr = mem_read(reg[R_PC]++);
    uint16_t op = instr >>12;
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t offset = sign_extend(instr & 0x3F, 6);
    mem_write(reg[r1] + offset, reg[r0]);
}

void BAD_OPCODE()
{
    abort();
}

void Shutdown()
{
    restore_input_buffering();
}
