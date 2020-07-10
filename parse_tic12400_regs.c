#include "parse_tic12400_regs.h"

#include "tic12400_bits_offsets.h"
#include "utils.h"
#include "str_ops.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

Tic12400Channel_t tic12400channelList [CHANNEL_AMOUNT];
Tic12400Reg_t tic12400RegMap [REG_AMOUNT];

bool parse_tic12400_regs_file (char *inFileName, char *outFileName) {
    char curFileStr [500];
    bool res = false;
    FILE *inFilePrt = NULL;
    printf ("\n>In file: [%s]", inFileName);
    inFilePrt = fopen (inFileName, "r");
    if (inFilePrt) {
        int line = 0;
        while (NULL != fgets (curFileStr, sizeof(curFileStr), inFilePrt)) {
            unsigned int regAddr;
            unsigned int regVal;
#if DEBUG_IN_FILE_STR
            printf ("\n>[%s]", curFileStr);
#endif
            sscanf (curFileStr, "%x %x", (unsigned int *) &regAddr, (unsigned int *) &regVal);
            if((REG_ADDR_LEN+REG_VAL_LEN)<strlen(curFileStr)){

            uint8_t reg8bitAddr;
            uint32_t reg32bitVal;
            res = try_canch_hex_uint8 (curFileStr, strlen (curFileStr), &reg8bitAddr);
            if (true == res) {
                res = try_canch_hex_uint32 (curFileStr, strlen (curFileStr), &reg32bitVal);
                if (true == res) {
                    load_tic12400_reg (reg8bitAddr, reg32bitVal);
                } else {
                    printf ("\n Unable to parse 24 bit reg val in [%s]",curFileStr);
                }
            } else {
                printf ("\n Unable to parse 8 bit reg addr in [%s]",curFileStr);
            }
            }
#if DEBUG_IN_FILE_STR
            printf ("\n[%x] [%x]", regAddr, regVal);
#endif
            line++;
        }
        fclose (inFilePrt);
        res = true;
    } else {
        printf ("\nUnable to open file [%s]", inFileName);
    }

    FILE *outFilePrt = NULL;
    outFilePrt = fopen (outFileName, "w");
    if (outFilePrt) {
        if (true == res) {
            uint8_t reg_8bit_addr;
            for (reg_8bit_addr = 0; reg_8bit_addr <= REG_MODE; reg_8bit_addr++) {
                parse_tic12400_reg (reg_8bit_addr, outFilePrt);
            }
            write_channel_table (outFilePrt);

            fclose (outFilePrt);
        }
    }

    return res;
}

char *tic12400_reg_2_name [51] =
    { "   REG_RESERVED_00H", //0
        "      REG_DEVICE_ID",
        "       REG_INT_STAT",
        "            REG_CRC",
        "   REG_IN_STAT_MISC",
        "   REG_IN_STAT_COMP",
        "   REG_IN_STAT_ADC0",
        "   REG_IN_STAT_ADC1",
        "REG_IN_STAT_MATRIX0",
        "REG_IN_STAT_MATRIX1",
        "      REG_ANA_STAT0", //10
        "      REG_ANA_STAT1",
        "      REG_ANA_STAT2",
        "      REG_ANA_STAT3",
        "      REG_ANA_STAT4",
        "      REG_ANA_STAT5",
        "      REG_ANA_STAT6",
        "      REG_ANA_STAT7",
        "      REG_ANA_STAT8",
        "      REG_ANA_STAT9",
        "     REG_ANA_STAT10", //20
        "     REG_ANA_STAT11",
        "     REG_ANA_STAT12",
        "   REG_RESERVED_17H",
        "   REG_RESERVED_18H",
        "   REG_RESERVED_19H",
        "         REG_CONFIG",
        "         REG_IN_EN",
        "     REG_CS_SELECT",
        "       REG_WC_CFG0",
        "       REG_WC_CFG1", //30
        "      REG_CCP_CFG0",
        "      REG_CCP_CFG1",
        "    REG_THRES_COMP",
        "  REG_INT_EN_COMP1",
        "  REG_INT_EN_COMP2",
        "   REG_INT_EN_CFG0",
        "   REG_INT_EN_CFG1",
        "   REG_INT_EN_CFG2",
        "   REG_INT_EN_CFG3",
        "   REG_INT_EN_CFG4", //40
        "    REG_THRES_CFG0",
        "    REG_THRES_CFG1",
        "    REG_THRES_CFG2",
        "    REG_THRES_CFG3",
        "    REG_THRES_CFG4",
        " REG_THRESMAP_CFG0",
        " REG_THRESMAP_CFG1",
        " REG_THRESMAP_CFG2",
        "        REG_MATRIX",
        "          REG_MODE" //50
        };

bool load_tic12400_reg (uint8_t regAddr, uint32_t regVal) {
    bool res = false;
    tic12400RegMap [regAddr].reg24BitVal = regVal;
    tic12400RegMap [regAddr].regName = tic12400_reg_2_name [regAddr];
    return res;
}

bool write_channel_table (FILE *outFilePrt) {
    bool res = false;
    uint8_t channel;
    if (NULL != outFilePrt) {
        res = true;
        fprintf (outFilePrt, " \n");
        fprintf (outFilePrt, "+----+-----+------+------+----+\n");
        fprintf (outFilePrt, "| ch | wc  | Mode | pull | En |\n");
        fprintf (outFilePrt, "+----+-----+------+------+----+\n");
        for (channel = 0; channel < CHANNEL_AMOUNT; channel++) {
            fprintf (
                outFilePrt,
                "| %02u | %2.1f |  %s |  %c   |  %u |\n",
                channel,
                tic12400channelList [channel].wettingCurrent,
                tic12400channelList [channel].inMode,
                tic12400channelList [channel].pullMode,
                tic12400channelList [channel].state);

        }
        fprintf (outFilePrt, "+----+-----+------+------+----+\n");
        fprintf (outFilePrt, " \n");
    }
    return res;
}

bool parse_tic12400_reg (uint8_t regAddr, FILE *outFilePrt) {
    bool res = false;
    uint32_t regVal = tic12400RegMap [regAddr].reg24BitVal;
    tic12400RegMap [regAddr].reg24BitVal = regVal;
    fprintf (outFilePrt, "\n\nreg %s addr 0x%02x val 0x%06x 0b_%s", tic12400_reg_2_name [regAddr], regAddr, regVal, utoa_bin24 (regVal));
    switch (regAddr) {
        case REG_CONFIG:
            res = parse_tic12400_config_register_1ah (regVal, outFilePrt);
            break;
        case REG_CS_SELECT:
            res = parse_tic12400_cs_select_register_1ch (regVal, outFilePrt);
            break;

        case REG_DEVICE_ID:
            res = parse_tic12400_device_id_register_01h (regVal, outFilePrt);
            break;
        case REG_INT_STAT:
            res = parse_tic12400_interrupt_status_register_02h (regVal, outFilePrt);
            break;

        case REG_THRES_COMP:
            break;

        case REG_CRC:
            res = parse_tic12400_crc_register_03h (regVal, outFilePrt);
            break;

        case REG_IN_STAT_MISC:
            res = parse_tic12400_in_stat_misc_register_04h (regVal, outFilePrt);
            break;

        case REG_WC_CFG0:
            res = parse_tic12400_wc_cfg0_register_1dh (regVal, outFilePrt);
            break;

        case REG_WC_CFG1:
            res = parse_tic12400_wc_cfg1_register_1eh (regVal, outFilePrt);
            break;

        case REG_THRES_CFG0:
            res = parse_tic12400_thres_cfg0_register_29h (regVal, outFilePrt);
            break;
        case REG_THRES_CFG1:
            res = parse_tic12400_thres_cfg1_register_2ah (regVal, outFilePrt);
            break;
        case REG_THRES_CFG2:
            res = parse_tic12400_thres_cfg2_register_2bh (regVal, outFilePrt);
            break;
        case REG_THRES_CFG3:
            res = parse_tic12400_thres_cfg3_register_2ch (regVal, outFilePrt);
            break;
        case REG_THRES_CFG4:
            res = parse_tic12400_thres_cfg4_register_2dh (regVal, outFilePrt);
            break;

        case REG_ANA_STAT12:
            res = parse_tic12400_ana_stat12_register_16h (regVal, outFilePrt);
            break;

        case REG_MODE:
            res = parse_tic12400_mode_register_32h (regVal, outFilePrt);
            break;
        case REG_CCP_CFG0:
            res = parse_tic12400_ccp_cfg0_register_1fh (regVal, outFilePrt);
            break;
        case REG_CCP_CFG1:
            res = parse_tic12400_ccp_cfg1_register_20h (regVal, outFilePrt);
            break;
        case REG_IN_STAT_COMP:
            parse_tic12400_in_stat_comp_register_05h (regVal, outFilePrt);
            break;
        case REG_IN_EN:
            parse_tic12400_in_en_register_1bh (regVal, outFilePrt);
            break;
        default:
#if DPLOY_LACK
            fprintf (outFilePrt, "\n Lack of parser for reg %s addr 0x%x val 0x%x", tic12400_reg_2_name [regAddr], regAddr, regVal);
#endif
            break;
    }
    return res;
}

bool parse_tic12400_crc_register_03h (uint32_t regVal, FILE *outFilePrt) {
    (void) regVal;
    (void) outFilePrt;
    bool res = true;
    return res;
}

bool parse_tic12400_interrupt_status_register_02h (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    if (regVal & INT_STAT_CHK_FAIL_13) {
        fprintf (outFilePrt, "\n bit 13:1 RC An error is detected when loading factory settings into the device upon device initialization");
    } else {
        fprintf (outFilePrt, "\n bit 13:0 RC Default factory setting is successfully loaded upon device initialization");
    }
    if (regVal & INT_STAT_ADC_DIAG_12) {
        fprintf (outFilePrt, "\n bit 12:1 RC ADC self-diagnostic error is detected");
    } else {
        fprintf (outFilePrt, "\n bit 12:0 RC No ADC self-diagnostic error is detected");
    }
    if (regVal & INT_STAT_WET_DIAG_11) {
        fprintf (outFilePrt, "\n  bit 11:1 RC Wetting current error is detected ");
    } else {
        fprintf (outFilePrt, "\n  bit 11:0 RC No wetting current error is detected ");
    }
    if (regVal & INT_STAT_VS1_10) {
        fprintf (outFilePrt, "\n  bit 10:1 RC VS voltage state change occurred with respect to VS1_THRES2A or VS1_THRES2B ");
    } else {
        fprintf (outFilePrt, "\n  bit 10:0 RC No VS voltage state change occurred with respect to VS1_THRES2A or VS1_THRES2B ");
    }
    if (regVal & INT_STAT_VS0_9) {
        fprintf (outFilePrt, "\n  bit  9:1 RC VS voltage state change occurred with respect to VS0_THRES2A or VS0_THRES2B");
    } else {
        fprintf (outFilePrt, "\n  bit  9:0 RC No VS voltage state change occurred with respect to VS0_THRES2A or VS0_THRES2B ");
    }
    if (regVal & INT_STAT_CRC_CALC_8) {
        fprintf (outFilePrt, "\n  bit  8:1 RC CRC calculation is finished ");
    } else {
        fprintf (outFilePrt, "\n  bit  8:0 RC CRC calculation is running, not started ");
    }
    if (regVal & INT_STAT_UV_7) {
        fprintf (outFilePrt, "\n  bit  7:1 RC Under-voltage condition occurred or cleared on the VS pin");
    } else {
        fprintf (outFilePrt, "\n  bit  7:0 RC No under-voltage condition occurred");
    }
    if (regVal & INT_STAT_OV_6) {
        fprintf (outFilePrt, "\n  bit  6:1 RC Over-voltage condition occurred or cleared on the VS pin ");
    } else {
        fprintf (outFilePrt, "\n  bit  6:0 RC No over-voltage condition occurred or cleared on the VS pin,");
    }

    if (regVal & INT_STAT_TW_5) {
        fprintf (outFilePrt, "\n  bit  5:1 RC Temperature warning event occurred or cleared ");
    } else {
        fprintf (outFilePrt, "\n  bit  5:0 RC No temperature warning event occurred ");
    }
    if (regVal & INT_STAT_TSD_4) {
        fprintf (outFilePrt, "\n  bit  4:1 RC Temperature Shutdown event occurred or cleared ");
    } else {
        fprintf (outFilePrt, "\n  bit  4:0 RC No temperature Shutdown event occurred ");
    }
    if (regVal & INT_STAT_SSC_3) {
        fprintf (outFilePrt, "\n  bit  3:1 RC Switch state change occurred ");
    } else {
        fprintf (outFilePrt, "\n  bit  3:0 RC No switch state change occurred ");
    }
    if (regVal & INT_STAT_PRTY_FAIL_2) {
        fprintf (outFilePrt, "\n  bit  2:1 RC Parity error occurred");
    } else {
        fprintf (outFilePrt, "\n  bit  2:0 RC No parity error occurred in the last received SI stream");
    }

    if (regVal & INT_STAT_SPI_FAIL_1) {
        fprintf (outFilePrt, "\n  bit  1:1 RC SPI error occurred ");
    } else {
        fprintf (outFilePrt, "\n  bit  1:0 RC 32 clock pulse during a CS = low sequence was detected");
    }
    if (regVal & INT_STAT_POR_0) {
        fprintf (outFilePrt, "\n  bit  0:1 RC Power-On-Reset (POR) event occurred");
    } else {
        fprintf (outFilePrt, "\n  bit  0:0 RC no Power-On-Reset (POR) event occurred");
    }

    return res;
}

bool parse_tic12400_wc_cfg0_register_1dh (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint32_t wc_in0_in1;
    uint32_t wc_in2_in3;
    uint32_t wc_in4;
    uint32_t wc_in5;
    uint32_t wc_in6_in7;
    uint32_t wc_in8_in9;
    uint32_t wc_in10;
    uint32_t wc_in11;
    wc_in11 = extract_subval_from_32bit (regVal, 23, 21);
    parse_wetting_current (wc_in11, outFilePrt, 23, 21);
    tic12400channelList [11].wettingCurrent = num2wett_current (wc_in11);

    wc_in10 = extract_subval_from_32bit (regVal, 20, 18);
    parse_wetting_current (wc_in10, outFilePrt, 20, 18);
    tic12400channelList [10].wettingCurrent = num2wett_current (wc_in10);

    wc_in8_in9 = extract_subval_from_32bit (regVal, 17, 15);
    parse_wetting_current (wc_in8_in9, outFilePrt, 17, 15);
    tic12400channelList [8].wettingCurrent = num2wett_current (wc_in8_in9);
    tic12400channelList [9].wettingCurrent = num2wett_current (wc_in8_in9);

    wc_in6_in7 = extract_subval_from_32bit (regVal, 14, 12);
    parse_wetting_current (wc_in6_in7, outFilePrt, 14, 12);
    tic12400channelList [6].wettingCurrent = num2wett_current (wc_in6_in7);
    tic12400channelList [7].wettingCurrent = num2wett_current (wc_in6_in7);

    wc_in5 = extract_subval_from_32bit (regVal, 11, 9);
    parse_wetting_current (wc_in5, outFilePrt, 11, 9);
    tic12400channelList [5].wettingCurrent = num2wett_current (wc_in5);

    wc_in4 = extract_subval_from_32bit (regVal, 8, 6);
    parse_wetting_current (wc_in4, outFilePrt, 8, 6);
    tic12400channelList [4].wettingCurrent = num2wett_current (wc_in4);

    wc_in2_in3 = extract_subval_from_32bit (regVal, 5, 3);
    parse_wetting_current (wc_in2_in3, outFilePrt, 5, 3);
    tic12400channelList [2].wettingCurrent = num2wett_current (wc_in2_in3);
    tic12400channelList [3].wettingCurrent = num2wett_current (wc_in2_in3);

    wc_in0_in1 = extract_subval_from_32bit (regVal, 2, 0);
    parse_wetting_current (wc_in0_in1, outFilePrt, 2, 0);
    tic12400channelList [0].wettingCurrent = num2wett_current (wc_in0_in1);
    tic12400channelList [1].wettingCurrent = num2wett_current (wc_in0_in1);

    return res;
}

bool parse_tic12400_wc_cfg1_register_1eh (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint8_t reserved;
    uint8_t wc_in23;
    uint8_t wc_in22;
    uint8_t wc_in20_in21;
    uint8_t wc_in18_in19;
    uint8_t wc_in16_in17;
    uint8_t wc_in14_in15;
    uint8_t wc_in12_in13;

    reserved = extract_subval_from_32bit (regVal, 23, 22);
    fprintf (outFilePrt, "\n  bit  23:22: %u", reserved);

    if (regVal & WC_CFG1_AUTO_SCALE_DIS_CSI_22) {
        fprintf (
            outFilePrt,
            "\n  bit  22:1 RW Disable wetting current auto-scaling (to 2mA) in continuous mode for 10mA and 15mA settings upon switch closure for all inputs enabled with CS");
    } else {
        fprintf (
            outFilePrt,
            "\n  bit  22:0 RW Enable wetting current auto-scaling (to 2mA) in continuous mode for 10mA and 15mA settings upon switch closure for all inputs enabled with CSI");
    }

    if (regVal & WC_CFG1_AUTO_SCALE_DIS_CSO_21) {
        fprintf (
            outFilePrt,
            "\n  bit  21:1 RW Disable wetting current auto-scaling (to 2mA) in continuous mode for 10mA and 15mA settings upon switch closure for all inputs enabled with CSO");
    } else {
        fprintf (
            outFilePrt,
            "\n  bit  21:0 RW Enable wetting current auto-scaling (to 2mA) in continuous mode for 10mA and 15mA settings upon switch closure for all inputs enabled with CSO");
    }

    wc_in23 = extract_subval_from_32bit (regVal, 20, 18);
    parse_wetting_current (wc_in23, outFilePrt, 20, 18);
    tic12400channelList [23].wettingCurrent = num2wett_current (wc_in23);

    wc_in22 = extract_subval_from_32bit (regVal, 17, 15);
    parse_wetting_current (wc_in22, outFilePrt, 17, 15);
    tic12400channelList [22].wettingCurrent = num2wett_current (wc_in22);

    wc_in20_in21 = extract_subval_from_32bit (regVal, 14, 12);
    parse_wetting_current (wc_in20_in21, outFilePrt, 14, 12);
    tic12400channelList [20].wettingCurrent = num2wett_current (wc_in20_in21);
    tic12400channelList [21].wettingCurrent = num2wett_current (wc_in20_in21);

    wc_in18_in19 = extract_subval_from_32bit (regVal, 11, 9);
    parse_wetting_current (wc_in18_in19, outFilePrt, 11, 9);
    tic12400channelList [18].wettingCurrent = num2wett_current (wc_in18_in19);
    tic12400channelList [19].wettingCurrent = num2wett_current (wc_in18_in19);

    wc_in16_in17 = extract_subval_from_32bit (regVal, 8, 6);
    parse_wetting_current (wc_in16_in17, outFilePrt, 8, 6);
    tic12400channelList [16].wettingCurrent = num2wett_current (wc_in16_in17);
    tic12400channelList [17].wettingCurrent = num2wett_current (wc_in16_in17);

    wc_in14_in15 = extract_subval_from_32bit (regVal, 5, 3);
    parse_wetting_current (wc_in14_in15, outFilePrt, 5, 3);
    tic12400channelList [14].wettingCurrent = num2wett_current (wc_in14_in15);
    tic12400channelList [15].wettingCurrent = num2wett_current (wc_in14_in15);

    wc_in12_in13 = extract_subval_from_32bit (regVal, 2, 0);
    parse_wetting_current (wc_in12_in13, outFilePrt, 2, 0);
    tic12400channelList [12].wettingCurrent = num2wett_current (wc_in12_in13);
    tic12400channelList [13].wettingCurrent = num2wett_current (wc_in12_in13);

    return res;
}

bool parse_tic12400_in_stat_misc_register_04h (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint8_t vs1_stat = 0;
    uint8_t vs0_stat = 0;

    if (regVal & IN_STAT_MISC_ADC_D_12) {
        fprintf (outFilePrt, "\n  bit 12: R An error is identified from ADC self-diagnostic.");
    } else {
        fprintf (outFilePrt, "\n  bit 12: R No error is identified from ADC self-diagnostic.");
    }
    if (regVal & IN_STAT_MISC_IN3_D_11) {
        fprintf (outFilePrt, "\n  bit 11: R Current sink on IN3 is abnormal");
    } else {
        fprintf (outFilePrt, "\n  bit 11: R Current sink on IN3 is operational");
    }
    if (regVal & IN_STAT_MISC_IN2_D_10) {
        fprintf (outFilePrt, "\n  bit 10: R Current sink on IN2 is abnormal");
    } else {
        fprintf (outFilePrt, "\n  bit 10: R Current sink on IN2 is operational");
    }
    if (regVal & IN_STAT_MISC_IN1_D_9) {
        fprintf (outFilePrt, "\n  bit 9: R Current source on IN1 is abnormal");
    } else {
        fprintf (outFilePrt, "\n  bit 9: R Current source on IN1 is operational");
    }
    if (regVal & IN_STAT_MISC_IN0_D_8) {
        fprintf (outFilePrt, "\n  bit 8: R Current source on IN0 is abnormal");
    } else {
        fprintf (outFilePrt, "\n  bit 8: R Current source on IN0 is operational");
    }
    vs1_stat = extract_subval_from_32bit (regVal, 7, 6);
    parse_tic12400_vs1_stat (vs1_stat, outFilePrt);
    vs0_stat = extract_subval_from_32bit (regVal, 5, 4);
    parse_tic12400_vs0_stat (vs0_stat, outFilePrt);

    if (regVal & IN_STAT_MISC_UV_STAT_3) {
        fprintf (outFilePrt, "\n  bit 3: R VS voltage is below the under-voltage condition threshold");
    } else {
        fprintf (outFilePrt, "\n  bit 3: R VS voltage is above the under-voltage condition threshold");
    }
    if (regVal & IN_STAT_MISC_OV_STAT_2) {
        fprintf (outFilePrt, "\n  bit 2: R VS voltage is above the over-voltage condition threshold");
    } else {
        fprintf (outFilePrt, "\n  bit 2: R VS voltage is below the over-voltage condition threshold");
    }
    if (regVal & IN_STAT_MISC_TW_STAT_1) {
        fprintf (outFilePrt, "\n  bit 1: R Device junction temperature is above the temperature warning threshold TTW.");
    } else {
        fprintf (outFilePrt, "\n  bit 1: R Device junction temperature is below the temperature warning threshold TTW.");
    }
    if (regVal & IN_STAT_MISC_TSD_STAT_0) {
        fprintf (outFilePrt, "\n  bit 0: R Device junction temperature is above the temperature warning threshold TTW");
    } else {
        fprintf (outFilePrt, "\n  bit 0: R Device junction temperature is below the temperature warning threshold TTW");
    }

    return res;
}

bool parse_tic12400_device_id_register_01h (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint32_t reserv = 0;
    uint8_t minor = 0;
    uint8_t major = 0;
    reserv = extract_subval_from_32bit (regVal, 23, 11);
    major = extract_subval_from_32bit (regVal, 10, 4);
    minor = extract_subval_from_32bit (regVal, 3, 0);

    fprintf (outFilePrt, "\n  bit 23-11: R RESERVED %u ", reserv);
    fprintf (outFilePrt, "\n  bit 10-4: R major %u ", major);
    fprintf (outFilePrt, "\n  bit 3-0: R  minor %u ", minor);
    return res;
}

bool parse_tic12400_thres_cfg0_register_29h (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint16_t thres0, thres1, reserv;
    reserv = extract_subval_from_32bit (regVal, 23, 20);
    thres1 = extract_subval_from_32bit (regVal, 19, 10);
    thres0 = extract_subval_from_32bit (regVal, 9, 0);
    fprintf (outFilePrt, "\n  bit 23-20: RESERVED %u ", reserv);
    fprintf (outFilePrt, "\n  bit 19-10: THRES1 %u ", thres1);
    fprintf (outFilePrt, "\n  bit 9-0:   THRES0 %u ", thres0);
    return res;
}

bool parse_tic12400_thres_cfg1_register_2ah (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint16_t thres3, thres2, reserv;
    reserv = extract_subval_from_32bit (regVal, 23, 20);
    thres3 = extract_subval_from_32bit (regVal, 19, 10);
    thres2 = extract_subval_from_32bit (regVal, 9, 0);
    fprintf (outFilePrt, "\n  bit 23-20: RESERVED %u ", reserv);
    fprintf (outFilePrt, "\n  bit 19-10: THRES3 %u ", thres3);
    fprintf (outFilePrt, "\n  bit 9-0:   THRES2 %u ", thres2);
    return res;
}

bool parse_tic12400_thres_cfg2_register_2bh (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint16_t thres4, thres5, reserv;
    reserv = extract_subval_from_32bit (regVal, 23, 20);
    thres5 = extract_subval_from_32bit (regVal, 19, 10);
    thres4 = extract_subval_from_32bit (regVal, 9, 0);
    fprintf (outFilePrt, "\n  bit 23-20: RESERVED %u ", reserv);
    fprintf (outFilePrt, "\n  bit 19-10: THRES5 %u ", thres5);
    fprintf (outFilePrt, "\n  bit 9-0:   THRES4 %u ", thres4);
    return res;
}

bool parse_tic12400_thres_cfg3_register_2ch (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint16_t thres6, thres7, reserv;
    reserv = extract_subval_from_32bit (regVal, 23, 20);
    thres7 = extract_subval_from_32bit (regVal, 19, 10);
    thres6 = extract_subval_from_32bit (regVal, 9, 0);
    fprintf (outFilePrt, "\n  bit 23-20: RESERVED %u ", reserv);
    fprintf (outFilePrt, "\n  bit 19-10: THRES7 %u ", thres7);
    fprintf (outFilePrt, "\n  bit 9-0:   THRES6 %u ", thres6);
    return res;
}

bool parse_tic12400_ana_stat12_register_16h (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    float vs_voltage = 0;
    uint16_t adc_self_ana, vs_ana, reserv;

    reserv = extract_subval_from_32bit (regVal, 32, 20);
    adc_self_ana = extract_subval_from_32bit (regVal, 19, 10);
    vs_ana = extract_subval_from_32bit (regVal, 9, 0);

    vs_voltage = calc_vs_voltage ();

    fprintf (outFilePrt, "\n  bit 23-20: R RESERVED %u ", reserv);
    fprintf (outFilePrt, "\n  bit 19-10: R adc_self_ana %u ", adc_self_ana);
    fprintf (outFilePrt, "\n  bit   9-0: R vs_ana %u vs %f V", vs_ana, vs_voltage);
    return res;
}

bool parse_tic12400_thres_cfg4_register_2dh (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    uint16_t thres8, thres9, reserv;
    reserv = extract_subval_from_32bit (regVal, 23, 20);
    thres9 = extract_subval_from_32bit (regVal, 19, 10);
    thres8 = extract_subval_from_32bit (regVal, 9, 0);
    fprintf (outFilePrt, "\n  bit 23-20: RESERVED %u ", reserv);
    fprintf (outFilePrt, "\n  bit 19-10: THRES9 %u ", thres9);
    fprintf (outFilePrt, "\n  bit 9-0:   THRES8 %u ", thres8);
    return res;
}

bool parse_tic12400_cs_select_register_1ch (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    int8_t chan;
    for (chan = 9; 0 <= chan; chan--) {
        if (regVal && (1U << chan)) {
            tic12400channelList [chan].pullMode = 'D';
            fprintf (outFilePrt, "\n  bit %u:1 RW Current Sink (CSI) selected", chan);
        } else {
            tic12400channelList [chan].pullMode = 'U';
            fprintf (outFilePrt, "\n  bit %u:0 RW Current Source (CSO) selected", chan);
        }
    }
    uint8_t rchan;
    for (rchan = 10u; rchan < CHANNEL_AMOUNT; rchan++) {
        tic12400channelList [rchan].pullMode = 'U';
    }
    return res;
}

bool parse_tic12400_config_register_1ah (uint32_t regVal, FILE *outFilePrt) {
    bool res = true;
    if (regVal & RESET_0) {
        fprintf (outFilePrt, "\n  bit 0: Trigger software reset of the device.");
    } else {
        fprintf (outFilePrt, "\n  bit 0: No reset");
    }

    if (regVal & CONFIG_VS_RATIO_23) {
        fprintf (outFilePrt, "\n  bit 23: RW Use voltage divider factor of 10 for the VS measurement");
    } else {
        fprintf (outFilePrt, "\n  bit 23: RW Use voltage divider factor of 3 for the VS measurement");
    }

    if (regVal & ADC_DIAG_T_22) {
        fprintf (outFilePrt, "\n  bit 22: RW Enable ADC self-diagnostic feature");
    } else {
        fprintf (outFilePrt, "\n  bit 22: RW Disable ADC self-diagnostic feature");
    }

    if (regVal & VS_MEAS_EN_17) {
        fprintf (outFilePrt, "\n  bit 17: Enable VS measurement at the end of every polling cycle");
    } else {
        fprintf (outFilePrt, "\n  bit 17: Disable VS measurement at the end of every polling cycle");
    }

    if (regVal & TW_CUR_DIS_CSI_16) {
        fprintf (
            outFilePrt,
            "\n  bit 16: Disable wetting current reduction (to 2 mA) for 10mA and 15mA settings upon TW event for all inputs enabled with CSI.");
    } else {
        fprintf (
            outFilePrt,
            "\n  bit 16: Enable wetting current reduction (to 2 mA) for 10mA and 15mA settings upon TW event for all inputs enabled with CSI");
    }

    if (regVal & TW_CUR_DIS_CSO_13) {
        fprintf (
            outFilePrt,
            "\n  bit 13: RW  Disable wetting current reduction (to 2mA) for 10mA and 15mA settings upon TW event for all inputs enabled with CSO");
    } else {
        fprintf (
            outFilePrt,
            "\n  bit 13: RW  Enable wetting current reduction (to 2mA) for 10mA and 15mA settings upon TW event for all inputs enabled with CSO");
    }

    if (regVal & INT_CONFIG_12) {
        fprintf (outFilePrt, "\n  bit 12: RW  INT pin assertion scheme set to dynamic");
    } else {
        fprintf (outFilePrt, "\n  bit 12: RW  INT pin assertion scheme set to static");
    }

    if (regVal & TRIGGER_11) {
        fprintf (outFilePrt, "\n  bit 11: RW Trigger TIC12400-Q1 normal operation");
    } else {
        fprintf (outFilePrt, "\n  bit 11: RW Stop TIC12400-Q1 from normal operation");
    }

    if (regVal & POLL_EN_10) {
        fprintf (outFilePrt, "\n  bit 10: RW Polling enabled and the device operates in one of the polling modes");
    } else {
        fprintf (outFilePrt, "\n  bit 10: RW Polling disabled. Device operates in continuous mode");
    }

    if (regVal & CRC_T_9) {
        fprintf (outFilePrt, "\n  bit 9: RW trigger CRC calculation");
    } else {
        fprintf (outFilePrt, "\n  bit 9: RW no CRC calculation triggered");
    }

    uint8_t pollActTime = extract_subval_from_32bit (regVal, 8, 5);
    parse_poll_act_time_val (pollActTime, outFilePrt);

    uint8_t pollTime = extract_subval_from_32bit (regVal, 4, 1);
    parse_poll_time_val (pollTime, outFilePrt);
    return res;
}

bool parse_poll_act_time_val (uint8_t pollTime, FILE *outFilePrt) {
    bool res = true;
    switch (pollTime) {
        case 0x0:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 64us");
            break;
        case 0x1:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 128us");
            break;
        case 0x2:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 192us");
            break;
        case 0x3:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 256us");
            break;
        case 0x4:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 320us");
            break;
        case 0x5:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 384us");
            break;
        case 0x6:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 448us");
            break;
        case 0x7:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 512us");
            break;
        case 0x8:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 640us");
            break;
        case 0x9:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 768us");
            break;
        case 0xA:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 896us");
            break;
        case 0xB:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 1024us");
            break;
        case 0xC:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 2048us");
            break;
        default:
            fprintf (outFilePrt, "\n  bit 8-5: Poll Act time 512us ");
            break;
    }
    return res;
}

bool parse_poll_time_val (uint8_t pollTime, FILE *outFilePrt) {
    bool res = true;
    switch (pollTime) {
        case 0x0:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 2ms");
            break;
        case 0x1:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 4ms");
            break;
        case 0x2:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 8ms");
            break;
        case 0x3:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 16ms");
            break;
        case 0x4:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 32ms");
            break;
        case 0x5:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 48ms");
            break;
        case 0x6:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 64ms");
            break;
        case 0x7:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 128ms");
            break;
        case 0x8:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 256ms");
            break;
        case 0x9:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 512ms");
            break;
        case 0xA:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 1024ms");
            break;
        case 0xB:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 2048ms");
            break;
        case 0xC:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 4096ms");
            break;
        default:
            fprintf (outFilePrt, "\n  bit 4-1: Poll time 2ms");
            break;
    }
    return res;
}

bool parse_tic12400_mode_register_32h (uint32_t regVal, FILE *outFilePrt) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_mode_bit (regVal, bitNum, outFilePrt);
    }

    return res;
}

bool parse_tic12400_mode_bit (uint32_t regVal, uint8_t bitNum, FILE *outFilePrt) {
    bool res = false;

    if (regVal & (1 << bitNum)) {
        strncpy (tic12400channelList [bitNum].inMode, "ADC", sizeof(tic12400channelList [bitNum].inMode));
        fprintf (outFilePrt, "\n  bit %02u: ADC mode for IN%d", bitNum, bitNum);
    } else {
        strncpy (tic12400channelList [bitNum].inMode, "CMP", sizeof(tic12400channelList [bitNum].inMode));
        fprintf (outFilePrt, "\n  bit %02u: comparator mode for IN%d", bitNum, bitNum);
    }
    return res;
}

bool parse_tic12400_ccp_cfg1_bit (uint32_t regVal, uint8_t bitNum, FILE *outFilePrt) {
    bool res = false;
    if (regVal & (1U << bitNum)) {
        fprintf (outFilePrt, "\n  bit %02u: clean current polling wetting current activated CCP_IN%d", bitNum, bitNum);
    } else {
        fprintf (outFilePrt, "\n  bit %02u: no clean current polling wetting current CCP_IN%d", bitNum, bitNum);
    }
    return res;
}

bool parse_tic12400_n_stat_comp_bit (uint32_t regVal, uint8_t bitNum, FILE *outFilePrt) {
    bool res = false;
    if (regVal & (1U << bitNum)) {
        fprintf (outFilePrt, "\n  bit %02u: Input IN%02u is above the comparator threshold.", bitNum, bitNum);
    } else {
        fprintf (outFilePrt, "\n  bit %02u: Input IN%02u is below the comparator threshold.", bitNum, bitNum);
    }
    return res;
}

bool parse_tic12400_in_en_bit (uint32_t regVal, uint8_t bitNum, FILE *outFilePrt) {
    bool res = false;
    if (regVal & (1U << bitNum)) {
        tic12400channelList [bitNum].state = true;
        fprintf (outFilePrt, "\n  bit %02u: Input channel IN%u enabled", bitNum, bitNum);
    } else {
        tic12400channelList [bitNum].state = false;
        fprintf (outFilePrt, "\n  bit %02u: Input channel IN%u disabled. Polling sequence skips this channel", bitNum, bitNum);
    }
    return res;
}

bool parse_tic12400_ccp_cfg0_register_1fh (uint32_t regVal, FILE *outFilePrt) {
    (void) regVal;
    (void) outFilePrt;
    bool res = false;
    return res;
}

bool parse_tic12400_in_stat_comp_register_05h (uint32_t regVal, FILE *outFilePrt) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_n_stat_comp_bit (regVal, bitNum, outFilePrt);
    }
    return res;
}

bool parse_tic12400_ccp_cfg1_register_20h (uint32_t regVal, FILE *outFilePrt) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_ccp_cfg1_bit (regVal, bitNum, outFilePrt);
    }
    return res;
}

bool parse_tic12400_in_en_register_1bh (uint32_t regVal, FILE *outFilePrt) {
    bool res = false;
    int8_t bitNum;
    for (bitNum = 23; 0 <= bitNum; bitNum--) {
        parse_tic12400_in_en_bit (regVal, bitNum, outFilePrt);
    }
    return res;
}

bool parse_tic12400_vs1_stat (uint8_t val, FILE *outFilePrt) {
    bool res = true;
    switch (val) {
        case 0:
            fprintf (outFilePrt, "\n  bit 7-6: R VS voltage is below threshold VS1_THRES2A");
            break;
        case 1:
            fprintf (outFilePrt, "\n  bit 7-6: R VS voltage is below threshold VS1_THRES2B and equal to or above threshold VS1_THRES2A");
            break;
        case 2:
            fprintf (outFilePrt, "\n  bit 7-6: R VS voltage is equal to or above threshold VS1_THRES2B");
            break;
        case 3:
            fprintf (outFilePrt, "\n  bit 7-6: R N/A ");
            break;
    }
    return res;
}

bool parse_tic12400_vs0_stat (uint8_t val, FILE *outFilePrt) {
    bool res = false;
    switch (val) {
        case 0:
            fprintf (outFilePrt, "\n  bit 5-4: R VS voltage is below threshold VS0_THRES2A ");
            break;
        case 1:
            fprintf (outFilePrt, "\n  bit 5-4: R VS voltage is below threshold VS0_THRES2B and equal to or above threshold VS0_THRES2A");
            break;
        case 2:
            fprintf (outFilePrt, "\n  bit 5-4: R VS voltage is equal to or above threshold VS0_THRES2B");
            break;
        case 3:
            fprintf (outFilePrt, "\n  bit 5-4: R N/A ");
            break;
    }
    return res;
}

float calc_vs_voltage (void) {
    float vs_voltage = 0;
    float one_bit_voltage = 0;
    uint16_t vs_ana;
    if (tic12400RegMap [REG_CONFIG].reg24BitVal & CONFIG_VS_RATIO_23) {
        one_bit_voltage = 30.0 / 1024.0;
    } else {
        one_bit_voltage = 9.0 / 1024.0;
    }
    vs_ana = extract_subval_from_32bit (tic12400RegMap [REG_ANA_STAT12].reg24BitVal, 9, 0);
    vs_voltage = one_bit_voltage * ((float) vs_ana);
    return vs_voltage;
}

bool parse_wetting_current (uint8_t reg_val, FILE *outFilePrt, uint8_t oldBit, uint8_t littleBit) {
    bool res = true;
    switch (reg_val) {
        case 0:
            fprintf (outFilePrt, "\n  bit %u-%u:%u R/W no wetting current", oldBit, littleBit, reg_val);
            break;
        case 1:
            fprintf (outFilePrt, "\n  bit %u-%u:%u R/W 1mA (typ.) wetting current", oldBit, littleBit, reg_val);
            break;
        case 2:
            fprintf (outFilePrt, "\n  bit %u-%u:%u R/W 2mA (typ.) wetting current", oldBit, littleBit, reg_val);
            break;
        case 3:
            fprintf (outFilePrt, "\n  bit %u-%u:%u R/W 5mA (typ.) wetting current", oldBit, littleBit, reg_val);
            break;
        case 4:
            fprintf (outFilePrt, "\n  bit %u-%u:%u R/W 10mA (typ.) wetting current", oldBit, littleBit, reg_val);
            break;
        default:
            fprintf (outFilePrt, "\n  bit %u-%u:%u R/W 15mA (typ.) wetting current", oldBit, littleBit, reg_val);
            break;
    }
    return res;
}

float num2wett_current (uint8_t wcCode) {
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

