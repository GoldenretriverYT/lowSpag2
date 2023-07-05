#include "ram.h"

u8 ram[RAM_TOTAL_SIZE] = {0};
u8* ramBank;
u8 ramBankIndex;
u8 ramBanks[NUM_RAM_BANKS][RAM_BANK_AREA_SIZE];

u8 readByte(u16 addr) {
    if((addr < RAM_BANK_AREA_START || addr >= RAM_BANK_AREA_END)) {
        // If flag USER_MODE_OUTSIDE_CANREAD is not set, we can only read from the ram bank area
        if(!cpuState.isInPrivilegedMode && !getFlag(CPU_FLAG_USER_MODE_OUTSIDE_CANREAD)) {
            // TODO: Raise Cpu Exception ER_PROT_VIOLATION
            return 0;
        }

        return ram[addr];
    }else {
        return ramBank[addr - RAM_BANK_AREA_START];
    }
}

u16 readWord(u16 addr) {
    return (readByte(addr + 1) << 8) | readByte(addr);
}

u8* getRealPointer(u16 addr) {
    return &ram[addr];
}

void writeByte(u16 addr, u8 val) {
    // If we are in user mode, we can only write to the ram bank area
    if((addr < RAM_BANK_AREA_START || addr >= RAM_BANK_AREA_END)) {
        if(!cpuState.isInPrivilegedMode) {
            // TODO: Raise Cpu Exception ER_PROT_VIOLATION
            return;
        }

        ram[addr] = val;
    }else {
        ramBank[addr - RAM_BANK_AREA_START] = val;
    }
}

void writeWord(u16 addr, u16 val) {
    writeByte(addr, val & 0xFF);
    writeByte(addr + 1, val >> 8);
}

void dumpRam() {
    for (int i = 0; i < RAM_TOTAL_SIZE; i++) {
        printf("%02X ", ram[i]);
    }
    printf("\n");
}