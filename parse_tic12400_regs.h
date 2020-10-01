#ifndef PARSE_TIC12400_REGS_H
#define PARSE_TIC12400_REGS_H

#include "custom_type.h"
#include "parse_regs.h"
#include "tic12400_bits_offsets.h"

#include <stdio.h>

typedef struct xReg32bit_t {
    uint8_t reg8BitAddr;
    uint32_t reg24BitVal;
    char *regName;
    bool valid;
} Reg32bit_t;

extern Reg32bit_t tic12400RegMap[REG_ADDR_NUM];

typedef enum { PULL_UP = 0, PULL_DOWN = 1 } pullMode_t;

typedef struct xTic12400Channel_t {
    uint8_t channel;
    bool state;
    float wettingCurrent;
    float threshold;
    uint8_t pullState;
    uint16_t valueThreshold;
    uint16_t valueVoltage;
    float comparatorThreshold;
    char interruptGeneration[10];
    char interruptMode;
    char inputADCLevel[100];
    char pullMode;
    char inMode[10];
} Tic12400Channel_t;

extern Tic12400Channel_t tic12400channelList[CHANNEL_AMOUNT];
extern uint8_t procRegCnd;

bool parse_tic12400_regs_file (char *inFileName, char *outFileName);
bool parse_tic12400_reg (uint8_t regAddr, FILE *outFilePrt);
bool load_tic12400_reg (uint8_t regAddr, uint32_t regVal);


#endif /* PARSE_TIC12400_REGS_H */
