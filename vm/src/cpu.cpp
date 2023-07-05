#include "cpu.h"

CpuState cpuState;
InterruptInfo interruptInfo;
u16 registers[REGISTERS_COUNT];

//#define DEBUG

#ifdef DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif

void initCpu(u8* buffer, u16 bufferSize) {
    // set stack pointer to the end of the stack
    registers[STACK_POINTER_REGISTER] = STACK_START;

    // set program counter to the start of the program
    registers[PROGRAM_COUNTER_REGISTER] = 0;

    // set privileged mode flag to true
    cpuState.isInPrivilegedMode = true;

    // set default flags
    setFlag(CPU_FLAG_INTERRUPTS_ENABLED, true);
    setFlag(CPU_FLAG_USER_MODE_OUTSIDE_CANREAD, true);

    // select first ram bank
    setRAMBank(0);

    // copy program into ram
    for(u64 i = 0; i < bufferSize; i++) {
        writeByte(i, buffer[i]);
    }
}

void callInterruptHandler(u16 handler) {
    if(getFlag(CPU_FLAG_INTERRUPTS_ENABLED)) {
        push(registers[PROGRAM_COUNTER_REGISTER]);
        registers[PROGRAM_COUNTER_REGISTER] = handler;

        // Interrupts clear the privileged mode flag
        cpuState.isInPrivilegedMode = true;
    }
}

bool getFlag(u8 flag) {
    return (registers[CPU_FLAGS_REGISTER] >> flag) & 1;
}

void setFlag(u8 flag, bool val) {
    if(val) {
        registers[CPU_FLAGS_REGISTER] |= 1 << flag;
    }else {
        registers[CPU_FLAGS_REGISTER] &= ~(1 << flag);
    }
}

void setRAMBank(u8 bankIndex) {
    ramBank = ramBanks[bankIndex];
    ramBankIndex = bankIndex;
}

void runCpu() {
    while(registers[PROGRAM_COUNTER_REGISTER] < RAM_TOTAL_SIZE - 8) {
        runInstruction();
    }
}

u16 pop() {
    registers[STACK_POINTER_REGISTER] -= 2;
    return readWord(registers[STACK_POINTER_REGISTER]);
}

void push(u16 val) {
    writeWord(registers[STACK_POINTER_REGISTER], val);
    registers[STACK_POINTER_REGISTER] += 2;
}

void runInstruction() {
    const char* instructionNameTable[256] = {
        "NOP", "PUSHIM", "PUSH", "POP", "POP", "MOV", "ADDIM", "ADD", "SUBIM", "SUB", "MULIM", "MUL", "DIVIM", "DIV", "MOD", "AND", "OR", "XOR", "NOT", "SHL", "SHR", "JMPIM", "JMP", "JEQ", "JNE", "JGT", "JLT", "JGE", "JLE", "DUP", "SWAP", "SETIIP", "INT", "RET", "CALL", "HALT", "USER", "LD", "MOVRD", "MOVRS", "SELBANK", "GETBANK"
    };

    instructionNameTable[0x7F] = "PRINT";


    u16 instPc = registers[PROGRAM_COUNTER_REGISTER];

    u16 inst = readWord(instPc);
    u16 operand1 = readWord(instPc + 2);
    u16 operand2 = readWord(instPc + 4);
    u16 operand3 = readWord(instPc + 6);

    u16 opcode = inst;

    if(opcode != 0) {
        dprintf("inst: %04X, instname: %s, opcode: %04X, operand1: %04X, operand2: %04X, operand3: %04X\n", inst, instructionNameTable[opcode], opcode, operand1, operand2, operand3);
    }

    if(opcode == 0) {
        // NOP
        registers[PROGRAM_COUNTER_REGISTER] += 4;
    }else if(opcode == 1) {
        // PUSHIM operand1
        writeWord(registers[STACK_POINTER_REGISTER], operand1);
        registers[STACK_POINTER_REGISTER] += 2;
        registers[PROGRAM_COUNTER_REGISTER] += 4;
    }else if(opcode == 2) {
        // PUSH operand1
        writeWord(registers[STACK_POINTER_REGISTER], readWord(operand1));
        registers[STACK_POINTER_REGISTER] += 2;
        registers[PROGRAM_COUNTER_REGISTER] += 4;
    }else if(opcode == 3) {
        // POPINTO operand1
        registers[STACK_POINTER_REGISTER] -= 2;
        writeWord(operand1, readWord(registers[STACK_POINTER_REGISTER]));
        registers[PROGRAM_COUNTER_REGISTER] += 4;
    }else if(opcode == 4) {
        // POP
        registers[STACK_POINTER_REGISTER] -= 2;
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 5) {
        // MOV operand1, operand2
        writeWord(operand1, readWord(operand2));
        registers[PROGRAM_COUNTER_REGISTER] += 6;
    }else if(opcode == 6) {
        // ADDIM operand1, operand2
        registers[ARITHMETIC_RESULT_REGISTER] = operand1 + operand2;
        registers[PROGRAM_COUNTER_REGISTER] += 6;
    }else if(opcode == 7) {
        // ADD
        u16 val1 = pop();
        u16 val2 = pop();

        dprintf("running add: val1: %04X, val2: %04X\n", val1, val2);
        push(val1 + val2);
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 8) {
        // SUBIM operand1, operand2
        registers[ARITHMETIC_RESULT_REGISTER] = operand1 - operand2;
        registers[PROGRAM_COUNTER_REGISTER] += 6;
    }else if(opcode == 9) {
        // SUB
        push(pop() - pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 10) {
        // MULIM operand1, operand2
        registers[ARITHMETIC_RESULT_REGISTER] = operand1 * operand2;
        registers[PROGRAM_COUNTER_REGISTER] += 6;
    }else if(opcode == 11) {
        // MUL
        push(pop() * pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 12) {
        // DIVIM operand1, operand2
        registers[ARITHMETIC_RESULT_REGISTER] = operand1 / operand2;
        registers[PROGRAM_COUNTER_REGISTER] += 6;
    }else if(opcode == 13) {
        // DIV
        push(pop() / pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 14) {
        // MOD
        push(pop() % pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 15) {
        // AND
        push(pop() & pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 16) {
        // OR
        push(pop() | pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 17) {
        // XOR
        push(pop() ^ pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 18) {
        // NOT
        push(~pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 19) {
        // SHL
        push(pop() << pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 20) {
        // SHR
        push(pop() >> pop());
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 21) {
        // JMPIM operand1
        dprintf("Jumping to %04X\n", operand1);
        registers[PROGRAM_COUNTER_REGISTER] = operand1;
    }else if(opcode == 22) {
        // JMP
        registers[PROGRAM_COUNTER_REGISTER] = pop();
    }else if(opcode == 23) {
        // JEQ operand1
        u16 val1 = pop();
        u16 val2 = pop();

        if(val1 == val2) {
            registers[PROGRAM_COUNTER_REGISTER] = operand1;
        }else {
            dprintf("not equal: val1: %04X, val2: %04X\n", val1, val2);
            registers[PROGRAM_COUNTER_REGISTER] += 4;
        }
    }else if(opcode == 24) {
        // JNE operand1
        if(pop() != pop()) {
            registers[PROGRAM_COUNTER_REGISTER] = operand1;
        }else {
            registers[PROGRAM_COUNTER_REGISTER] += 4;
        }
    }else if(opcode == 25) {
        // JGT operand1
        if(pop() > pop()) {
            registers[PROGRAM_COUNTER_REGISTER] = operand1;
        }else {
            registers[PROGRAM_COUNTER_REGISTER] += 4;
        }
    }else if(opcode == 26) {
        // JLT operand1
        if(pop() < pop()) {
            registers[PROGRAM_COUNTER_REGISTER] = operand1;
        }else {
            registers[PROGRAM_COUNTER_REGISTER] += 4;
        }
    }else if(opcode == 27) {
        // JGE operand1
        if(pop() >= pop()) {
            registers[PROGRAM_COUNTER_REGISTER] = operand1;
        }else {
            registers[PROGRAM_COUNTER_REGISTER] += 4;
        }
    }else if(opcode == 28) {
        // JLE operand1
        if(pop() <= pop()) {
            registers[PROGRAM_COUNTER_REGISTER] = operand1;
        }else {
            registers[PROGRAM_COUNTER_REGISTER] += 4;
        }
    }else if(opcode == 29) {
        // DUP
        u16 val = pop();
        push(val);
        push(val);
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 30) {
        // SWAP
        u16 val1 = pop();
        u16 val2 = pop();
        push(val1);
        push(val2);
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 31) {
        // SETIIP operand1
        if(!cpuState.isInPrivilegedMode) {
            // TODO: raise cpu exception
            return;
        }

        registers[INTERRUPT_INFO_POINTER_REGISTER] = operand1;
        interruptInfo = *(InterruptInfo*)getRealPointer(registers[INTERRUPT_INFO_POINTER_REGISTER]);
        registers[PROGRAM_COUNTER_REGISTER] += 4;
    }else if(opcode == 32) {
        // INT operand1
        u8 interruptId = operand1 >> 8;

        u16 interruptHandler = interruptInfo.mappings[interruptId];
        if(interruptHandler != 0) {
            callInterruptHandler(interruptHandler);
        }else {
            registers[PROGRAM_COUNTER_REGISTER] += 4;
        }
    }else if(opcode == 33) {
        // RET
        registers[PROGRAM_COUNTER_REGISTER] = pop();
    }else if(opcode == 34) {
        // CALL
        push(registers[PROGRAM_COUNTER_REGISTER]);
        registers[PROGRAM_COUNTER_REGISTER] = pop();
    }else if(opcode == 35) {
        // HALT
        registers[PROGRAM_COUNTER_REGISTER] = RAM_TOTAL_SIZE - 1;
    }else if(opcode == 36) {
        // USER
        cpuState.isInPrivilegedMode = false;
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 0x7F) {
        // TEMP: Prints the ascii character of the value on top of the stack
        u8 val = pop();
        //printf("Printing Reached: numeric: %d, ascii: ", val);
        printf("%c", val);
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 37) {
        // LD
        u16 addr = pop();

        dprintf("LD: addr: %04X, val: %04X\n", addr, readWord(addr));
        push(readWord(addr));
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 38) {
        // MOVRD operand1, operand2
        registers[operand1] = readWord(operand2);
        registers[PROGRAM_COUNTER_REGISTER] += 6;
    }else if(opcode == 40) {
        // MOVRS operand1, operand2
        writeWord(operand1, registers[operand2]);
        registers[PROGRAM_COUNTER_REGISTER] += 6;
    }else if(opcode == 41) {
        // SELBANK operand1
        setRAMBank(operand1);
        registers[PROGRAM_COUNTER_REGISTER] += 4;
    }else if(opcode == 42) {
        // GETBANK
        push(ramBankIndex);
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }else if(opcode == 43) {
        // LD8
        u16 addr = pop();
        push(readByte(addr));
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }
    else {
        printf("Unknown opcode: %04X\n", opcode);
        registers[PROGRAM_COUNTER_REGISTER] += 2;
    }
}