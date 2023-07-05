#pragma once
#include <stdio.h>
#include "num.h"
#include "cpu.h"

// lowSpag2 has 64k of RAM
#define RAM_TOTAL_SIZE 65536

// Swappanle RAM bank area starts at 24k and is 32k in size
#define RAM_BANK_AREA_SIZE 32768
#define RAM_BANK_AREA_START 24576
#define RAM_BANK_AREA_END RAM_BANK_AREA_START + RAM_BANK_AREA_SIZE
#define NUM_RAM_BANKS 8

// RAM is a 64k block of memory
extern u8 ram[RAM_TOTAL_SIZE];

// Current RAM bank
extern u8* ramBank;
// Current RAM bank index
extern u8 ramBankIndex;
// RAM banks
extern u8 ramBanks[NUM_RAM_BANKS][RAM_BANK_AREA_SIZE];

u8 readByte(u16 addr);
u16 readWord(u16 addr);

u8* getRealPointer(u16 addr);

void writeByte(u16 addr, u8 val);
void writeWord(u16 addr, u16 val);

void dumpRam();