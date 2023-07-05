#pragma once
#include <stdio.h>
#include "num.h"
#include "ram.h"

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#define STACK_SIZE 1024
#define STACK_START RAM_TOTAL_SIZE - STACK_SIZE

#define REGISTERS_COUNT 16

#define INTERRUPT_INFO_POINTER_REGISTER 15
#define CPU_FLAGS_REGISTER 14

#define STACK_POINTER_REGISTER 7
#define PROGRAM_COUNTER_REGISTER 6
#define ARITHMETIC_RESULT_REGISTER 5

// CPU flags
#define CPU_FLAG_INTERRUPTS_ENABLED 0
#define CPU_FLAG_USER_MODE_OUTSIDE_CANREAD 1

PACK(struct CpuState {
    u8 isInPrivilegedMode:1;
    u8 reserved:7;
});

struct InterruptInfo {
    u16 mappings[256];
};

extern CpuState cpuState;
extern InterruptInfo interruptInfo;
extern u16 registers[REGISTERS_COUNT];

void push(u16 val);
u16 pop();

bool getFlag(u8 flag);
void setFlag(u8 flag, bool val);
void setRAMBank(u8 bankIndex);

void initCpu(u8* buffer, u16 bufferSize);
void runInstruction();
void interrupt(u8 interruptId);