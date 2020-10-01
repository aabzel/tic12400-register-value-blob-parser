#include "parse_tic12400_regs.h"

#include "str_ops.h"
#include "tic12400_bits_offsets.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Tic12400Channel_t tic12400channelList[CHANNEL_AMOUNT];
Reg32bit_t tic12400RegMap[REG_ADDR_NUM];
float g_one_bit_voltage = 0.0;
uint8_t procRegCnd = 0;


static bool write_channel_table (FILE *out_file_prt) {
    bool res = false;
    uint8_t channel;
    // collomn_t tableHead[7][2]={
    //    {"ch",4},{"wc",5},{"mode",6},{"pull",4},{"En",4},{"ctrsh",7},{"trsh",7}
    //};
    if (NULL != out_file_prt) {
        res = true;
        fprintf (out_file_prt, " \n\n");
        fprintf (out_file_prt, "+----+-----+------+------+----+-------+-------+---------+---------+---------+\n");
        fprintf (out_file_prt, "| ch | wc  | Mode | pull | En | ctrsh | tresh | intMode | voltage | inLevel |\n");
        fprintf (out_file_prt, "+----+-----+------+------+----+-------+-------+---------+---------+---------+\n");
        for (channel = 0; channel < CHANNEL_AMOUNT; channel++) {
            fprintf (out_file_prt,
                     "| %02u | %2.1f |  %s |  %c   |  %u |  %2.1fV |  %3.1fV |    %c    |   %3.1fV  | %6s  |\n",
                     channel, tic12400channelList[channel].wettingCurrent, tic12400channelList[channel].inMode,
                     tic12400channelList[channel].pullMode, tic12400channelList[channel].state,
                     tic12400channelList[channel].comparatorThreshold,
                     ((float)tic12400channelList[channel].valueThreshold) * g_one_bit_voltage,
                     tic12400channelList[channel].interruptMode,
                     ((float)tic12400channelList[channel].valueVoltage) * g_one_bit_voltage,
                     tic12400channelList[channel].inputADCLevel);
        }
        fprintf (out_file_prt, "+----+-----+------+------+----+-------+-------+---------+---------+---------+\n");
        fprintf (out_file_prt, " \n");
    }
    return res;
}


static bool parse_poll_act_time_val (uint8_t pollTime, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    switch (pollTime) {
    case 0x0:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 64us", reg_addr );
        break;
    case 0x1:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 128us", reg_addr );
        break;
    case 0x2:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 192us", reg_addr );
        break;
    case 0x3:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 256us", reg_addr );
        break;
    case 0x4:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 320us", reg_addr );
        break;
    case 0x5:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 384us", reg_addr );
        break;
    case 0x6:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 448us", reg_addr );
        break;
    case 0x7:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 512us", reg_addr );
        break;
    case 0x8:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 640us", reg_addr );
        break;
    case 0x9:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 768us", reg_addr );
        break;
    case 0xA:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 896us", reg_addr );
        break;
    case 0xB:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 1024us", reg_addr );
        break;
    case 0xC:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 2048us", reg_addr );
        break;
    default:
        fprintf (out_file_prt, "\n    reg %2u bit  8- 5: Poll Act time 512us ", reg_addr );
        break;
    }
    return res;
}

static bool parse_poll_time_val (uint8_t pollTime, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    switch (pollTime) {
    case 0x0:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 2ms", reg_addr );
        break;
    case 0x1:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 4ms", reg_addr );
        break;
    case 0x2:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 8ms", reg_addr );
        break;
    case 0x3:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 16ms", reg_addr );
        break;
    case 0x4:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 32ms", reg_addr );
        break;
    case 0x5:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 48ms", reg_addr );
        break;
    case 0x6:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 64ms", reg_addr );
        break;
    case 0x7:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 128ms", reg_addr );
        break;
    case 0x8:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 256ms", reg_addr );
        break;
    case 0x9:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 512ms", reg_addr );
        break;
    case 0xA:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 1024ms", reg_addr );
        break;
    case 0xB:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 2048ms", reg_addr );
        break;
    case 0xC:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 4096ms", reg_addr );
        break;
    default:
        fprintf (out_file_prt, "\n    reg %2u bit  4- 1: Poll time 2ms", reg_addr );
        break;
    }
    return res;
}


static bool parse_tic12400_vs1_stat (uint8_t val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    switch (val) {
    case 0:
        fprintf (out_file_prt, "\n    reg %2u bit  7- 6: R VS voltage is below threshold VS1_THRES2A", reg_addr );
        break;
    case 1:
        fprintf (
            out_file_prt,
            "\n    reg %2u bit  7- 6: R VS voltage is below threshold VS1_THRES2B and equal to or above threshold VS1_THRES2A", reg_addr );
        break;
    case 2:
        fprintf (out_file_prt, "\n    reg %2u bit  7- 6: R VS voltage is equal to or above threshold VS1_THRES2B", reg_addr );
        break;
    case 3:
        fprintf (out_file_prt, "\n    reg %2u bit  7- 6: R N/A ", reg_addr );
        break;
    }
    return res;
}

static bool parse_tic12400_vs0_stat (uint8_t val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    switch (val) {
    case 0:
        fprintf (out_file_prt, "\n    reg %2u bit  5- 4: R VS voltage is below threshold VS0_THRES2A ", reg_addr );
        break;
    case 1:
        fprintf (
            out_file_prt,
            "\n    reg %2u bit  5- 4: R VS voltage is below threshold VS0_THRES2B and equal to or above threshold VS0_THRES2A", reg_addr );
        break;
    case 2:
        fprintf (out_file_prt, "\n    reg %2u bit  5- 4: R VS voltage is equal to or above threshold VS0_THRES2B", reg_addr );
        break;
    case 3:
        fprintf (out_file_prt, "\n    reg %2u bit  5- 4: R N/A ", reg_addr );
        break;
    }
    return res;
}



static float calc_vs_voltage (void) {
    float vs_voltage = 0;
    g_one_bit_voltage = 0;
    uint16_t vs_ana;
    if (tic12400RegMap[REG_CONFIG].reg24BitVal & CONFIG_VS_RATIO_23) {
        g_one_bit_voltage = 30.0 / 1024.0;
    } else {
        g_one_bit_voltage = 9.0 / 1024.0;
    }
    vs_ana = extract_subval_from_32bit (tic12400RegMap[REG_ANA_STAT12].reg24BitVal, 9, 0);
    vs_voltage = g_one_bit_voltage * ((float)vs_ana);
    return vs_voltage;
}



static bool parse_wetting_current (uint8_t reg_val, FILE *out_file_prt, uint8_t oldBit, uint8_t littleBit, uint8_t reg_addr) {
    bool res = true;
    switch (reg_val) {
    case 0:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W no wetting current", reg_addr, oldBit, littleBit, reg_val);
        break;
    case 1:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W 1mA (typ.) wetting current", reg_addr, oldBit, littleBit, reg_val);
        break;
    case 2:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W 2mA (typ.) wetting current", reg_addr, oldBit, littleBit, reg_val);
        break;
    case 3:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W 5mA (typ.) wetting current", reg_addr, oldBit, littleBit, reg_val);
        break;
    case 4:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W 10mA (typ.) wetting current", reg_addr, oldBit, littleBit, reg_val);
        break;
    default:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W 15mA (typ.) wetting current", reg_addr, oldBit, littleBit, reg_val);
        break;
    }
    return res;
}


static float num2wett_current (uint8_t wcCode) {
    float wettingCurrentMa = 0;
    switch (wcCode) {
    case 0:
        wettingCurrentMa = 0.0;
        break;
    case 1:
        wettingCurrentMa = 1.0;
        break;
    case 2:
        wettingCurrentMa = 2.0;
        break;
    case 3:
        wettingCurrentMa = 5.0;
        break;
    case 4:
        wettingCurrentMa = 10.0;
        break;
    default:
        wettingCurrentMa = 15.0;
        break;
    }
    return wettingCurrentMa;
}



static float interpretation_comparator_threshold_code (uint8_t trCode) {
    float comparatorThreshold = 0;
    switch (trCode) {
    case 0:
        comparatorThreshold = 2.0;
        break;
    case 1:
        comparatorThreshold = 2.7;
        break;
    case 2:
        comparatorThreshold = 3.0;
        break;
    case 3:
        comparatorThreshold = 4.0;
        break;
    default:
        comparatorThreshold = 0.0;
        break;
    }
    return comparatorThreshold;
}



static bool parse_interrupt_en_comp_mode (uint8_t reg_val, FILE *out_file_prt, uint8_t oldBit, uint8_t littleBit,
                                   uint8_t channel, uint8_t reg_addr) {
    bool res = true;
    switch (reg_val) {
    case 0:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W no interrupt generation for IN%u", reg_addr, oldBit, littleBit, reg_val,
                 channel);
        break;
    case 1:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W interrupt generation on rising edge for IN%u", reg_addr, oldBit, littleBit,
                 reg_val, channel);
        break;
    case 2:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W interrupt generation on falling edge for IN%u", reg_addr, oldBit,
                 littleBit, reg_val, channel);
        break;
    case 3:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W interrupt generation on falling and rising edge for IN%u", reg_addr,
                 oldBit, littleBit, reg_val, channel);
        break;
    }
    return res;
}



static bool parse_in_a_code (uint8_t ina_code, FILE *out_file_prt, uint8_t oldBit, uint8_t littleBit, uint8_t channel, uint8_t reg_addr) {
    bool res = false;
    if (channel <= 17) {
        if (0 < (oldBit - littleBit)) {
            res = true;
            switch (ina_code) {
            case 0:
                snprintf (tic12400channelList[channel].inputADCLevel,
                          sizeof (tic12400channelList[channel].inputADCLevel), "below");
                fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R Input IN%u is below threshold 2A.", reg_addr, oldBit, littleBit,
                         ina_code, channel);
                break;
            case 1:
                snprintf (tic12400channelList[channel].inputADCLevel,
                          sizeof (tic12400channelList[channel].inputADCLevel), "middle");
                fprintf (out_file_prt,
                         "\n    reg %2u bit %02u-%02u:%u R Input IN%u is below threshold 2B and equal to or above threshold 2A", reg_addr,
                         oldBit, littleBit, ina_code, channel);
                break;
            case 2:
                snprintf (tic12400channelList[channel].inputADCLevel,
                          sizeof (tic12400channelList[channel].inputADCLevel), "above");
                fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R Input IN%u is equal to or above threshold 2B", reg_addr, oldBit,
                         littleBit, ina_code, channel);
                break;
            case 3:
                snprintf (tic12400channelList[channel].inputADCLevel,
                          sizeof (tic12400channelList[channel].inputADCLevel), "N/A");
                fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R IN%u N/A", reg_addr, oldBit, littleBit, ina_code, channel);
                break;
            default:
                res = false;
                break;
            }
            return res;
        } else {
            if ((oldBit == littleBit) && (littleBit <= 11)) {
                if (1 == ina_code) {
                    res = true;
                    snprintf (tic12400channelList[channel].inputADCLevel,
                              sizeof (tic12400channelList[channel].inputADCLevel), "aboveThres");
                    fprintf (out_file_prt, "\n    reg %2u bit    %02u:%u R Input IN%02u is above configured threshold", reg_addr, littleBit,
                             ina_code, channel);
                }
                if (0 == ina_code) {
                    res = true;
                    snprintf (tic12400channelList[channel].inputADCLevel,
                              sizeof (tic12400channelList[channel].inputADCLevel), "below");
                    fprintf (out_file_prt, "\n    reg %2u bit    %02u:%u R Input IN%02u is below configured threshold", reg_addr, littleBit,
                             ina_code, channel);
                }
            }
            return res;
        }
    } else if ((18 <= channel) || (channel <= 22)) {
        res = true;
        switch (ina_code) {
        case 0:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "below");
            fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R Input IN%u is below threshold 3A.", reg_addr, oldBit, littleBit,
                     ina_code, channel);
            break;
        case 1:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "middle");
            fprintf (out_file_prt,
                     "\n    reg %2u bit %02u-%02u:%u R Input IN%u is below threshold 3B and equal to or above threshold 3A", reg_addr,
                     oldBit, littleBit, ina_code, channel);
            break;
        case 2:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "edge");
            fprintf (out_file_prt,
                     "\n    reg %2u bit %02u-%02u:%u R Input IN%u is below threshold 3C and equal to or above threshold 3B", reg_addr,
                     oldBit, littleBit, ina_code, channel);
            break;
        case 3:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "N/A");
            fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R Input is IN%u is equal to or above threshold 3C", reg_addr, oldBit,
                     littleBit, ina_code, channel);
            break;
        default:
            res = false;
            break;
        }
    } else if (23 == channel) {
        switch (ina_code) {
        case 0:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "below");
            fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u Input IN23 is below threshold 3A.", reg_addr, oldBit, littleBit, ina_code);
            break;
        case 1:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "mid");
            fprintf (out_file_prt,
                     "\n    reg %2u bit %02u-%02u:%u R Input IN23 is below threshold 3B and equal to or above threshold 3A.", reg_addr,
                     oldBit, littleBit, ina_code);
            break;
        case 2:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "mid");
            fprintf (out_file_prt,
                     "\n    reg %2u bit %02u-%02u:%u R Input IN23 is below threshold 3C and equal to or above threshold 3B.", reg_addr,
                     oldBit, littleBit, ina_code);
            break;
        case 3:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "mid");
            fprintf (out_file_prt,
                     "\n    reg %2u bit %02u-%02u:%u R Input IN23 is below threshold THRES8 and equal to or above threshold 3C.", reg_addr,
                     oldBit, littleBit, ina_code);
            break;
        case 4:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "mid");
            fprintf (
                out_file_prt,
                "\n    reg %2u bit %02u-%02u:%u R Input IN23 is below threshold THRES9 and equal to or above threshold THRES8.", reg_addr,
                oldBit, littleBit, ina_code);
            break;
        case 5:
            snprintf (tic12400channelList[channel].inputADCLevel, sizeof (tic12400channelList[channel].inputADCLevel),
                      "above");
            fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R Input IN23 is equal to or above threshold THRES9", reg_addr, oldBit,
                     littleBit, ina_code);
            break;
        default:
            res = false;
            break;
        }
    } else {
        res = false;
    }
    return res;
}




static bool parse_comparator_threshold (uint8_t reg_val, FILE *out_file_prt, uint8_t oldBit, uint8_t littleBit, uint8_t reg_addr) {
    bool res = true;
    switch (reg_val) {
    case 0:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W comparator threshold set to 2V", reg_addr, oldBit, littleBit, reg_val);
        break;
    case 1:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W comparator threshold set to 2.7V", reg_addr, oldBit, littleBit, reg_val);
        break;
    case 2:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W comparator threshold set to 3V", reg_addr, oldBit, littleBit, reg_val);
        break;
    case 3:
        fprintf (out_file_prt, "\n    reg %2u bit %02u-%02u:%u R/W comparator threshold set to 4V", reg_addr, oldBit, littleBit, reg_val);
        break;
    default:
        break;
    }
    return res;
}

static char interpret_int_code (uint8_t intCode) {
    char intCodeLetter = '?';
    switch (intCode) {
    case 0:
        intCodeLetter = 'N';
        break;
    case 1:
        intCodeLetter = 'R';
        break;
    case 2:
        intCodeLetter = 'F';
        break;
    case 3:
        intCodeLetter = 'B';
        break;
    }
    return intCodeLetter;
}

bool parse_tic12400_regs_file (char *inFileName, char *outFileName) {
    printf ("\n%s()\n", __FUNCTION__);
    char curFileStr[500];
    bool res = false;
    FILE *inFilePrt = NULL;
    printf ("\n>In file: [%s]", inFileName);
    inFilePrt = fopen (inFileName, "r");
    if (inFilePrt) {
        int line = 0;
        while (NULL != fgets (curFileStr, sizeof (curFileStr), inFilePrt)) {

#if DEBUG_IN_FILE_STR
            printf ("\n>[%s]", curFileStr);
#endif
            if ((REG_ADDR_LEN + REG_VAL32_LEN) < strlen (curFileStr)) {
                uint8_t reg8bitAddr;
                uint32_t reg32bitVal;
                res = try_canch_hex_uint8 (curFileStr, strlen (curFileStr), &reg8bitAddr);
                if (true == res) {
                    res = try_canch_hex_uint32 (curFileStr, strlen (curFileStr), &reg32bitVal);
                    if (true == res) {
                        load_tic12400_reg (reg8bitAddr, reg32bitVal);
                    } else {
                        printf ("\n Unable to parse 24 bit reg val in [%s]", curFileStr);
                    }
                } else {
                    printf ("\n Unable to parse 8 bit reg addr in [%s]", curFileStr);
                }
            }
#if DEBUG_IN_FILE_STR
            printf ("\n[%x] [%x]", reg_addr, reg32_val);
#endif
            line++;
        }
        fclose (inFilePrt);
        res = true;
    } else {
        printf ("\nUnable to open file [%s]", inFileName);
    }

    FILE *out_file_prt = NULL;
    out_file_prt = fopen (outFileName, "w");
    if (out_file_prt) {
        if (true == res) {
            uint8_t reg_8bit_addr;
            for (reg_8bit_addr = 0; reg_8bit_addr <= REG_MODE; reg_8bit_addr++) {
                parse_tic12400_reg (reg_8bit_addr, out_file_prt);
            }
            write_channel_table (out_file_prt);
            printf ("\n processed %u/47 registers %7.2f%% remain %u registers", procRegCnd,
                    cacl_percent ((float)procRegCnd, AMOUNT_OF_REGISTERS), AMOUNT_OF_REGISTERS - procRegCnd);
            printf ("\n processed %u/1128 bits %7.2f%%", procRegCnd * 24,
                    cacl_percent ((float)procRegCnd * 24.0, AMOUNT_OF_REGISTERS * 24.0));
            fprintf (out_file_prt, "\n\n Support: aabzele@gmail.com Alexander Barunin");
            fclose (out_file_prt);
        }
    }

    return res;
}

char *tic12400_reg_2_name[51] = {
    "   RESERVED_00H", // 0
    "      DEVICE_ID", "       INT_STAT", "            CRC", "   IN_STAT_MISC", "   IN_STAT_COMP",
    "   IN_STAT_ADC0", "   IN_STAT_ADC1", "IN_STAT_MATRIX0", "IN_STAT_MATRIX1",
    "      ANA_STAT0", // 10
    "      ANA_STAT1", "      ANA_STAT2", "      ANA_STAT3", "      ANA_STAT4", "      ANA_STAT5",
    "      ANA_STAT6", "      ANA_STAT7", "      ANA_STAT8", "      ANA_STAT9",
    "     ANA_STAT10", // 20
    "     ANA_STAT11", "     ANA_STAT12", "   RESERVED_17H", "   RESERVED_18H", "   RESERVED_19H",
    "         CONFIG", "         IN_EN",  "     CS_SELECT",  "       WC_CFG0",
    "       WC_CFG1", // 30
    "      CCP_CFG0",  "      CCP_CFG1",  "    THRES_COMP",  "  INT_EN_COMP1",  "  INT_EN_COMP2",
    "   INT_EN_CFG0",  "   INT_EN_CFG1",  "   INT_EN_CFG2",  "   INT_EN_CFG3",
    "   INT_EN_CFG4", // 40
    "    THRES_CFG0",  "    THRES_CFG1",  "    THRES_CFG2",  "    THRES_CFG3",  "    THRES_CFG4",
    " THRESMAP_CFG0",  " THRESMAP_CFG1",  " THRESMAP_CFG2",  "        MATRIX",
    "          MODE" // 50
};

bool load_tic12400_reg (uint8_t reg_addr, uint32_t reg32_val) {
    bool res = false;
    tic12400RegMap[reg_addr].reg24BitVal = reg32_val;
    tic12400RegMap[reg_addr].regName = tic12400_reg_2_name[reg_addr];
    tic12400RegMap[reg_addr].valid = true;
    return res;
}

typedef struct xCollomn_t {
    char mane[100];
    uint8_t width;
} collomn_t;

static bool parse_tic12400_thresmap_cfg0_register_46 (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr){
	uint8_t code;
	code = extract_subval_from_32bit (reg32_val, 23, 21);
	fprintf (out_file_prt, "\n    reg %2u bit 23-21:%u RW THRESMAP_IN7 THRES%u",reg_addr, code, code);
	code = extract_subval_from_32bit (reg32_val, 20, 18);
	fprintf (out_file_prt, "\n    reg %2u bit 20-18:%u RW THRESMAP_IN6 THRES%u",reg_addr, code, code);
	code = extract_subval_from_32bit (reg32_val, 17, 15);
	fprintf (out_file_prt, "\n    reg %2u bit 17-15:%u RW THRESMAP_IN5 THRES%u",reg_addr, code, code);
	code = extract_subval_from_32bit (reg32_val, 14, 12);
	fprintf (out_file_prt, "\n    reg %2u bit 14-12:%u RW THRESMAP_IN4 THRES%u",reg_addr, code, code);
	code = extract_subval_from_32bit (reg32_val, 11, 9);
	fprintf (out_file_prt, "\n    reg %2u bit 11- 9:%u RW THRESMAP_IN3 THRES%u",reg_addr, code, code);
	code = extract_subval_from_32bit (reg32_val, 8, 6);
	fprintf (out_file_prt, "\n    reg %2u bit  8- 6:%u RW THRESMAP_IN2 THRES%u",reg_addr, code, code);
	code = extract_subval_from_32bit (reg32_val, 5, 3);
	fprintf (out_file_prt, "\n    reg %2u bit  5- 3:%u RW THRESMAP_IN1 THRES%u",reg_addr, code, code);
	code = extract_subval_from_32bit (reg32_val, 2, 0);
	fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW THRESMAP_IN0 THRES%u",reg_addr, code, code);
    return true;
}

static bool parse_tic12400_thresmap_cfg1_register_47 (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr){
	uint8_t code;
	code = extract_subval_from_32bit (reg32_val, 23, 21);
	fprintf (out_file_prt, "\n    reg %2u bit 23-18:%u R Reserved",reg_addr, code);

	code = extract_subval_from_32bit (reg32_val, 17, 15);
	fprintf (out_file_prt, "\n    reg %2u bit 17-15:%u RW THRESMAP_IN12_IN17_THRES2B THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 14, 12);
	fprintf (out_file_prt, "\n    reg %2u bit 14-12:%u RW THRESMAP_IN12_IN17_THRES2A THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 11, 9);
	fprintf (out_file_prt, "\n    reg %2u bit 11- 9:%u RW THRESMAP_IN11 THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 9, 6);
	fprintf (out_file_prt, "\n    reg %2u bit  8- 6:%u RW THRESMAP_IN10 THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 5, 3);
	fprintf (out_file_prt, "\n    reg %2u bit  5- 3:%u RW THRESMAP_IN9 THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 2, 0);
	fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW THRESMAP_IN8 THRES%u",reg_addr, code, code);
    return true;
}

static bool parse_tic12400_thresmap_cfg2_register_48 (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr){
	uint8_t code;
	code = extract_subval_from_32bit (reg32_val, 23, 21);
	fprintf (out_file_prt, "\n    reg %2u bit 23-21:%u R Reserved",reg_addr, code);

	code = extract_subval_from_32bit (reg32_val, 20, 18);
	fprintf (out_file_prt, "\n    reg %2u bit 20-18:%u RW THRESMAP_VS1_THRES2B", reg_addr, code);

	code = extract_subval_from_32bit (reg32_val, 17, 15);
	fprintf (out_file_prt, "\n    reg %2u bit 17-15:%u RW THRESMAP_VS1_THRES2A THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 14, 12);
	fprintf (out_file_prt, "\n    reg %2u bit 14-12:%u RW THRESMAP_VS0_THRES2B THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 11, 9);
	fprintf (out_file_prt, "\n    reg %2u bit 11- 9:%u RW THRESMAP_VS0_THRES2A THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 8, 6);
	fprintf (out_file_prt, "\n    reg %2u bit  9- 6:%u RW THRESMAP_IN18_IN23_THRES3C THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 5, 3);
	fprintf (out_file_prt, "\n    reg %2u bit  5- 3:%u RW THRESMAP_IN18_IN23_THRES3B THRES%u",reg_addr, code, code);

	code = extract_subval_from_32bit (reg32_val, 2, 0);
	fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW THRESMAP_IN18_IN23_THRES3A THRES%u",reg_addr, code, code);
    return true;
}

static bool parse_tic12400_matrix_register_49(uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr){
	uint8_t code;
	code = extract_subval_from_32bit (reg32_val, 23, 17);
	fprintf (out_file_prt, "\n    reg %2u bit 23-17:%u R Reserved", reg_addr, code);
	code = extract_subval_from_32bit (reg32_val, 16, 15);
	if ( 0 == code) {
     	fprintf (out_file_prt, "\n    reg %2u bit 16-15:%u RW no interrupt generation for w.r.t. threshold THRES_COM", reg_addr, code);
	}
	if ( 1 == code) {
     	fprintf (out_file_prt, "\n    reg %2u bit 16-15:%u RW interrupt generation on rising edge above threshold THRES_COM", reg_addr, code);
	}
	if ( 2 == code) {
     	fprintf (out_file_prt, "\n    reg %2u bit 16-15:%u RW interrupt generation on falling edge below threshold THRES_COM", reg_addr, code);
	}
	if ( 3 == code) {
     	fprintf (out_file_prt, "\n    reg %2u bit 16-15:%u RW interrupt generation on falling and rising edge of threshold THRES_COM", reg_addr, code);
	}
	code = extract_subval_from_32bit (reg32_val, 14, 5);
	fprintf (out_file_prt, "\n    reg %2u bit 14- 5:%u RW THRES_COM", reg_addr, code);

	code = extract_subval_from_32bit (reg32_val, 4, 3);
	if (0==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  4- 3:%u RW no matrix, regular inputs only", reg_addr, code);
	}
	if (1==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  4- 3:%u RW 4x4 matrix", reg_addr, code);
	}
	if (2==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  4- 3:%u RW 5x5 matrix", reg_addr, code);
	}
	if (3==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  4- 3:%u RW 6x6 matrix", reg_addr, code);
	}

	code = extract_subval_from_32bit (reg32_val, 2, 0);
	if (0==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 64μs", reg_addr, code);
	}
	if (1==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 128μs", reg_addr, code);
	}
	if (2==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 256μs", reg_addr, code);
	}
	if (3==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 384μs", reg_addr, code);
	}
	if (4==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 512μs", reg_addr, code);
	}
	if (5==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 768μs", reg_addr, code);
	}
	if (6==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 1024μs", reg_addr, code);
	}
	if (7==code) {
		fprintf (out_file_prt, "\n    reg %2u bit  2- 0:%u RW Polling active time setting for the matrix inputs: 1360μs", reg_addr, code);
	}
    return true;
}


static bool parse_tic12400_crc_register_03h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    uint32_t code;
    code = extract_subval_from_32bit (reg32_val, 23, 16);
    fprintf (out_file_prt, "\n    reg %2u bit 22-16:%u R Reserved", 3, code);

    code = extract_subval_from_32bit (reg32_val, 15, 0);
    fprintf (out_file_prt, "\n    reg %2u bit 15- 0:%u R CRC-16-CCITT calculation result", 3, code);
    bool res = true;
    return res;
}

static bool parse_tic12400_int_en_comp1_register_22h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    (void)reg32_val;
    (void)out_file_prt;
    uint8_t inc_en_11, inc_en_10, inc_en_9, inc_en_8;
    uint8_t inc_en_7, inc_en_6, inc_en_5, inc_en_4;
    uint8_t inc_en_3, inc_en_2, inc_en_1, inc_en_0;
    inc_en_11 = extract_subval_from_32bit (reg32_val, 23, 22);
    parse_interrupt_en_comp_mode (inc_en_11, out_file_prt, 23, 22, 11,reg_addr);
    tic12400channelList[11].interruptMode = interpret_int_code (inc_en_11);

    inc_en_10 = extract_subval_from_32bit (reg32_val, 21, 20);
    parse_interrupt_en_comp_mode (inc_en_10, out_file_prt, 21, 20, 10,reg_addr);
    tic12400channelList[10].interruptMode = interpret_int_code (inc_en_10);

    inc_en_9 = extract_subval_from_32bit (reg32_val, 19, 18);
    parse_interrupt_en_comp_mode (inc_en_9, out_file_prt, 19, 18, 9,reg_addr);
    tic12400channelList[9].interruptMode = interpret_int_code (inc_en_9);

    inc_en_8 = extract_subval_from_32bit (reg32_val, 17, 16);
    parse_interrupt_en_comp_mode (inc_en_8, out_file_prt, 17, 16, 8,reg_addr);
    tic12400channelList[8].interruptMode = interpret_int_code (inc_en_8);

    inc_en_7 = extract_subval_from_32bit (reg32_val, 15, 14);
    parse_interrupt_en_comp_mode (inc_en_7, out_file_prt, 15, 14, 7,reg_addr);
    tic12400channelList[7].interruptMode = interpret_int_code (inc_en_7);

    inc_en_6 = extract_subval_from_32bit (reg32_val, 13, 12);
    parse_interrupt_en_comp_mode (inc_en_6, out_file_prt, 13, 12, 6,reg_addr);
    tic12400channelList[6].interruptMode = interpret_int_code (inc_en_6);

    inc_en_5 = extract_subval_from_32bit (reg32_val, 11, 10);
    parse_interrupt_en_comp_mode (inc_en_5, out_file_prt, 11, 10, 5,reg_addr);
    tic12400channelList[5].interruptMode = interpret_int_code (inc_en_5);

    inc_en_4 = extract_subval_from_32bit (reg32_val, 9, 8);
    parse_interrupt_en_comp_mode (inc_en_4, out_file_prt, 9, 8, 4,reg_addr);
    tic12400channelList[4].interruptMode = interpret_int_code (inc_en_4);

    inc_en_3 = extract_subval_from_32bit (reg32_val, 7, 6);
    parse_interrupt_en_comp_mode (inc_en_3, out_file_prt, 7, 6, 3,reg_addr);
    tic12400channelList[3].interruptMode = interpret_int_code (inc_en_3);

    inc_en_2 = extract_subval_from_32bit (reg32_val, 5, 4);
    parse_interrupt_en_comp_mode (inc_en_2, out_file_prt, 5, 4, 2,reg_addr);
    tic12400channelList[2].interruptMode = interpret_int_code (inc_en_2);

    inc_en_1 = extract_subval_from_32bit (reg32_val, 3, 2);
    parse_interrupt_en_comp_mode (inc_en_1, out_file_prt, 3, 2, 1,reg_addr);
    tic12400channelList[1].interruptMode = interpret_int_code (inc_en_1);

    inc_en_0 = extract_subval_from_32bit (reg32_val, 1, 0);
    parse_interrupt_en_comp_mode (inc_en_0, out_file_prt, 1, 0, 0,reg_addr);
    tic12400channelList[0].interruptMode = interpret_int_code (inc_en_0);
    bool res = true;
    return res;
}

static bool parse_tic12400_int_en_comp2_register_23h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint8_t inc_en_23, inc_en_22, inc_en_21, inc_en_20, inc_en_19, inc_en_18;
    uint8_t inc_en_17, inc_en_16, inc_en_15, inc_en_14, inc_en_13, inc_en_12;
    inc_en_23 = extract_subval_from_32bit (reg32_val, 23, 22);
    parse_interrupt_en_comp_mode (inc_en_23, out_file_prt, 23, 22, 23,reg_addr);
    tic12400channelList[23].interruptMode = interpret_int_code (inc_en_23);

    inc_en_22 = extract_subval_from_32bit (reg32_val, 21, 20);
    parse_interrupt_en_comp_mode (inc_en_22, out_file_prt, 21, 20, 22,reg_addr);
    tic12400channelList[22].interruptMode = interpret_int_code (inc_en_22);

    inc_en_21 = extract_subval_from_32bit (reg32_val, 19, 18);
    parse_interrupt_en_comp_mode (inc_en_21, out_file_prt, 19, 18, 21,reg_addr);
    tic12400channelList[21].interruptMode = interpret_int_code (inc_en_21);

    inc_en_20 = extract_subval_from_32bit (reg32_val, 17, 16);
    parse_interrupt_en_comp_mode (inc_en_20, out_file_prt, 17, 16, 20,reg_addr);
    tic12400channelList[20].interruptMode = interpret_int_code (inc_en_20);

    inc_en_19 = extract_subval_from_32bit (reg32_val, 15, 14);
    parse_interrupt_en_comp_mode (inc_en_19, out_file_prt, 15, 14, 19,reg_addr);
    tic12400channelList[19].interruptMode = interpret_int_code (inc_en_19);

    inc_en_18 = extract_subval_from_32bit (reg32_val, 13, 12);
    parse_interrupt_en_comp_mode (inc_en_18, out_file_prt, 13, 12, 18,reg_addr);
    tic12400channelList[18].interruptMode = interpret_int_code (inc_en_18);

    inc_en_17 = extract_subval_from_32bit (reg32_val, 11, 10);
    parse_interrupt_en_comp_mode (inc_en_17, out_file_prt, 11, 10, 17,reg_addr);
    tic12400channelList[17].interruptMode = interpret_int_code (inc_en_17);

    inc_en_16 = extract_subval_from_32bit (reg32_val, 9, 8);
    parse_interrupt_en_comp_mode (inc_en_16, out_file_prt, 9, 8, 16,reg_addr);
    tic12400channelList[16].interruptMode = interpret_int_code (inc_en_16);

    inc_en_15 = extract_subval_from_32bit (reg32_val, 7, 6);
    parse_interrupt_en_comp_mode (inc_en_15, out_file_prt, 7, 6, 15,reg_addr);
    tic12400channelList[15].interruptMode = interpret_int_code (inc_en_15);

    inc_en_14 = extract_subval_from_32bit (reg32_val, 5, 4);
    parse_interrupt_en_comp_mode (inc_en_14, out_file_prt, 5, 4, 14,reg_addr);
    tic12400channelList[14].interruptMode = interpret_int_code (inc_en_14);

    inc_en_13 = extract_subval_from_32bit (reg32_val, 3, 2);
    parse_interrupt_en_comp_mode (inc_en_13, out_file_prt, 3, 2, 13,reg_addr);
    tic12400channelList[13].interruptMode = interpret_int_code (inc_en_13);

    inc_en_12 = extract_subval_from_32bit (reg32_val, 1, 0);
    parse_interrupt_en_comp_mode (inc_en_12, out_file_prt, 1, 0, 12,reg_addr);
    tic12400channelList[12].interruptMode = interpret_int_code (inc_en_12);
    return res;
}

static bool parse_tic12400_thres_comp_register_21h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint32_t reserved;
    uint8_t thres_comp_in20_in23, thres_comp_in16_in19;
    uint8_t thres_comp_in12_in15, thres_comp_in8_in11;
    uint8_t thres_comp_in4_in7, thres_comp_in0_in3;

    reserved = extract_subval_from_32bit (reg32_val, 31, 12);
    fprintf (out_file_prt, "\n    reg %2u bit 32-12:%u R RESERVED", reg_addr, reserved);

    thres_comp_in20_in23 = extract_subval_from_32bit (reg32_val, 11, 10);
    parse_comparator_threshold (thres_comp_in20_in23, out_file_prt, 11, 10, reg_addr);
    tic12400channelList[23].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in20_in23);
    tic12400channelList[22].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in20_in23);
    tic12400channelList[21].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in20_in23);
    tic12400channelList[20].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in20_in23);

    thres_comp_in16_in19 = extract_subval_from_32bit (reg32_val, 9, 8);
    parse_comparator_threshold (thres_comp_in16_in19, out_file_prt, 9, 8, reg_addr);
    tic12400channelList[19].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in16_in19);
    tic12400channelList[18].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in16_in19);
    tic12400channelList[17].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in16_in19);
    tic12400channelList[16].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in16_in19);

    thres_comp_in12_in15 = extract_subval_from_32bit (reg32_val, 7, 6);
    parse_comparator_threshold (thres_comp_in12_in15, out_file_prt, 7, 6, reg_addr);
    tic12400channelList[15].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in12_in15);
    tic12400channelList[14].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in12_in15);
    tic12400channelList[13].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in12_in15);
    tic12400channelList[12].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in12_in15);

    thres_comp_in8_in11 = extract_subval_from_32bit (reg32_val, 5, 4);
    parse_comparator_threshold (thres_comp_in8_in11, out_file_prt, 5, 4, reg_addr);
    tic12400channelList[11].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in8_in11);
    tic12400channelList[10].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in8_in11);
    tic12400channelList[9].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in8_in11);
    tic12400channelList[8].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in8_in11);

    thres_comp_in4_in7 = extract_subval_from_32bit (reg32_val, 3, 2);
    parse_comparator_threshold (thres_comp_in4_in7, out_file_prt, 3, 2, reg_addr);
    tic12400channelList[7].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in4_in7);
    tic12400channelList[6].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in4_in7);
    tic12400channelList[5].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in4_in7);
    tic12400channelList[4].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in4_in7);

    thres_comp_in0_in3 = extract_subval_from_32bit (reg32_val, 1, 0);
    parse_comparator_threshold (thres_comp_in0_in3, out_file_prt, 1, 0, reg_addr);
    tic12400channelList[3].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in0_in3);
    tic12400channelList[2].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in0_in3);
    tic12400channelList[1].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in0_in3);
    tic12400channelList[0].comparatorThreshold = interpretation_comparator_threshold_code (thres_comp_in0_in3);
    return res;
}

static bool parse_tic12400_interrupt_status_register_02h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    if (reg32_val & INT_STAT_CHK_FAIL_13) {
        fprintf (out_file_prt, "\n    reg %2u bit 13:1 RC An error is detected when loading factory settings into the device upon "
                             "device initialization", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit 13:0 RC Default factory setting is successfully loaded upon device initialization", reg_addr );
    }
    if (reg32_val & INT_STAT_ADC_DIAG_12) {
        fprintf (out_file_prt, "\n    reg %2u bit 12:1 RC ADC self-diagnostic error is detected", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 12:0 RC No ADC self-diagnostic error is detected", reg_addr );
    }
    if (reg32_val & INT_STAT_WET_DIAG_11) {
        fprintf (out_file_prt, "\n    reg %2u bit 11:1 RC Wetting current error is detected ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 11:0 RC No wetting current error is detected ", reg_addr );
    }
    if (reg32_val & INT_STAT_VS1_10) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit 10:1 RC VS voltage state change occurred with respect to VS1_THRES2A or VS1_THRES2B ", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit 10:0 RC No VS voltage state change occurred with respect to VS1_THRES2A or VS1_THRES2B ", reg_addr );
    }
    if (reg32_val & INT_STAT_VS0_9) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  9:1 RC VS voltage state change occurred with respect to VS0_THRES2A or VS0_THRES2B", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  9:0 RC No VS voltage state change occurred with respect to VS0_THRES2A or VS0_THRES2B ", reg_addr );
    }
    if (reg32_val & INT_STAT_CRC_CALC_8) {
        fprintf (out_file_prt, "\n    reg %2u bit  8:1 RC CRC calculation is finished ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  8:0 RC CRC calculation is running, not started ", reg_addr );
    }
    if (reg32_val & INT_STAT_UV_7) {
        fprintf (out_file_prt, "\n    reg %2u bit  7:1 RC Under-voltage condition occurred or cleared on the VS pin", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  7:0 RC No under-voltage condition occurred", reg_addr );
    }
    if (reg32_val & INT_STAT_OV_6) {
        fprintf (out_file_prt, "\n    reg %2u bit  6:1 RC Over-voltage condition occurred or cleared on the VS pin ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  6:0 RC No over-voltage condition occurred or cleared on the VS pin,", reg_addr );
    }

    if (reg32_val & INT_STAT_TW_5) {
        fprintf (out_file_prt, "\n    reg %2u bit  5:1 RC Temperature warning event occurred or cleared ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  5:0 RC No temperature warning event occurred ", reg_addr );
    }
    if (reg32_val & INT_STAT_TSD_4) {
        fprintf (out_file_prt, "\n    reg %2u bit  4:1 RC Temperature Shutdown event occurred or cleared ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  4:0 RC No temperature Shutdown event occurred ", reg_addr );
    }
    if (reg32_val & INT_STAT_SSC_3) {
        fprintf (out_file_prt, "\n    reg %2u bit  3:1 RC Switch state change occurred ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  3:0 RC No switch state change occurred ", reg_addr );
    }
    if (reg32_val & INT_STAT_PRTY_FAIL_2) {
        fprintf (out_file_prt, "\n    reg %2u bit  2:1 RC Parity error occurred", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  2:0 RC No parity error occurred in the last received SI stream", reg_addr );
    }

    if (reg32_val & INT_STAT_SPI_FAIL_1) {
        fprintf (out_file_prt, "\n    reg %2u bit  1:1 RC SPI error occurred ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  1:0 RC 32 clock pulse during a CS = low sequence was detected", reg_addr );
    }
    if (reg32_val & INT_STAT_POR_0) {
        fprintf (out_file_prt, "\n    reg %2u bit  0:1 RC Power-On-Reset (POR) event occurred", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  0:0 RC no Power-On-Reset (POR) event occurred", reg_addr );
    }

    return res;
}

static bool parse_tic12400_wc_cfg0_register_1dh  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint32_t wc_in0_in1;
    uint32_t wc_in2_in3;
    uint32_t wc_in4;
    uint32_t wc_in5;
    uint32_t wc_in6_in7;
    uint32_t wc_in8_in9;
    uint32_t wc_in10;
    uint32_t wc_in11;
    wc_in11 = extract_subval_from_32bit (reg32_val, 23, 21);
    parse_wetting_current (wc_in11, out_file_prt, 23, 21, reg_addr);
    tic12400channelList[11].wettingCurrent = num2wett_current (wc_in11);

    wc_in10 = extract_subval_from_32bit (reg32_val, 20, 18);
    parse_wetting_current (wc_in10, out_file_prt, 20, 18, reg_addr);
    tic12400channelList[10].wettingCurrent = num2wett_current (wc_in10);

    wc_in8_in9 = extract_subval_from_32bit (reg32_val, 17, 15);
    parse_wetting_current (wc_in8_in9, out_file_prt, 17, 15, reg_addr);
    tic12400channelList[8].wettingCurrent = num2wett_current (wc_in8_in9);
    tic12400channelList[9].wettingCurrent = num2wett_current (wc_in8_in9);

    wc_in6_in7 = extract_subval_from_32bit (reg32_val, 14, 12);
    parse_wetting_current (wc_in6_in7, out_file_prt, 14, 12, reg_addr);
    tic12400channelList[6].wettingCurrent = num2wett_current (wc_in6_in7);
    tic12400channelList[7].wettingCurrent = num2wett_current (wc_in6_in7);

    wc_in5 = extract_subval_from_32bit (reg32_val, 11, 9);
    parse_wetting_current (wc_in5, out_file_prt, 11, 9, reg_addr);
    tic12400channelList[5].wettingCurrent = num2wett_current (wc_in5);

    wc_in4 = extract_subval_from_32bit (reg32_val, 8, 6);
    parse_wetting_current (wc_in4, out_file_prt, 8, 6, reg_addr);
    tic12400channelList[4].wettingCurrent = num2wett_current (wc_in4);

    wc_in2_in3 = extract_subval_from_32bit (reg32_val, 5, 3);
    parse_wetting_current (wc_in2_in3, out_file_prt, 5, 3, reg_addr);
    tic12400channelList[2].wettingCurrent = num2wett_current (wc_in2_in3);
    tic12400channelList[3].wettingCurrent = num2wett_current (wc_in2_in3);

    wc_in0_in1 = extract_subval_from_32bit (reg32_val, 2, 0);
    parse_wetting_current (wc_in0_in1, out_file_prt, 2, 0, reg_addr);
    tic12400channelList[0].wettingCurrent = num2wett_current (wc_in0_in1);
    tic12400channelList[1].wettingCurrent = num2wett_current (wc_in0_in1);

    return res;
}

static bool parse_tic12400_wc_cfg1_register_1eh  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint8_t reserved;
    uint8_t wc_in23;
    uint8_t wc_in22;
    uint8_t wc_in20_in21;
    uint8_t wc_in18_in19;
    uint8_t wc_in16_in17;
    uint8_t wc_in14_in15;
    uint8_t wc_in12_in13;

    reserved = extract_subval_from_32bit (reg32_val, 23, 22);
    fprintf (out_file_prt, "\n    reg %2u bit  23:22: %u", reg_addr, reserved);

    if (reg32_val & WC_CFG1_AUTO_SCALE_DIS_CSI_22) {
        fprintf (out_file_prt, "\n    reg %2u bit  22:1 RW Disable wetting current auto-scaling (to 2mA) in continuous mode for "
                             "10mA and 15mA settings upon switch closure for all inputs enabled with CS", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  22:0 RW Enable wetting current auto-scaling (to 2mA) in continuous mode for "
                             "10mA and 15mA settings upon switch closure for all inputs enabled with CSI", reg_addr );
    }

    if (reg32_val & WC_CFG1_AUTO_SCALE_DIS_CSO_21) {
        fprintf (out_file_prt, "\n    reg %2u bit  21:1 RW Disable wetting current auto-scaling (to 2mA) in continuous mode for "
                             "10mA and 15mA settings upon switch closure for all inputs enabled with CSO", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  21:0 RW Enable wetting current auto-scaling (to 2mA) in continuous mode for "
                             "10mA and 15mA settings upon switch closure for all inputs enabled with CSO", reg_addr );
    }

    wc_in23 = extract_subval_from_32bit (reg32_val, 20, 18);
    parse_wetting_current (wc_in23, out_file_prt, 20, 18, reg_addr);
    tic12400channelList[23].wettingCurrent = num2wett_current (wc_in23);

    wc_in22 = extract_subval_from_32bit (reg32_val, 17, 15);
    parse_wetting_current (wc_in22, out_file_prt, 17, 15, reg_addr);
    tic12400channelList[22].wettingCurrent = num2wett_current (wc_in22);

    wc_in20_in21 = extract_subval_from_32bit (reg32_val, 14, 12);
    parse_wetting_current (wc_in20_in21, out_file_prt, 14, 12, reg_addr);
    tic12400channelList[20].wettingCurrent = num2wett_current (wc_in20_in21);
    tic12400channelList[21].wettingCurrent = num2wett_current (wc_in20_in21);

    wc_in18_in19 = extract_subval_from_32bit (reg32_val, 11, 9);
    parse_wetting_current (wc_in18_in19, out_file_prt, 11, 9, reg_addr);
    tic12400channelList[18].wettingCurrent = num2wett_current (wc_in18_in19);
    tic12400channelList[19].wettingCurrent = num2wett_current (wc_in18_in19);

    wc_in16_in17 = extract_subval_from_32bit (reg32_val, 8, 6);
    parse_wetting_current (wc_in16_in17, out_file_prt, 8, 6, reg_addr);
    tic12400channelList[16].wettingCurrent = num2wett_current (wc_in16_in17);
    tic12400channelList[17].wettingCurrent = num2wett_current (wc_in16_in17);

    wc_in14_in15 = extract_subval_from_32bit (reg32_val, 5, 3);
    parse_wetting_current (wc_in14_in15, out_file_prt, 5, 3, reg_addr);
    tic12400channelList[14].wettingCurrent = num2wett_current (wc_in14_in15);
    tic12400channelList[15].wettingCurrent = num2wett_current (wc_in14_in15);

    wc_in12_in13 = extract_subval_from_32bit (reg32_val, 2, 0);
    parse_wetting_current (wc_in12_in13, out_file_prt, 2, 0, reg_addr);
    tic12400channelList[12].wettingCurrent = num2wett_current (wc_in12_in13);
    tic12400channelList[13].wettingCurrent = num2wett_current (wc_in12_in13);

    return res;
}

static bool parse_tic12400_in_stat_misc_register_04h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint8_t vs1_stat = 0;
    uint8_t vs0_stat = 0;

    if (reg32_val & IN_STAT_MISC_ADC_D_12) {
        fprintf (out_file_prt, "\n    reg %2u bit 12: R An error is identified from ADC self-diagnostic.", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 12: R No error is identified from ADC self-diagnostic.", reg_addr );
    }
    if (reg32_val & IN_STAT_MISC_IN3_D_11) {
        fprintf (out_file_prt, "\n    reg %2u bit 11: R Current sink on IN3 is abnormal", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 11: R Current sink on IN3 is operational", reg_addr );
    }
    if (reg32_val & IN_STAT_MISC_IN2_D_10) {
        fprintf (out_file_prt, "\n    reg %2u bit 10: R Current sink on IN2 is abnormal", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 10: R Current sink on IN2 is operational", reg_addr );
    }
    if (reg32_val & IN_STAT_MISC_IN1_D_9) {
        fprintf (out_file_prt, "\n    reg %2u bit  9: R Current source on IN1 is abnormal", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  9: R Current source on IN1 is operational", reg_addr );
    }
    if (reg32_val & IN_STAT_MISC_IN0_D_8) {
        fprintf (out_file_prt, "\n    reg %2u bit  8: R Current source on IN0 is abnormal", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  8: R Current source on IN0 is operational", reg_addr );
    }
    vs1_stat = extract_subval_from_32bit (reg32_val, 7, 6);
    parse_tic12400_vs1_stat (vs1_stat, out_file_prt, reg_addr);
    vs0_stat = extract_subval_from_32bit (reg32_val, 5, 4);
    parse_tic12400_vs0_stat (vs0_stat, out_file_prt, reg_addr);

    if (reg32_val & IN_STAT_MISC_UV_STAT_3) {
        fprintf (out_file_prt, "\n    reg %2u bit  3: R VS voltage is below the under-voltage condition threshold", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  3: R VS voltage is above the under-voltage condition threshold", reg_addr );
    }
    if (reg32_val & IN_STAT_MISC_OV_STAT_2) {
        fprintf (out_file_prt, "\n    reg %2u bit  2: R VS voltage is above the over-voltage condition threshold", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  2: R VS voltage is below the over-voltage condition threshold", reg_addr );
    }
    if (reg32_val & IN_STAT_MISC_TW_STAT_1) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  1: R Device junction temperature is above the temperature warning threshold TTW.", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  1: R Device junction temperature is below the temperature warning threshold TTW.", reg_addr );
    }
    if (reg32_val & IN_STAT_MISC_TSD_STAT_0) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  0:1 R Device junction temperature is above the temperature warning threshold TTW", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  0:0 R Device junction temperature is below the temperature warning threshold TTW", reg_addr );
    }

    return res;
}

static bool parse_tic12400_int_en_cfg0_register_24h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    uint32_t reserved;
    reserved = extract_subval_from_32bit (reg32_val, 23, 12);
    fprintf (out_file_prt, "\n    reg %2u bit 23-12: %u R Reserved", reg_addr, reserved);

    if (reg32_val & INT_EN_CFG0_ADC_DIAG_EN_11) {
        fprintf (out_file_prt, "\n    reg %2u bit 11:1 RW INT pin assertion due to ADC error enabled. ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 11:0 RW INT pin assertion due to ADC error disabled ", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_WET_DIAG_EN_10) {
        fprintf (out_file_prt, "\n    reg %2u bit 10:1 RW INT pin assertion due to wetting current error enabled", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 10:0 RW INT pin assertion due to wetting current error disabled. ", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_VS1_EN_9) {
        fprintf (out_file_prt, "\n    reg %2u bit  9:1 RW INT pin assertion due to VS1 threshold crossing enabled ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  9:0 RW INT pin assertion due to VS1 threshold crossing disabled. ", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_VS0_EN_8) {
        fprintf (out_file_prt, "\n    reg %2u bit  8:1 RW INT pin assertion due to VS0 threshold crossing enabled ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  8:0 RW INT pin assertion due to VS0 threshold crossing disabled ", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_CRC_CALC_EN_7) {
        fprintf (out_file_prt, "\n    reg %2u bit  7:1 RW INT pin assertion due to CRC calculation completion enabled  ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  7:0 RW INT pin assertion due to CRC calculation completion disabled ", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_UV_EN_6) {
        fprintf (out_file_prt, "\n    reg %2u bit  6:1 RW INT pin assertion due to UV event enabled. ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  6:0 RW INT pin assertion due to UV event disabled ", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_OV_EN_5) {
        fprintf (out_file_prt, "\n    reg %2u bit  5:1 RW INT pin assertion due to OV event enabled ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  5:0 RW INT pin assertion due to OV event disabled ", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_TW_EN_4) {
        fprintf (out_file_prt, "\n    reg %2u bit  4:1 RW INT pin assertion due to TW event enabled ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  4:0 RW INT pin assertion due to TW event disabled", reg_addr );
    }

    if (reg32_val & INT_EN_CFG0_TSD_EN_3) {
        fprintf (out_file_prt, "\n    reg %2u bit  3:1 RW INT pin assertion due to TSD event enabled  ", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  3:0 RW INT pin assertion due to TSD event disabled ", reg_addr );
    }

    if (reg32_val & INT_EN_CFG0_SSC_EN_2) {
        fprintf (out_file_prt, "\n    reg %2u bit  2:1 RW INT pin assertion due to SSC event enabled", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  2:0 RW INT pin assertion due to SSC event disabled", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_PRTY_FAIL_EN_1) {
        fprintf (out_file_prt, "\n    reg %2u bit  1:1 RW INT pin assertion due to parity fail event enabled.", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  1:0 RW INT pin assertion due to parity fail event disabled", reg_addr );
    }
    if (reg32_val & INT_EN_CFG0_SPI_FAIL_EN_0) {
        fprintf (out_file_prt, "\n    reg %2u bit  0:1 RW INT pin assertion due to SPI fail event enabled", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  0:0 RW INT pin assertion due to SPI fail event disabled.", reg_addr );
    }
    return true;
}


static bool parse_tic12400_int_en_cfg1_register_37 (uint32_t reg_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint16_t code = 0U;
    (void) code;
    if ( 37 == reg_addr ) {
        res = true;
    }
    if(0==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:0 RW no interrupt generation for IN11", reg_addr);
    }
    if(1==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:1 RW interrupt generation on rising edge above THRESx for IN11", reg_addr);
    }
    if(2==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:2 RW interrupt generation on falling edge below THRESx for IN11", reg_addr);
    }
    if(3==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:3 RW interrupt generation on falling and rising edge of THRESx for IN11", reg_addr);
    }
    if(0==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:0 RW no interrupt generation for IN10", reg_addr);
    }
    if(1==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:1 RW interrupt generation on rising edge above THRESx for IN10", reg_addr);
    }
    if(2==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:2 RW interrupt generation on falling edge below THRESx for IN10", reg_addr);
    }
    if(3==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:3 RW interrupt generation on falling and rising edge of THRESx for IN10", reg_addr);
    }
    if(0==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:0 RW no interrupt generation for IN9", reg_addr);
    }
    if(1==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:1 RW interrupt generation on rising edge above THRESx for IN9", reg_addr);
    }
    if(2==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:2 RW interrupt generation on falling edge below THRESx for IN9", reg_addr);
    }
    if(3==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:3 RW interrupt generation on falling and rising edge of THRESx for IN9", reg_addr);
    }
    if(0==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:0 RW no interrupt generation for IN8", reg_addr);
    }
    if(1==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:1 RW interrupt generation on rising edge above THRESx for IN8", reg_addr);
    }
    if(2==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:2 RW interrupt generation on falling edge below THRESx for IN8", reg_addr);
    }
    if(3==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:3 RW interrupt generation on falling and rising edge of THRESx for IN8", reg_addr);
    }
    if(0==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:0 RW no interrupt generation for IN7", reg_addr);
    }
    if(1==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:1 RW interrupt generation on rising edge above THRESx for IN7", reg_addr);
    }
    if(2==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:2 RW interrupt generation on falling edge below THRESx for IN7", reg_addr);
    }
    if(3==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:3 RW interrupt generation on falling and rising edge of THRESx for IN7", reg_addr);
    }
    if(0==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:0 RW no interrupt generation for IN6", reg_addr);
    }
    if(1==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:1 RW interrupt generation on rising edge above THRESx for IN6", reg_addr);
    }
    if(2==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:2 RW interrupt generation on falling edge below THRESx for IN6", reg_addr);
    }
    if(3==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:3 RW interrupt generation on falling and rising edge of THRESx for IN6", reg_addr);
    }
    if(0==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:0 RW no interrupt generation for IN5", reg_addr);
    }
    if(1==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:1 RW interrupt generation on rising edge above THRESx for IN5", reg_addr);
    }
    if(2==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:2 RW interrupt generation on falling edge below THRESx for IN5", reg_addr);
    }
    if(3==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:3 RW interrupt generation on falling and rising edge of THRESx for IN5", reg_addr);
    }
    return res;
}

static bool parse_tic12400_int_en_cfg2_register_38 (uint32_t reg_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint16_t code = 0U;
    (void) code;
    if ( 38 == reg_addr ) {
        res = true;
    }
    if(0==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:0 RW no interrupt generation for IN17 w.r.t. THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:1 RW interrupt generation on rising edge above THRES2B for IN17", reg_addr);
    }
    if(2==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:2 RW interrupt generation on falling edge below THRES2B for IN17", reg_addr);
    }
    if(3==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:3 RW interrupt generation on falling and rising edge of THRES2B for IN17", reg_addr);
    }
    if(0==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:0 RW no interrupt generation for IN17 w.r.t. THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:1 RW interrupt generation on rising edge above THRES2A for IN17", reg_addr);
    }
    if(2==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:2 RW interrupt generation on falling edge below THRES2A for IN17", reg_addr);
    }
    if(3==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:3 RW interrupt generation on falling and rising edge of THRES2A for IN17", reg_addr);
    }
    if(0==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:0 RW no interrupt generation for IN16 w.r.t. THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:1 RW interrupt generation on rising edge above THRES2B for IN16", reg_addr);
    }
    if(2==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:2 RW interrupt generation on falling edge below THRES2B for IN16", reg_addr);
    }
    if(3==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:3 RW interrupt generation on falling and rising edge of THRES2B for IN16", reg_addr);
    }
    if(0==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:0 RW no interrupt generation for IN16 w.r.t. THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:1 RW interrupt generation on rising edge above THRES2A for IN16", reg_addr);
    }
    if(2==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:2 RW interrupt generation on falling edge below THRES2A for IN16", reg_addr);
    }
    if(3==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:3 RW interrupt generation on falling and rising edge of THRES2A for IN16", reg_addr);
    }
    if(0==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:0 RW no interrupt generation for IN15 w.r.t. THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:1 RW interrupt generation on rising edge above THRES2B for IN15", reg_addr);
    }
    if(2==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:2 RW interrupt generation on falling edge below THRES2B for IN15", reg_addr);
    }
    if(3==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:3 RW interrupt generation on falling and rising edge of THRES2B for IN15", reg_addr);
    }
    if(0==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:0 RW no interrupt generation for IN15 w.r.t. THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:1 RW interrupt generation on rising edge above THRES2A for IN15", reg_addr);
    }
    if(2==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:2 RW interrupt generation on falling edge below THRES2A for IN15", reg_addr);
    }
    if(3==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:3 RW interrupt generation on falling and rising edge of THRES2A for IN15", reg_addr);
    }
    if(0==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:0 RW no interrupt generation for IN14 w.r.t. THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:1 RW interrupt generation on rising edge above THRES2B for IN14", reg_addr);
    }
    if(2==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:2 RW interrupt generation on falling edge below THRES2B for IN14", reg_addr);
    }
    if(3==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:3 RW interrupt generation on falling and rising edge of THRES2B for IN14", reg_addr);
    }
    if(0==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:0 RW no interrupt generation for IN14 w.r.t. THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:1 RW interrupt generation on rising edge above THRES2A for IN14", reg_addr);
    }
    if(2==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:2 RW interrupt generation on falling edge below THRES2A for IN14", reg_addr);
    }
    if(3==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:3 RW interrupt generation on falling and rising edge of THRES2A for IN14", reg_addr);
    }
    if(0==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:0 RW no interrupt generation for IN13 w.r.t. THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:1 RW interrupt generation on rising edge above THRES2B for IN13", reg_addr);
    }
    if(2==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:2 RW interrupt generation on falling edge below THRES2B for IN13", reg_addr);
    }
    if(3==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:3 RW interrupt generation on falling and rising edge of THRES2B for IN13", reg_addr);
    }
    if(0==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:0 RW no interrupt generation for IN13 w.r.t. THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:1 RW interrupt generation on rising edge above THRES2A for IN13", reg_addr);
    }
    if(2==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:2 RW interrupt generation on falling edge below THRES2A for IN13", reg_addr);
    }
    if(3==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:3 RW interrupt generation on falling and rising edge of THRES2A for IN13", reg_addr);
    }
    if(0==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:0 RW no interrupt generation for IN12 w.r.t. THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:1 RW interrupt generation on rising edge above THRES2B for IN12", reg_addr);
    }
    if(2==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:2 RW interrupt generation on falling edge below THRES2B for IN12", reg_addr);
    }
    if(3==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:3 RW interrupt generation on falling and rising edge of THRES2B for IN12", reg_addr);
    }
    if(0==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:0 RW no interrupt generation for IN12 w.r.t. THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:1 RW interrupt generation on rising edge above THRES2A for IN12", reg_addr);
    }
    if(2==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:2 RW interrupt generation on falling edge below THRES2A for IN12", reg_addr);
    }
    if(3==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:3 RW interrupt generation on falling and rising edge of THRES2A for IN12", reg_addr);
    }
    return res;
}

static bool parse_tic12400_int_en_cfg3_register_39 (uint32_t reg_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint16_t code = 0U;
    (void) code;
    if ( 39 == reg_addr ) {
        res = true;
    }
    if(0==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:0 RW no interrupt generation for IN21 w.r.t. THRES3C", reg_addr);
    }
    if(1==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:1 RW interrupt generation on rising edge above THRES3C for IN21", reg_addr);
    }
    if(2==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:2 RW interrupt generation on falling edge below THRES3C for IN21", reg_addr);
    }
    if(3==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:3 RW interrupt generation on falling and rising edge of THRES3C for IN21", reg_addr);
    }
    if(0==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:0 RW no interrupt generation for IN21 w.r.t. THRES3B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:1 RW interrupt generation on rising edge above THRES3B for IN21", reg_addr);
    }
    if(2==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:2 RW interrupt generation on falling edge below THRES3B for IN21", reg_addr);
    }
    if(3==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:3 RW interrupt generation on falling and rising edge of THRES3B for IN21", reg_addr);
    }
    if(0==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:0 RW no interrupt generation for IN21 w.r.t. THRES3A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:1 RW interrupt generation on rising edge above THRES3A for IN21", reg_addr);
    }
    if(2==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:2 RW interrupt generation on falling edge below THRES3A for IN21", reg_addr);
    }
    if(3==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:3 RW interrupt generation on falling and rising edge of THRES3A for IN21", reg_addr);
    }
    if(0==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:0 RW no interrupt generation for IN20 w.r.t. THRES3C", reg_addr);
    }
    if(1==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:1 RW interrupt generation on rising edge above THRES3C for IN20", reg_addr);
    }
    if(2==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:2 RW interrupt generation on falling edge below THRES3C for IN20", reg_addr);
    }
    if(3==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:3 RW interrupt generation on falling and rising edge of THRES3C for IN20", reg_addr);
    }
    if(0==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:0 RW no interrupt generation for IN20 w.r.t. THRES3B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:1 RW interrupt generation on rising edge above THRES3B for IN20", reg_addr);
    }
    if(2==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:2 RW interrupt generation on falling edge below THRES3B for IN20", reg_addr);
    }
    if(3==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:3 RW interrupt generation on falling and rising edge of THRES3B for IN20", reg_addr);
    }
    if(0==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:0 RW no interrupt generation for IN20 w.r.t. THRES3A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:1 RW interrupt generation on rising edge above THRES3A for IN20", reg_addr);
    }
    if(2==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:2 RW interrupt generation on falling edge below THRES3A for IN20", reg_addr);
    }
    if(3==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:3 RW interrupt generation on falling and rising edge of THRES3A for IN20", reg_addr);
    }
    if(0==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:0 RW no interrupt generation for IN19 w.r.t. THRES3C", reg_addr);
    }
    if(1==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:1 RW generation on rising edge above THRES3C for IN19", reg_addr);
    }
    if(2==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:2 RW interrupt generation on falling edge below THRES3C for IN19", reg_addr);
    }
    if(3==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:3 RW interrupt generation on falling and rising edge of THRES3C for IN19", reg_addr);
    }
    if(0==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:0 RW no interrupt generation for IN19 w.r.t. THRES3B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:1 RW interrupt generation on rising edge above THRES3B for IN19", reg_addr);
    }
    if(2==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:2 RW interrupt generation on falling edge below THRES3B for IN19", reg_addr);
    }
    if(3==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:3 RW interrupt generation on falling and rising edge of THRES3B for IN19", reg_addr);
    }
    if(0==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:0 RW no interrupt generation for IN19 w.r.t. THRES3A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:1 RW interrupt generation on rising edge above THRES3A for IN19", reg_addr);
    }
    if(2==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:2 RW interrupt generation on falling edge below THRES3A for IN19", reg_addr);
    }
    if(3==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:3 RW interrupt generation on falling and rising edge of THRES3A for IN19", reg_addr);
    }
    if(0==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:0 RW no interrupt generation for IN18 w.r.t. THRES3C", reg_addr);
    }
    if(1==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:1 RW interrupt generation on rising edge above THRES3C for IN18", reg_addr);
    }
    if(2==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:2 RW interrupt generation on falling edge below THRES3C for IN18", reg_addr);
    }
    if(3==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:3 RW interrupt generation on falling and rising edge of THRES3C for IN18", reg_addr);
    }
    if(0==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:0 RW no interrupt generation for IN18 w.r.t. THRES3B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:1 RW interrupt generation on rising edge above THRES3B for IN18", reg_addr);
    }
    if(2==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:2 RW interrupt generation on falling edge below THRES3B for IN18", reg_addr);
    }
    if(3==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:3 RW interrupt generation on falling and rising edge of THRES3B for IN18", reg_addr);
    }
    if(0==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:0 RW no interrupt generation for IN18 w.r.t. THRES3A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:1 RW interrupt generation on rising edge above THRES3A for IN18", reg_addr);
    }
    if(2==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:2 RW interrupt generation on falling edge below THRES3A for IN18", reg_addr);
    }
    if(3==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:3 RW interrupt generation on falling and rising edge of THRES3A for IN18", reg_addr);
    }
    return res;
}

static bool parse_tic12400_int_en_cfg4_register_40 (uint32_t reg_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint16_t code = 0U;
    (void) code;
    if ( 40 == reg_addr ) {
        res = true;
    }
    if(0==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:0 RW no interrupt generation for VS w.r.t. VS1_THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:1 RW generation on rising edge above VS1_THRES2B for VS", reg_addr);
    }
    if(2==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:2 RW interrupt generation on falling edge below VS1_THRES2B for VS", reg_addr);
    }
    if(3==(0x3 & (reg_val>>22))){
        fprintf (out_file_prt,"\n    reg %02u bit 23-22:3 RW interrupt generation on falling and rising edge of VS1_THRES2B for VS", reg_addr);
    }
    if(0==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:0 RW no interrupt generation for VS w.r.t. VS1_THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:1 RW generation on rising edge above VS1_THRES2A for VS", reg_addr);
    }
    if(2==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:2 RW interrupt generation on falling edge below VS1_THRES2A for VS", reg_addr);
    }
    if(3==(0x3 & (reg_val>>20))){
        fprintf (out_file_prt,"\n    reg %02u bit 21-20:3 RW interrupt generation on falling and rising edge of VS1_THRES2A for VS", reg_addr);
    }
    if(0==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:0 RW no interrupt generation for VS w.r.t. VS0_THRES2B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:1 RW interrupt generation on rising edge above VS0_THRES2B for VS", reg_addr);
    }
    if(2==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:2 RW interrupt generation on falling edge below VS0_THRES2B for VS", reg_addr);
    }
    if(3==(0x3 & (reg_val>>18))){
        fprintf (out_file_prt,"\n    reg %02u bit 19-18:3 RW interrupt generation on falling and rising edge of VS0_THRES2B for VS", reg_addr);
    }
    if(0==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:0 RW no interrupt generation for VS w.r.t. VS0_THRES2A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:1 RW interrupt generation on rising edge above VS0_THRES2A for VS", reg_addr);
    }
    if(2==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:2 RW interrupt generation on falling edge below VS0_THRES2A for VS", reg_addr);
    }
    if(3==(0x3 & (reg_val>>16))){
        fprintf (out_file_prt,"\n    reg %02u bit 17-16:3 RW interrupt generation on falling and rising edge of VS0_THRES2A for VS", reg_addr);
    }
    if(0==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:0 RW no interrupt generation for IN23 w.r.t. THRES9", reg_addr);
    }
    if(1==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:1 RW interrupt generation on rising edge above THRES9 for IN23", reg_addr);
    }
    if(2==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:2 RW interrupt generation on falling edge below THRES9 for IN23", reg_addr);
    }
    if(3==(0x3 & (reg_val>>14))){
        fprintf (out_file_prt,"\n    reg %02u bit 15-14:3 RW interrupt generation on falling and rising edge of THRES9 for IN23", reg_addr);
    }
    if(0==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:0 RW no interrupt generation for IN23 w.r.t. THRES8", reg_addr);
    }
    if(1==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:1 RW interrupt generation on rising edge above THRES8 for IN23", reg_addr);
    }
    if(2==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:2 RW interrupt generation on falling edge below THRES8 for IN23", reg_addr);
    }
    if(3==(0x3 & (reg_val>>12))){
        fprintf (out_file_prt,"\n    reg %02u bit 13-12:3 RW interrupt generation on falling and rising edge of THRES8 for IN23", reg_addr);
    }
    if(0==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:0 RW no interrupt generation for IN23 w.r.t. THRES3C", reg_addr);
    }
    if(1==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:1 RW interrupt generation on rising edge above THRES3C for IN23", reg_addr);
    }
    if(2==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:2 RW interrupt generation on falling edge below THRES3C for IN23", reg_addr);
    }
    if(3==(0x3 & (reg_val>>10))){
        fprintf (out_file_prt,"\n    reg %02u bit 11-10:3 RW interrupt generation on falling and rising edge of THRES3C for IN23", reg_addr);
    }
    if(0==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:0 RW no interrupt generation for IN23 w.r.t. THRES3B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:1 RW interrupt generation on rising edge above THRES3B for IN23", reg_addr);
    }
    if(2==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:2 RW interrupt generation on falling edge below THRES3B for IN23", reg_addr);
    }
    if(3==(0x3 & (reg_val>>8))){
        fprintf (out_file_prt,"\n    reg %02u bit  9- 8:3 RW interrupt generation on falling and rising edge of THRES3B for IN23", reg_addr);
    }
    if(0==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:0 RW no interrupt generation for IN23 w.r.t. THRES3A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:1 RW interrupt generation on rising edge above THRES3A for IN23", reg_addr);
    }
    if(2==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:2 RW interrupt generation on falling edge below THRES3A for IN23", reg_addr);
    }
    if(3==(0x3 & (reg_val>>6))){
        fprintf (out_file_prt,"\n    reg %02u bit  7- 6:3 RW interrupt generation on falling and rising edge of THRES3A for IN23", reg_addr);
    }
    if(0==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:0 RW no interrupt generation for IN22 w.r.t. THRES3C", reg_addr);
    }
    if(1==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:1 RW interrupt generation on rising edge above THRES3C for IN22", reg_addr);
    }
    if(2==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:2 RW interrupt generation on falling edge below THRES3C for IN22", reg_addr);
    }
    if(3==(0x3 & (reg_val>>4))){
        fprintf (out_file_prt,"\n    reg %02u bit  5- 4:3 RW interrupt generation on falling and rising edge of THRES3C for IN22", reg_addr);
    }
    if(0==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:0 RW no interrupt generation for IN22 w.r.t. THRES3B", reg_addr);
    }
    if(1==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:1 RW interrupt generation on rising edge above THRES3B for IN22", reg_addr);
    }
    if(2==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:2 RW interrupt generation on falling edge below THRES3B for IN22", reg_addr);
    }
    if(3==(0x3 & (reg_val>>2))){
        fprintf (out_file_prt,"\n    reg %02u bit  3- 2:3 RW interrupt generation on falling and rising edge of THRES3B for IN22", reg_addr);
    }
    if(0==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:0 RW no interrupt generation for IN22 w.r.t. THRES3A", reg_addr);
    }
    if(1==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:1 RW interrupt generation on rising edge above THRES3A for IN22", reg_addr);
    }
    if(2==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:2 RW interrupt generation on falling edge below THRES3A for IN22", reg_addr);
    }
    if(3==(0x3 & (reg_val>>0))){
        fprintf (out_file_prt,"\n    reg %02u bit  1- 0:3 RW interrupt generation on falling and rising edge of THRES3A for IN22", reg_addr);
    }
    return res;
}



static bool parse_tic12400_device_id_register_01h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint32_t reserv = 0;
    uint8_t minor = 0, major = 0;
    reserv = extract_subval_from_32bit (reg32_val, 23, 11);
    major = extract_subval_from_32bit (reg32_val, 10, 4);
    minor = extract_subval_from_32bit (reg32_val, 3, 0);

    fprintf (out_file_prt, "\n    reg %2u bit 23-11: R RESERVED %u ", reg_addr, reserv);
    fprintf (out_file_prt, "\n    reg %2u bit 10- 4: R major %u ", reg_addr, major);
    fprintf (out_file_prt, "\n    reg %2u bit  3- 0: R minor %u ", reg_addr, minor);

    if ((0 == minor) && (2 == major)) {
        res = true;
    } else {
        fprintf (out_file_prt, "\n It is not TIC12400-Q1 register  blob  device_id: %x%x is not 0x20\n ", major, minor);
        exit (1);
    }

    return res;
}

static bool parse_tic12400_thres_cfg0_register_29h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint16_t thres0, thres1, reserv;
    reserv = extract_subval_from_32bit (reg32_val, 23, 20);
    thres1 = extract_subval_from_32bit (reg32_val, 19, 10);
    thres0 = extract_subval_from_32bit (reg32_val, 9, 0);
    fprintf (out_file_prt, "\n    reg %2u bit 23-20: RESERVED %u ", reg_addr, reserv);
    fprintf (out_file_prt, "\n    reg %2u bit 19-10: THRES1 %u ", reg_addr, thres1);
    fprintf (out_file_prt, "\n    reg %2u bit  9- 0: THRES0 %u ", reg_addr, thres0);
    tic12400channelList[0].valueThreshold = thres0;
    tic12400channelList[1].valueThreshold = thres1;
    return res;
}

static bool parse_tic12400_thres_cfg1_register_2ah  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint16_t thres3, thres2, reserv;
    reserv = extract_subval_from_32bit (reg32_val, 23, 20);
    thres3 = extract_subval_from_32bit (reg32_val, 19, 10);
    thres2 = extract_subval_from_32bit (reg32_val, 9, 0);
    fprintf (out_file_prt, "\n    reg %2u bit 23-20: RESERVED %u ", reg_addr, reserv);
    fprintf (out_file_prt, "\n    reg %2u bit 19-10: THRES3 %u ", reg_addr, thres3);
    fprintf (out_file_prt, "\n    reg %2u bit  9- 0: THRES2 %u ", reg_addr, thres2);
    tic12400channelList[2].valueThreshold = thres2;
    tic12400channelList[3].valueThreshold = thres3;

    return res;
}

static bool parse_tic12400_thres_cfg2_register_2bh  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint16_t thres4, thres5, reserv;
    reserv = extract_subval_from_32bit (reg32_val, 23, 20);
    thres5 = extract_subval_from_32bit (reg32_val, 19, 10);
    thres4 = extract_subval_from_32bit (reg32_val, 9, 0);
    fprintf (out_file_prt, "\n    reg %2u bit 23-20: RESERVED %u ", reg_addr, reserv);
    fprintf (out_file_prt, "\n    reg %2u bit 19-10: THRES5 %u ", reg_addr, thres5);
    fprintf (out_file_prt, "\n    reg %2u bit  9- 0: THRES4 %u ", reg_addr, thres4);
    tic12400channelList[4].valueThreshold = thres4;
    tic12400channelList[5].valueThreshold = thres5;
    return res;
}

static bool parse_tic12400_thres_cfg3_register_2ch  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint16_t thres6, thres7, reserv;
    reserv = extract_subval_from_32bit (reg32_val, 23, 20);
    thres7 = extract_subval_from_32bit (reg32_val, 19, 10);
    thres6 = extract_subval_from_32bit (reg32_val, 9, 0);
    fprintf (out_file_prt, "\n    reg %2u bit 23-20: RESERVED %u ", reg_addr, reserv);
    fprintf (out_file_prt, "\n    reg %2u bit 19-10: THRES7 %u ", reg_addr, thres7);
    fprintf (out_file_prt, "\n    reg %2u bit  9- 0: THRES6 %u ", reg_addr, thres6);
    tic12400channelList[6].valueThreshold = thres6;
    tic12400channelList[7].valueThreshold = thres7;
    return res;
}

static bool parse_tic12400_ana_stat12_register_16h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    float vs_voltage = 0;
    uint16_t adc_self_ana, vs_ana, reserv;

    reserv = extract_subval_from_32bit (reg32_val, 32, 20);
    adc_self_ana = extract_subval_from_32bit (reg32_val, 19, 10);
    vs_ana = extract_subval_from_32bit (reg32_val, 9, 0);

    vs_voltage = calc_vs_voltage ();

    fprintf (out_file_prt, "\n    reg %2u bit 23-20: R RESERVED %u ", reg_addr, reserv);
    fprintf (out_file_prt, "\n    reg %2u bit 19-10: R adc_self_ana %u ", reg_addr, adc_self_ana);
    fprintf (out_file_prt, "\n    reg %2u bit  9- 0: R vs_ana %u vs %f V", reg_addr, vs_ana, vs_voltage);
    return res;
}

static bool parse_tic12400_thres_cfg4_register_2dh  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    uint16_t thres8, thres9, reserv;
    reserv = extract_subval_from_32bit (reg32_val, 23, 20);
    thres9 = extract_subval_from_32bit (reg32_val, 19, 10);
    thres8 = extract_subval_from_32bit (reg32_val, 9, 0);
    fprintf (out_file_prt, "\n    reg %2u bit 23-20: RESERVED %u ", reg_addr, reserv);
    fprintf (out_file_prt, "\n    reg %2u bit 19-10: THRES9 %u ", reg_addr, thres9);
    fprintf (out_file_prt, "\n    reg %2u bit  9- 0: THRES8 %u ", reg_addr, thres8);
    tic12400channelList[8].valueThreshold = thres8;
    tic12400channelList[9].valueThreshold = thres9;
    return res;
}

uint8_t inStatMatrix0InChanLUT[24] = {10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
                                      12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13};
//                                   0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
uint8_t inStatMatrix0InChan2LUT[24] = {4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9};

static bool parse_tic12400_in_stat_matrix0_register_08h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    if (NULL != out_file_prt) {
        int16_t bit = 0;
        for (bit = 23; 0 <= bit; bit--) {
            if (reg32_val & (1 << bit)) {
                fprintf (out_file_prt, "\n    reg %2u bit %2u:1 R Input IN%02u is above threshold while IN%u pulled to GND", reg_addr, bit,
                         inStatMatrix0InChanLUT[bit], inStatMatrix0InChan2LUT[bit]);
            } else {
                fprintf (out_file_prt, "\n    reg %2u bit %2u:0 R Input IN%02u is below threshold while IN%u pulled to GND", reg_addr, bit,
                         inStatMatrix0InChanLUT[bit], inStatMatrix0InChan2LUT[bit]);
            }
        }
        res = true;
    }

    return res;
}

uint8_t inStatMatrix1InChanLUT[24] = {14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15,
                                      0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11};
//    0   1   2   3   4   5   6   7   8   9   10  11  12 13 14 15 16 17 18 19 20 21 22 23
uint8_t inStatMatrix1InChan2LUT[24] = {4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9};

static bool parse_tic12400_in_stat_matrix1_register_09h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    if (NULL != out_file_prt) {
        int16_t bit = 0;
        for (bit = 23; 11 <= bit; bit--) {
            if (reg32_val & (1 << bit)) {
                fprintf (out_file_prt, "\n    reg %2u bit %2u:1 R Input IN%02u equal to or above threshold THRES_COM", reg_addr, bit,
                         inStatMatrix1InChanLUT[bit]);
            } else {
                fprintf (out_file_prt, "\n    reg %2u bit %2u:0 R Input IN%02u below threshold THRES_COM", reg_addr, bit,
                         inStatMatrix1InChanLUT[bit]);
            }
        }

        for (bit = 11; 0 <= bit; bit--) {
            if (reg32_val & (1 << bit)) {
                fprintf (out_file_prt, "\n    reg %2u bit %2u:1 R Input IN%02u is above threshold while IN%u pulled to GND", reg_addr, bit,
                         inStatMatrix1InChanLUT[bit], inStatMatrix1InChan2LUT[bit]);
            } else {
                fprintf (out_file_prt, "\n    reg %2u bit %2u:0 R Input IN%02u is below threshold while IN%u pulled to GND", reg_addr, bit,
                         inStatMatrix1InChanLUT[bit], inStatMatrix1InChan2LUT[bit]);
            }
        }

        res = true;
    }
    return res;
}

static bool parse_tic12400_cs_select_register_1ch  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;
    int8_t chan;
    for (chan = 9; 0 <= chan; chan--) {
        if (reg32_val && (1U << chan)) {
            tic12400channelList[chan].pullMode = 'D';
            fprintf (out_file_prt, "\n    reg %2u bit %u:1 RW Current Sink (CSI) selected", reg_addr, chan);
        } else {
            tic12400channelList[chan].pullMode = 'U';
            fprintf (out_file_prt, "\n    reg %2u bit %u:0 RW Current Source (CSO) selected", reg_addr, chan);
        }
    }
    uint8_t rchan;
    for (rchan = 10u; rchan < CHANNEL_AMOUNT; rchan++) {
        tic12400channelList[rchan].pullMode = 'U';
    }
    return res;
}

static bool parse_tic12400_config_register_1ah  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = true;

    if (reg32_val & CONFIG_VS_RATIO_23) {
        fprintf (out_file_prt, "\n    reg %2u bit 23: RW Use voltage divider factor of 10 for the VS measurement", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 23: RW Use voltage divider factor of 3 for the VS measurement", reg_addr );
    }

    if (reg32_val & ADC_DIAG_T_22) {
        fprintf (out_file_prt, "\n    reg %2u bit 22: RW Enable ADC self-diagnostic feature", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 22: RW Disable ADC self-diagnostic feature", reg_addr );
    }

    if (reg32_val & VS_MEAS_EN_17) {
        fprintf (out_file_prt, "\n    reg %2u bit 17: Enable VS measurement at the end of every polling cycle", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 17: Disable VS measurement at the end of every polling cycle", reg_addr );
    }

    if (reg32_val & TW_CUR_DIS_CSI_16) {
        fprintf (out_file_prt, "\n    reg %2u bit 16: Disable wetting current reduction (to 2 mA) for 10mA and 15mA settings upon "
                             "TW event for all inputs enabled with CSI.", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 16: Enable wetting current reduction (to 2 mA) for 10mA and 15mA settings upon "
                             "TW event for all inputs enabled with CSI", reg_addr );
    }

    if (reg32_val & TW_CUR_DIS_CSO_13) {
        fprintf (out_file_prt, "\n    reg %2u bit 13: RW  Disable wetting current reduction (to 2mA) for 10mA and 15mA settings "
                             "upon TW event for all inputs enabled with CSO", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 13: RW  Enable wetting current reduction (to 2mA) for 10mA and 15mA settings "
                             "upon TW event for all inputs enabled with CSO", reg_addr );
    }

    if (reg32_val & INT_CONFIG_12) {
        fprintf (out_file_prt, "\n    reg %2u bit 12: RW  INT pin assertion scheme set to dynamic", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 12: RW  INT pin assertion scheme set to static", reg_addr );
    }

    if (reg32_val & TRIGGER_11) {
        fprintf (out_file_prt, "\n    reg %2u bit 11: RW Trigger TIC12400-Q1 normal operation", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 11: RW Stop TIC12400-Q1 from normal operation", reg_addr );
    }

    if (reg32_val & POLL_EN_10) {
        fprintf (out_file_prt, "\n    reg %2u bit 10: RW Polling enabled and the device operates in one of the polling modes", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit 10: RW Polling disabled. Device operates in continuous mode", reg_addr );
    }

    if (reg32_val & CRC_T_9) {
        fprintf (out_file_prt, "\n    reg %2u bit  9: RW trigger CRC calculation", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  9: RW no CRC calculation triggered", reg_addr );
    }

    uint8_t pollActTime = extract_subval_from_32bit (reg32_val, 8, 5);
    parse_poll_act_time_val (pollActTime, out_file_prt, reg_addr);

    uint8_t pollTime = extract_subval_from_32bit (reg32_val, 4, 1);
    parse_poll_time_val (pollTime, out_file_prt, reg_addr);

    if (reg32_val & RESET_0) {
        fprintf (out_file_prt, "\n    reg %2u bit  0: Trigger software reset of the device.", reg_addr );
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit  0: No reset", reg_addr );
    }
    return res;
}


uint8_t getLowChannel[12] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22};
uint8_t getHiChannel[12] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23};

bool get_adc_channels (uint8_t reg_addr, uint8_t *lowChannel, uint8_t *hiChannel) {
    bool res = false;
    if ((REG_ANA_STAT0 <= reg_addr) && (reg_addr <= REG_ANA_STAT12)) {
        *lowChannel = getLowChannel[reg_addr - REG_ANA_STAT0];
        *hiChannel = getHiChannel[reg_addr - REG_ANA_STAT0];
        res = true;
    }
    return res;
}

static bool parse_tic12400_in_stat_adc0_register_06h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint8_t ina_7, ina_6, ina_5, ina_4, ina_3, ina_2, ina_1, ina_0;
    uint8_t ina_13, ina_12, ina_11, ina_10, ina_9, ina_8;
    uint8_t ina_17, ina_16, ina_15, ina_14;
    ina_17 = extract_subval_from_32bit (reg32_val, 23, 22);
    parse_in_a_code (ina_17, out_file_prt, 23, 22, 17, reg_addr);

    ina_16 = extract_subval_from_32bit (reg32_val, 21, 20);
    parse_in_a_code (ina_16, out_file_prt, 21, 20, 16, reg_addr);

    ina_15 = extract_subval_from_32bit (reg32_val, 19, 18);
    parse_in_a_code (ina_15, out_file_prt, 19, 18, 15, reg_addr);

    ina_14 = extract_subval_from_32bit (reg32_val, 17, 16);
    parse_in_a_code (ina_14, out_file_prt, 17, 16, 14, reg_addr);

    ina_13 = extract_subval_from_32bit (reg32_val, 15, 14);
    parse_in_a_code (ina_13, out_file_prt, 15, 14, 13, reg_addr);

    ina_12 = extract_subval_from_32bit (reg32_val, 13, 12);
    parse_in_a_code (ina_12, out_file_prt, 13, 12, 12, reg_addr);

    ina_11 = extract_subval_from_32bit (reg32_val, 11, 11);
    parse_in_a_code (ina_11, out_file_prt, 11, 11, 11, reg_addr);

    ina_10 = extract_subval_from_32bit (reg32_val, 10, 10);
    parse_in_a_code (ina_10, out_file_prt, 10, 10, 10, reg_addr);

    ina_9 = extract_subval_from_32bit (reg32_val, 9, 9);
    parse_in_a_code (ina_9, out_file_prt, 9, 9, 9, reg_addr);

    ina_8 = extract_subval_from_32bit (reg32_val, 8, 8);
    parse_in_a_code (ina_8, out_file_prt, 8, 8, 8, reg_addr);

    ina_7 = extract_subval_from_32bit (reg32_val, 7, 7);
    parse_in_a_code (ina_7, out_file_prt, 7, 7, 7, reg_addr);

    ina_6 = extract_subval_from_32bit (reg32_val, 6, 6);
    parse_in_a_code (ina_6, out_file_prt, 6, 6, 6, reg_addr);

    ina_5 = extract_subval_from_32bit (reg32_val, 5, 5);
    parse_in_a_code (ina_5, out_file_prt, 5, 5, 5, reg_addr);

    ina_4 = extract_subval_from_32bit (reg32_val, 4, 4);
    parse_in_a_code (ina_4, out_file_prt, 4, 4, 4, reg_addr);

    ina_3 = extract_subval_from_32bit (reg32_val, 3, 3);
    parse_in_a_code (ina_3, out_file_prt, 3, 3, 3, reg_addr);

    ina_2 = extract_subval_from_32bit (reg32_val, 2, 2);
    parse_in_a_code (ina_2, out_file_prt, 2, 2, 2, reg_addr);

    ina_1 = extract_subval_from_32bit (reg32_val, 1, 1);
    parse_in_a_code (ina_1, out_file_prt, 1, 1, 1, reg_addr);

    ina_0 = extract_subval_from_32bit (reg32_val, 0, 0);
    parse_in_a_code (ina_0, out_file_prt, 0, 0, 0, reg_addr);
    return res;
}

static bool parse_tic12400_in_stat_adc1_register_07h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint32_t reserved;
    uint8_t ina_18, ina_19, ina_20, ina_21, ina_22, ina_23;
    reserved = extract_subval_from_32bit (reg32_val, 23, 13);
    fprintf (out_file_prt, "\n    reg %2u bit 23-13:%5u R Reserved", reg_addr, reserved);

    ina_23 = extract_subval_from_32bit (reg32_val, 12, 10);
    parse_in_a_code (ina_23, out_file_prt, 12, 10, 23, reg_addr);

    ina_22 = extract_subval_from_32bit (reg32_val, 9, 8);
    parse_in_a_code (ina_22, out_file_prt, 9, 8, 22, reg_addr);

    ina_21 = extract_subval_from_32bit (reg32_val, 7, 6);
    parse_in_a_code (ina_21, out_file_prt, 7, 6, 21, reg_addr);

    ina_20 = extract_subval_from_32bit (reg32_val, 5, 4);
    parse_in_a_code (ina_20, out_file_prt, 5, 4, 20, reg_addr);

    ina_19 = extract_subval_from_32bit (reg32_val, 3, 2);
    parse_in_a_code (ina_19, out_file_prt, 3, 2, 19, reg_addr);

    ina_18 = extract_subval_from_32bit (reg32_val, 1, 0);
    parse_in_a_code (ina_18, out_file_prt, 1, 0, 18, reg_addr);

    return res;
}

static bool parse_tic12400_ana_statX_register_XXh (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    uint32_t reserved;
    uint16_t ana2nd, ana1st;
    uint8_t lowChannel, hiChannel;
    res = get_adc_channels (reg_addr, &lowChannel, &hiChannel);
    if (true == res) {
        reserved = extract_subval_from_32bit (reg32_val, 23, 20);
        fprintf (out_file_prt, "\n    reg %2u bit 23-20:%5u R Reserved",reg_addr, reserved);

        ana2nd = extract_subval_from_32bit (reg32_val, 19, 10);
        fprintf (out_file_prt, "\n    reg %2u bit 19-10:%5u R  10-bits value of IN%u",reg_addr, ana2nd, hiChannel);
        tic12400channelList[hiChannel].valueVoltage = ana2nd;

        ana1st = extract_subval_from_32bit (reg32_val, 9, 0);
        fprintf (out_file_prt, "\n    reg %2u bit  9- 0:%5u R  10-bits value of IN%u",reg_addr, ana1st, lowChannel);
        tic12400channelList[ana1st].valueVoltage = ana1st;
    }

    return res;
}

static bool parse_tic12400_mode_bit (uint32_t reg32_val, uint8_t bitNum, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;

    if (reg32_val & (1 << bitNum)) {
        strncpy (tic12400channelList[bitNum].inMode, "ADC", sizeof (tic12400channelList[bitNum].inMode));
        fprintf (out_file_prt, "\n    reg %2u bit %02u: ADC mode for IN%d", reg_addr, bitNum, bitNum);
    } else {
        strncpy (tic12400channelList[bitNum].inMode, "CMP", sizeof (tic12400channelList[bitNum].inMode));
        fprintf (out_file_prt, "\n    reg %2u bit %02u: comparator mode for IN%d", reg_addr, bitNum, bitNum);
    }
    return res;
}

static bool parse_tic12400_mode_register_32h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_mode_bit (reg32_val, bitNum, out_file_prt, reg_addr);
    }

    return res;
}


static bool parse_tic12400_ccp_cfg1_bit (uint32_t reg32_val, uint8_t bitNum, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    if (reg32_val & (1U << bitNum)) {
        fprintf (out_file_prt, "\n    reg %2u bit %02u: clean current polling wetting current activated CCP_IN%d", reg_addr, bitNum, bitNum);
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit %02u: no clean current polling wetting current CCP_IN%d", reg_addr, bitNum, bitNum);
    }
    return res;
}

static bool parse_tic12400_n_stat_comp_bit (uint32_t reg32_val, uint8_t bitNum, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    if (reg32_val & (1U << bitNum)) {
        fprintf (out_file_prt, "\n    reg %2u bit %02u: Input IN%02u is above the comparator threshold.", reg_addr, bitNum, bitNum);
    } else {
        fprintf (out_file_prt, "\n    reg %2u bit %02u: Input IN%02u is below the comparator threshold.", reg_addr, bitNum, bitNum);
    }
    return res;
}

static bool parse_tic12400_in_en_bit (uint32_t reg32_val, uint8_t bitNum, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    if (reg32_val & (1U << bitNum)) {
        tic12400channelList[bitNum].state = true;
        fprintf (out_file_prt, "\n    reg %2u bit %02u: Input channel IN%u enabled", reg_addr, bitNum, bitNum);
    } else {
        tic12400channelList[bitNum].state = false;
        fprintf (out_file_prt, "\n    reg %2u bit %02u: Input channel IN%u disabled. Polling sequence skips this channel", reg_addr, bitNum,
                 bitNum);
    }
    return res;
}

static bool parse_tic12400_in_stat_comp_register_05h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_n_stat_comp_bit (reg32_val, bitNum, out_file_prt, reg_addr);
    }
    return res;
}

static bool parse_tic12400_in_en_register_1bh  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_in_en_bit (reg32_val, bitNum, out_file_prt, reg_addr);
    }
    return res;
}

static bool parse_ccp_time (uint8_t ccp_time, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    if (NULL != out_file_prt) {
        res = true;
        switch (ccp_time) {
        case 0:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 64 us",
					 reg_addr, ccp_time);
            break;
        case 1:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 128 us",
					 reg_addr, ccp_time);
            break;
        case 2:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 192 us",
					 reg_addr, ccp_time);
            break;
        case 3:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 256 us",
					 reg_addr, ccp_time);
            break;
        case 4:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 320 us",
					 reg_addr, ccp_time);
            break;
        case 5:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 384 us",
					 reg_addr, ccp_time);
            break;
        case 6:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 448 us",
					 reg_addr, ccp_time);
            break;
        case 7:
            fprintf (out_file_prt,
                     "\n    reg %2u bit  6- 4:%u RW Wetting current activation time in CCP (Clean Current Polling) mode 512 us",
					 reg_addr, ccp_time);
            break;
        default:
            res = false;
            fprintf (out_file_prt, "\n    reg %2u bit  6- 4:%u RW UNDEF VALUE ERROR!", reg_addr, ccp_time);
            break;
        }
    }
    return res;
}

static bool parse_tic12400_ccp_cfg0_register_1fh  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    (void)reg32_val;
    (void)out_file_prt;
    uint32_t reserved;
    reserved = extract_subval_from_32bit (reg32_val, 23, 7);
    fprintf (out_file_prt, "\n    reg %2u bit 23- 7:%u R Reserved",reg_addr,  reserved);
    uint32_t ccp_time;
    ccp_time = extract_subval_from_32bit (reg32_val, 6, 4);
    parse_ccp_time (ccp_time, out_file_prt, reg_addr);

    if (reg32_val & CCP_CFG0_WC_CCP3) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  3:1 Wetting current setting for IN18 to IN23 in CCP mode 15mA (typ.) wetting current", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  3:0 Wetting current setting for IN18 to IN23 in CCP mode 10mA (typ.) wetting current", reg_addr );
    };
    if (reg32_val & CCP_CFG0_WC_CCP2) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  2:1 Wetting current setting for IN12 to IN17 in CCP mode 15mA (typ.) wetting current", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  2:0 Wetting current setting for IN12 to IN17 in CCP mode 10mA (typ.) wetting current", reg_addr );
    };
    if (reg32_val & CCP_CFG0_WC_CCP1) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  1:1 Wetting current setting for IN6 to IN11 in CCP mode 15mA (typ.) wetting current", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  1:0 Wetting current setting for IN6 to IN11 in CCP mode 10mA (typ.) wetting current", reg_addr );
    };
    if (reg32_val & CCP_CFG0_WC_CCP0) {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  0:1 Wetting current setting for IN0 to IN5 in CCP mode 15mA (typ.) wetting current", reg_addr );
    } else {
        fprintf (out_file_prt,
                 "\n    reg %2u bit  0:0 Wetting current setting for IN0 to IN5 in CCP mode 10mA (typ.) wetting current", reg_addr );
    };

    bool res = false;
    return res;
}

static bool parse_tic12400_ccp_cfg1_register_20h  (uint32_t reg32_val, FILE *out_file_prt, uint8_t reg_addr) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_ccp_cfg1_bit (reg32_val, bitNum, out_file_prt, reg_addr);
    }
    return res;
}

bool parse_tic12400_reg (uint8_t reg_addr, FILE *out_file_prt) {
    bool res = false;

    if(false==tic12400RegMap[reg_addr].valid){
    	printf("\nRegister %u=0x%x is not valid\n",reg_addr, reg_addr);
    	return false;
    }
    uint32_t reg32_val = tic12400RegMap[reg_addr].reg24BitVal;
    tic12400RegMap[reg_addr].reg24BitVal = reg32_val;
    fprintf (out_file_prt, "\n\nreg [%s] addr 0x%02x val 0x%06x 0b_%s", tic12400_reg_2_name[reg_addr], reg_addr, reg32_val,
             utoa_bin24 (reg32_val));
    switch (reg_addr) {
    case REG_DEVICE_ID:
        procRegCnd++;
        res = parse_tic12400_device_id_register_01h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_INT_STAT:
        procRegCnd++;
        res = parse_tic12400_interrupt_status_register_02h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_CRC:
        procRegCnd++;
        res = parse_tic12400_crc_register_03h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_IN_STAT_MISC:
        procRegCnd++;
        res = parse_tic12400_in_stat_misc_register_04h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_IN_STAT_COMP:
        procRegCnd++;
        parse_tic12400_in_stat_comp_register_05h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_IN_STAT_ADC0:
        procRegCnd++;
        res = parse_tic12400_in_stat_adc0_register_06h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_IN_STAT_ADC1:
        procRegCnd++;
        res = parse_tic12400_in_stat_adc1_register_07h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_IN_STAT_MATRIX0:
        procRegCnd++;
        res = parse_tic12400_in_stat_matrix0_register_08h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_IN_STAT_MATRIX1:
        procRegCnd++;
        res = parse_tic12400_in_stat_matrix1_register_09h (reg32_val, out_file_prt, reg_addr);
        break;

    case REG_CS_SELECT:
        procRegCnd++;
        res = parse_tic12400_cs_select_register_1ch (reg32_val, out_file_prt, reg_addr);
        break;

    case REG_ANA_STAT12:
        procRegCnd++;
        res = parse_tic12400_ana_stat12_register_16h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_CONFIG:
        procRegCnd++;
        res = parse_tic12400_config_register_1ah (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_IN_EN:
        procRegCnd++;
        parse_tic12400_in_en_register_1bh (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_WC_CFG0:
        procRegCnd++;
        res = parse_tic12400_wc_cfg0_register_1dh (reg32_val, out_file_prt, reg_addr);
        break;

    case REG_WC_CFG1:
        procRegCnd++;
        res = parse_tic12400_wc_cfg1_register_1eh (reg32_val, out_file_prt, reg_addr);
        break;

    case REG_CCP_CFG0:
        procRegCnd++;
        res = parse_tic12400_ccp_cfg0_register_1fh (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_CCP_CFG1:
        procRegCnd++;
        res = parse_tic12400_ccp_cfg1_register_20h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_THRES_COMP:
        procRegCnd++;
        res = parse_tic12400_thres_comp_register_21h (reg32_val, out_file_prt, reg_addr);
        break;

    case REG_INT_EN_COMP1:
        procRegCnd++;
        res = parse_tic12400_int_en_comp1_register_22h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_INT_EN_COMP2:
        procRegCnd++;
        res = parse_tic12400_int_en_comp2_register_23h (reg32_val, out_file_prt, reg_addr);
        break;

    case REG_THRES_CFG0:
        procRegCnd++;
        res = parse_tic12400_thres_cfg0_register_29h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_THRES_CFG1:
        procRegCnd++;
        res = parse_tic12400_thres_cfg1_register_2ah (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_THRES_CFG2:
        procRegCnd++;
        res = parse_tic12400_thres_cfg2_register_2bh (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_INT_EN_CFG0:
        procRegCnd++;
        res = parse_tic12400_int_en_cfg0_register_24h (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_INT_EN_CFG1:
    	procRegCnd++;
        res = parse_tic12400_int_en_cfg1_register_37 (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_INT_EN_CFG2:
    	procRegCnd++;
        res = parse_tic12400_int_en_cfg2_register_38 (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_INT_EN_CFG3:
    	procRegCnd++;
        res = parse_tic12400_int_en_cfg3_register_39 (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_INT_EN_CFG4:
    	procRegCnd++;
        res = parse_tic12400_int_en_cfg4_register_40 (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_THRES_CFG3:
        procRegCnd++;
        res = parse_tic12400_thres_cfg3_register_2ch (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_THRES_CFG4:
        procRegCnd++;
        res = parse_tic12400_thres_cfg4_register_2dh (reg32_val, out_file_prt, reg_addr);
        break;
    case REG_ANA_STAT0:
    case REG_ANA_STAT1:
    case REG_ANA_STAT2:
    case REG_ANA_STAT3:
    case REG_ANA_STAT4:
    case REG_ANA_STAT5:
    case REG_ANA_STAT6:
    case REG_ANA_STAT7:
    case REG_ANA_STAT8:
    case REG_ANA_STAT9:
    case REG_ANA_STAT10:
    case REG_ANA_STAT11:
        procRegCnd++;
        res = parse_tic12400_ana_statX_register_XXh (reg32_val, out_file_prt, reg_addr);
        break;

    case REG_THRESMAP_CFG0:
    	procRegCnd++;
    	parse_tic12400_thresmap_cfg0_register_46 (reg32_val, out_file_prt, reg_addr);
    	break;
    case REG_THRESMAP_CFG1:
    	procRegCnd++;
    	parse_tic12400_thresmap_cfg1_register_47 (reg32_val, out_file_prt, reg_addr);
    	break;
    case REG_THRESMAP_CFG2:
    	procRegCnd++;
    	parse_tic12400_thresmap_cfg2_register_48 (reg32_val, out_file_prt, reg_addr);
    	break;

    case REG_MATRIX:
        procRegCnd++;
        parse_tic12400_matrix_register_49 (reg32_val, out_file_prt, reg_addr);
	break;
    case REG_MODE:
        procRegCnd++;
        res = parse_tic12400_mode_register_32h (reg32_val, out_file_prt, reg_addr);
        break;
    default:
        fprintf (out_file_prt, "\n Lack of parser for reg [%s] addr 0x%x val 0x%x", tic12400_reg_2_name[reg_addr], reg_addr,
                 reg32_val);
        break;
    }
    return res;
}
