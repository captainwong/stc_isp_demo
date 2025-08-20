/**
 * @file stc8h.h
 * @brief Header file for STC8H series microcontroller.
 * @author captainwong (1281261856#qq.com)
 * @date 2025-08-15
 * @note Change log:
 *   - 2025-8-18 fill up sfr/xsfr
 */

#ifndef __STC8H_H__
#define __STC8H_H__

#include <libemb/emb_config.h>

#ifdef __C51__
#include <absacc.h>
#include <intrins.h>
#endif

//////////////////// SFRs ////////////////////

declare_sfr(0x80, P0);   // 8 bit Port0, Reset Value = 1111,1111
declare_sfr(0x81, SP);   // Stack Pointer, Reset Value = 0000,0011
declare_sfr(0x82, DPL);  // DPTR low byte, Reset value = 0000,0000
declare_sfr(0x83, DPH);  // DPTR high byte, Reset value = 0000,0000

declare_sfr(0x84, S4CON);  // Serial 4 Control, Reset Value = 0000,0000
#define S4SM0 0x80         // S4 mode select, 0: 8-bit brtx, 1: 9-bit brtx
#define S4ST4 0x40         // S4 brt select, 0: T2 as brt, 1: T4 as brt
#define S4SM2 0x20         // S4 在 mode 1 9-bit brtx 模式下多机通信控制位
#define S4REN 0x10         // S4 Receive Enable
#define S4TB8 0x08         // S4 Transmit Bit 8
#define S4RB8 0x04         // S4 Receive Bit 8
#define S4TI 0x02          // S4 Transmit Interrupt
#define S4RI 0x01          // S4 Receive Interrupt

declare_sfr(0x85, S4BUF);  // Serial 4 Buffer, Reset Value = 0000,0000

declare_sfr(0x87, PCON);  // Power Control, Reset Value = 0011,0000
#define SMOD 0x80         // 串口1波特率控制位
                          /* 0：各个模式的波特率都不加倍；
                           * 1：模式1（T1 MODE2作为BRT时有效）、模式2、模式3（T1 MODE2作为BRT时有效）的波特率加倍
                           */
#define SMOD0 0x40        // 帧错误检测控制位，0：禁止帧错误检测；1：允许帧错误检测，此时SCON的SM0/FE为FE功能，即帧错误检测标志位
#define LVDF 0x20         // 低压检测中断请求标志位. 需要软件清零
#define POF 0x10          // 上电标志位，硬件自动置位
#define GF1 0x08          // 通用标志位1
#define GF0 0x04          // 通用标志位0
#define PD 0x02           // Power Down Mode Enable, 主时钟停振、省电模式、掉电模式、停电模式
                          /* 0：无影响
                           * 1：单片机进入主时钟停振、省电模式、掉电模式、停电模式，CPU以及全部外设均停止工作. 唤醒后硬件自动清零.
                           *    省电模式、主时钟停电模式，俗称掉电模式：
                           *    仅主时钟停止震荡，依赖主时钟工作的CPU及大部分外设功能模块无时钟无法继续工作，
                           *    耗电的这部分功能模块工作暂停后，MCU几乎没有功耗.
                           *    不依赖主时钟的低功耗外设可以继续工作，如：
                           *    RTC若选择的是外部32768晶振，RTC会继续正常工作，仅需1.5uA电流.
                           *    段码LCD，如需继续工作，功耗也极低，不含屏的功耗，也仅需1.5uA电流.
                           */
#define IDL 0x01          // Idle Mode
                          /* 0: 无影响
                           * 1：单片机进入IDLE模式，只有CPU停止工作，其他外设依然在运行. 唤醒后硬件自动清零.
                           */

declare_sfr(0x88, TCON);     // Timer Control, Reset Value = 0000,0000
declare_sbit(TCON, 7, TF1);  // Timer1 overflow
declare_sbit(TCON, 6, TR1);  // Timer1 run control
declare_sbit(TCON, 5, TF0);  // Timer0 overflow
declare_sbit(TCON, 4, TR0);  // Timer0 run control
declare_sbit(TCON, 3, IE1);  // External Interrupt 1 edge flag
declare_sbit(TCON, 2, IT1);  // External Interrupt 1 type
declare_sbit(TCON, 1, IE0);  // External Interrupt 0 edge flag
declare_sbit(TCON, 0, IT0);  // External Interrupt 0 type

declare_sfr(0x89, TMOD);  // Timer Mode, Reset Value = 0000,0000
#define T1_GATE 0x80      // 控制定时器1，置1时只有在INT1脚为高且TR1=1时才可打开定时器/计数器1
#define T1_CT 0x40        // 定时器1工作方式，置0时T1为定时器，置1时T1为计数器(对引脚P3.5外部脉冲计数)
#define T1_M1 0x20        // 定时器1工作方式1
#define T1_M0 0x10        // 定时器1工作方式0
#define T0_GATE 0x08      // 控制定时器0，置1时只有在INT0脚为高且TR0=1时才可打开定时器/计数器0
#define T0_CT 0x04        // 定时器0工作方式，置0时T0为定时器，置1时T0为计数器(对引脚P3.4外部脉冲计数)
#define T0_M1 0x02        // 定时器0工作方式1
#define T0_M0 0x01        // 定时器0工作方式0

declare_sfr(0x8A, TL0);  // Timer 0 Low Byte, Reset Value = 0000,0000
declare_sfr(0x8B, TL1);  // Timer 1 Low Byte, Reset Value = 0000,0000
declare_sfr(0x8C, TH0);  // Timer 0 High Byte, Reset Value = 0000,0000
declare_sfr(0x8D, TH1);  // Timer 1 High Byte, Reset Value = 0000,0000

declare_sfr(0x8E, AUXR);  // Auxiliary Register, Reset Value = 0000,0001
#define T0x12 0x80        // 定时器0速度控制位，0: 12T模式，1: 1T模式
#define T1x12 0x40        // 定时器1速度控制位，0: 12T模式，1: 1T模式
#define UART_M0x6 0x20    // 串口1模式0的通讯速度控制，0：波特率不加倍，1：波特率加6倍
#define T2R 0x10          // 定时器2运行控制位，0: 关闭定时器2，1: 打开定时器2
#define T2_CT 0x08        // 定时器2工作方式，0: 定时器，1: 计数器(对引脚P3.7外部脉冲计数)
#define T2x12 0x04        // 定时器2速度控制位，0: 12T模式，1: 1T模式
#define EXTRAM 0x02       // 扩展RAM访问控制，0: 访问内部扩展RAM，1: 内部扩展RAM被禁用.
                          /* 使用总线扩展外部64KB RAM时，置1.
                           * 置0时，访问内部扩展RAM或外部扩展RAM(64KB)都可以.
                           * 只是置0时访问低(该型号内部扩展RAM容量)是访问的内部扩展RAM，
                           * 高于(该型号内部扩展RAM容量)的才是真正访问外部RAM，无伤大雅，但要精确控制的话，还是要设置该位.
                           * 两者都是通过MOVX指令访问的，时钟周期也一样.
                           */
#define S1ST2 0x01        // 串口1时钟源选择位，0: 使用定时器1作为波特率发生器，1: 使用定时器2作为波特率发生器（默认）

declare_sfr(0x8F, INT_CLKO);  // 外部中断允许和时钟输出寄存器，又名AUXR2, Reset Value = x000,x000
#define EX4 0x40              // INT4/P3.0 enable bit
#define EX3 0x20              // INT3/P3.7 enable bit
#define EX2 0x10              // INT2/P3.6 enable bit
#define T2CLKO 0x04           // CLKOUT2/P1.3 enable bit
#define T1CLKO 0x02           // CLKOUT1/P3.5 enable bit
#define T0CLKO 0x01           // CLKOUT0/P3.4 enable bit

declare_sfr(0x90, P1);     // 8 bit Port1, Reset Value = 1111,1111
declare_sbit(P1, 0, P10);  // P1.0
declare_sbit(P1, 1, P11);  // P1.1
declare_sbit(P1, 2, P12);  // P1.2
declare_sbit(P1, 3, P13);  // P1.3
declare_sbit(P1, 4, P14);  // P1.4
declare_sbit(P1, 5, P15);  // P1.5
declare_sbit(P1, 6, P16);  // P1.6
declare_sbit(P1, 7, P17);  // P1.7

declare_sfr(0x91, P1M1);  // Port1 Mode 1, Reset Value = 1111,1111
declare_sfr(0x92, P1M0);  // Port1 Mode 0, Reset Value = 0000,0000
declare_sfr(0x93, P0M1);  // Port0 Mode 1, Reset Value = 1111,1111
declare_sfr(0x94, P0M0);  // Port0 Mode 0, Reset Value = 0000,0000
declare_sfr(0x95, P2M1);  // Port2 Mode 1, Reset Value = 1111,1111
declare_sfr(0x96, P2M0);  // Port2 Mode 0, Reset Value = 0000,0000

declare_sfr(0x98, SCON);       // Serial Control, Reset Value = 0000,0000
declare_sbit(SCON, 7, SM0_FE); /* 当 PCON.6/SMOD0 = 1时，该位用于帧错误检测，当检测到一个无效停止位时由UART硬件置位，必须由软件清零；
                                * 当 PCON.6/SMOD0 = 0时，与SM1组合指定串口通信工作方式.
                                */
declare_sbit(SCON, 6, SM1);    // 当 PCON.6/SMOD0 = 0时，与SM0组合指定串口通信工作方式.
declare_sbit(SCON, 5, SM2);    /* 允许方式2(9bit)或方式3(9bit brtx)多机通信控制位
                                * 在方式2(9bit)或方式3(9bit brtx): 若SM2=1, REN=1, 则从机处于只有接收到RB8=1(地址帧)时才置位RI并中断
                                * 在方式1(8bit brtx): 若SM2=1，则只有在接收到有效的停止位时才置位RI并中断
                                * 在方式0(同步移位串行): SM2应置0
                                */
declare_sbit(SCON, 4, REN);    // Receive Enable, 1: 允许接收数据，0: 禁止接收数据
declare_sbit(SCON, 3, TB8);    /* Transmit Bit 8
                                * 在方式2(9bit)或方式3(9bit brtx): 为要发送第9位数据
                                */
declare_sbit(SCON, 2, RB8);    /* Receive Bit 8
                                * 在方式2(9bit)或方式3(9bit brtx): 为接收到的第9位数据
                                * 在方式1(8bit brtx): 若SM2=0, 则RB8是停止位
                                */
declare_sbit(SCON, 1, TI);     // Transmit Interrupt, 1: 发送完成
declare_sbit(SCON, 0, RI);     // Receive Interrupt, 1: 接收完成

declare_sfr(0x99, SBUF);  // Serial Data Buffer, Reset Value = 0000,0000

declare_sfr(0x9A, S2CON);  // Serial 2 Control, Reset Value = 0100,0000
#define S2SM0 0x80         // S2 mode 0
#define S2SM2 0x20         // S2 mode 2
#define S2REN 0x10         // S2 receive enable
#define S2TB8 0x08         // S2 transmit bit 8
#define S2RB8 0x04         // S2 receive bit 8
#define S2TI 0x02          // S2 transmit interrupt
#define S2RI 0x01          // S2 receive interrupt

declare_sfr(0x9B, S2BUF);  // Serial 2 Data Buffer, Reset Value = 0000,0000

declare_sfr(0x9D, IRCBAND);  // IRC band, Reset Value = xxxx,xxnn
#define USBCKS 0x80
#define USBCKS2 0x40
#define SEL_1 0x02
#define SEL_0 0x01

declare_sfr(0x9F, IRTRIM);  // IRC trim, Reset Value = nnnn,nnnn

declare_sfr(0xA0, P2);     // 8 bit Port2, Reset Value = 1111,1111
declare_sbit(P2, 0, P20);  // P2.0
declare_sbit(P2, 1, P21);  // P2.1
declare_sbit(P2, 2, P22);  // P2.2
declare_sbit(P2, 3, P23);  // P2.3
declare_sbit(P2, 4, P24);  // P2.4
declare_sbit(P2, 5, P25);  // P2.5
declare_sbit(P2, 6, P26);  // P2.6
declare_sbit(P2, 7, P27);  // P2.7

declare_sfr(0xA7, DID);        // 芯片版本，如A,B,C,D等，0 = A, 1 = B, ...,读之前须先设置 IAP_ADDRL=2
declare_sfr(0xA1, BUS_SPEED);  // Bus Speed Control, Reset Value = 00xx,x000
declare_sfr(0xA2, P_SW1);      // Port Switch Control, Reset Value = nn00,000x
#define S1_S_MASK 0xC0         // P_SW1[7:6] = S1_S, 串口1功能脚切换
#define SPI_S_MASK 0x60        // P_SW1[3:2] = SPI_S, SPI功能脚切换

declare_sfr(0xA8, IE);        // Interrupt Enable, Reset Value = 0000,0000
declare_sbit(0xA8, 7, EA);    // 中断允许总控制位
declare_sbit(0xA8, 6, ELVD);  // 低压监测中断允许
declare_sbit(0xA8, 5, EADC);  // ADC 中断允许位
declare_sbit(0xA8, 4, ES);    // 串口1中断允许控制位
declare_sbit(0xA8, 3, ET1);   // 定时中断1允许控制位
declare_sbit(0xA8, 2, EX1);   // INT1/P3.3允许控制位
declare_sbit(0xA8, 1, ET0);   // 定时中断0允许控制位
declare_sbit(0xA8, 0, EX0);   // INT0/P3.2允许控制位

declare_sfr(0xA9, SADDR);  // Slave Address, Reset Value = 0000,0000
declare_sfr(0xAA, WKTCL);  // Wakeup Timer Control Low Byte, Reset Value = 1111,1111
declare_sfr(0xAB, WKTCH);  // Wakeup Timer Control High Byte, Reset Value = 0111,1111
#define WKTEN 0x80         // Wakeup Timer Enable, 1: 启用唤醒定时器，0: 禁用

declare_sfr(0xAC, S3CON);  // Serial 3 Control, Reset Value = 0000,0000
#define S3SM0 0x80         // S3 mode, 0: 8-bit brtx, 1: 9-bit brtx
#define S3ST3 0x40         // S3 brt select, 0: T2, 1: T3
#define S3SM2 0x20         // S3 在 mode 1 9-bit brtx 模式下多机通信控制位
#define S3REN 0x10         // S3 Receive Enable
#define S3TB8 0x08         // S3 Transmit Bit 8
#define S3RB8 0x04         // S3 Receive Bit 8
#define S3TI 0x02          // S3 Transmit Interrupt
#define S3RI 0x01          // S3 Receive Interrupt

declare_sfr(0xAD, S3BUF);  // Serial 3 Data Buffer, Reset Value = 0000,0000
declare_sfr(0xAE, TA);     // DPTR时序控制寄存器，Reset Value = 0000,0000

declare_sfr(0xAF, IE2);  // Interrupt Control 2, Reset Value = 0000,0000
#define EUSB 0x80        // USB Interrupt Enable
#define ETKSU 0x80       // 触摸、按键 Interrupt Enable
#define ET4 0x40         // Timer 4 Interrupt Enable
#define ET3 0x20         // Timer 3 Interrupt Enable
#define ES4 0x10         // Serial 4 Interrupt Enable
#define ES3 0x08         // Serial 3 Interrupt Enable
#define ET2 0x04         // Timer 2 Interrupt Enable
#define ESPI 0x02        // SPI Interrupt Enable
#define ES2 0x01         // Serial 2 Interrupt Enable

declare_sfr(0xB0, P3);    // 8 bit Port3, Reset Value = 1111,1111
declare_sbit(P3, 0, P30);  // P3.0
declare_sbit(P3, 1, P31);  // P3.1
declare_sbit(P3, 2, P32);  // P3.2
declare_sbit(P3, 3, P33);  // P3.3
declare_sbit(P3, 4, P34);  // P3.4
declare_sbit(P3, 5, P35);  // P3.5
declare_sbit(P3, 6, P36);  // P3.6
declare_sbit(P3, 7, P37);  // P3.7

declare_sfr(0xB1, P3M1);  // Port3 Mode 1, Reset Value = 1111,1111
declare_sfr(0xB2, P3M0);  // Port3 Mode 0, Reset Value = 0000,0000
declare_sfr(0xB3, P4M1);  // Port4 Mode 1, Reset Value = 1111,1111
declare_sfr(0xB4, P4M0);  // Port4 Mode 0, Reset Value = 0000,0000

declare_sfr(0xB5, IP2);  // Interrupt Priority 2 Low Byte, Reset Value = 0000,0000
#define PUSB 0x80
#define PTKSU 0x80
#define PI2C 0x40
#define PCMP 0x20
#define PX4 0x10
#define PPWMB 0x08
#define PPWMA 0x04
#define PSPI 0x02
#define PS2 0x01

declare_sfr(0xB6, IP2H);  // Interrupt Priority 2 High Byte, Reset Value = 0000,0000
#define PUSBH 0x80
#define PTKSUH 0x80
#define PI2CH 0x40
#define PCMPH 0x20
#define PX4H 0x10
#define PPWMBH 0x08
#define PPWMAH 0x04
#define PSPIH 0x02
#define PS2H 0x01

declare_sfr(0xB7, IPH);  // Interrupt Priority High Byte, Reset Value = x000,0000
#define PLVDH 0x40       // Low votage detection interrupt priority HIGH
#define PADCH 0x20       // ADC interrupt priority HIGH
#define PSH 0x10         // Serial 1 interrupt priority HIGH
#define PT1H 0x08        // Timer 1 interrupt priority HIGH
#define PX1H 0x04        // INT1/P3.3 interrupt priority HIGH
#define PT0H 0x02        // Timer 0 interrupt priority HIGH
#define PX0H 0x01        // INT0/P3.2 interrupt priority HIGH

declare_sfr(0xB8, IP);        // Interrupt Priority Low Byte, Reset Value = x000,0000
declare_sbit(0xB8, 6, PLVD);  // Low votage detection interrupt priority LOW
declare_sbit(0xB8, 5, PADC);  // ADC interrupt priority LOW
declare_sbit(0xB8, 4, PS);    // Serial 1 interrupt priority LOW
declare_sbit(0xB8, 3, PT1);   // Timer 1 interrupt priority LOW
declare_sbit(0xB8, 2, PX1);   // INT1/P3.3 interrupt priority LOW
declare_sbit(0xB8, 1, PT0);   // Timer 0 interrupt priority LOW
declare_sbit(0xB8, 0, PX0);   // INT0/P3.2 interrupt priority LOW

declare_sfr(0xB9, SADEN);  // Slave Address Mask, Reset Value = 0000,0000

declare_sfr(0xBA, P_SW2);  // Port Switch Control 2, Reset Value = 0x00,0000
#define EAXFR 0x80         // 扩展RAM区特殊功能寄存器(XFR)访问控制，0：禁止访问XFR，1：允许访问XFR
                           /* STC8G/8H 的扩展SFR地址范围为0FA00H-0FFFFH，
                            * 与外扩64KB RAM有重合，因此在访问XFR时需先将EAXFR置一，在访问外部RAM时置零
                            */
#define I2C_S1 0x20        // I2C select bit 1
#define I2C_S0 0x10        // I2C select bit 0
#define I2C_S_MASK 0x30    // I2C select mask
#define CMPO_S 0x08        // CMP Output Select, 0: CMPO=P3.4, 1: CMPO=P4.1
#define S4_S 0x04          // Serial 4 select, 0: RxD4=P0.2, TxD4=P0.3; 1: RxD4=P5.2, TxD4=P5.3
#define S3_S 0x02          // Serial 3 select, 0: RxD3=P0.0, TxD3=P0.1; 1: RxD3=P5.0, TxD3=P5.1
#define S2_S 0x01          // Serial 2 select, 0: RxD2=P1.0, TxD2=P1.1; 1: RxD2=P4.6, TxD2=P4.7

declare_sfr(0xBC, ADC_CONTR);  // ADC Control, Reset Value = 0000,0000
#define ADC_POWER 0x80         // ADC Power Control
#define ADC_START 0x40         // ADC Start Conversion
#define ADC_FLAG 0x20          // ADC Conversion Complete Flag
#define ADC_EPWMT 0x10         // ADC Enable PWM Trigger
#define ADC_CHS_MASK 0x0F      // ADC Channel Select Mask
#define ADC_CHS_BGV 0x0F       // ADC Channel Select for Background Voltage

declare_sfr(0xBD, ADC_RES);   // ADC Result High Byte, Reset Value = 0000,0000
declare_sfr(0xBE, ADC_RESL);  // ADC Result Low Byte, Reset Value = 0000,0000

declare_sfr(0xC0, P4);     // 8 bit Port4, Reset Value = 1111,1111
declare_sbit(P4, 0, P40);  // P4.0
declare_sbit(P4, 1, P41);  // P4.1
declare_sbit(P4, 2, P42);  // P4.2
declare_sbit(P4, 3, P43);  // P4.3
declare_sbit(P4, 4, P44);  // P4.4
declare_sbit(P4, 5, P45);  // P4.5
declare_sbit(P4, 6, P46);  // P4.6
declare_sbit(P4, 7, P47);  // P4.7

declare_sfr(0xC1, WDT_CONTR);  // Watchdog Timer Control, Reset Value = 0x00,0000
#define WDT_FLAG 0x80          // Watch-Dog-Timer Flag, 1: Watch-Dog-Timer overflow, 0: No overflow
#define EN_WDT 0x20            // Watch-Dog-Timer Enable, 1: Enable Watch-Dog-Timer, 0: Disable
#define CLR_WDT 0x10           // Watch-Dog-Timer Clear, 1: Clear Watch-Dog-Timer, 0: No effect
#define IDL_WDT 0x08           // Watch-Dog-Timer Idle Mode, 1: Enable Watch-Dog-Timer in Idle Mode, 0: Disable
#define WDT_PS_MASK 0x07       // Watch-Dog-Timer Prescaler Mask, bits 0-2 are used to set the prescaler value

declare_sfr(0xC2, IAP_DATA);   // IAP Data Register, Reset Value = 1111,1111
declare_sfr(0xC3, IAP_ADDRH);  // IAP Address High Register, Reset Value = 0000,0000
declare_sfr(0xC4, IAP_ADDRL);  // IAP Address Low Register, Reset Value = 0000,0000
declare_sfr(0xC5, IAP_CMD);    // IAP Command Register, Reset Value = xxxx,xx00
#define IAP_CMD_MASK 0x03      // IAP Command Mask
declare_sfr(0xC6, IAP_TRIG);   // IAP Trigger Register, Reset Value = 0000,0000
declare_sfr(0xC7, IAP_CONTR);  // IAP Control Register, Reset Value = 0000,xxxx
#define IAPEN 0x80
#define SWBS 0x40     // 与 SWRST 配合，置1从ISP启动，置0从用户程序区启动
#define SWRST 0x20    // 置1产生软件复位，硬件自动复位该位
#define CMDFAIL 0x10  // IAP 命令失败标志，1: 命令失败，0: 命令成功

declare_sfr(0xC8, P5);     // 6 bit Port5, Reset Value = xx11,1111
declare_sbit(P5, 0, P50);  // P5.0
declare_sbit(P5, 1, P51);  // P5.1
declare_sbit(P5, 2, P52);  // P5.2
declare_sbit(P5, 3, P53);  // P5.3
declare_sbit(P5, 4, P54);  // P5.4
declare_sbit(P5, 5, P55);  // P5.5

declare_sfr(0xC9, P5M1);  // 6 bit Port5 Mode 1, Reset Value = xx11,1111
declare_sfr(0xCA, P5M0);  // 6 bit Port5 Mode 0, Reset Value = xx00,0000
declare_sfr(0xCB, P6M1);  // 8 bit Port6 Mode 1, Reset Value = 1111,1111
declare_sfr(0xCC, P6M0);  // 8 bit Port6 Mode 0, Reset Value = 0000,0000

declare_sfr(0xCD, SPSTAT);  // SPI status register, Reset Value = 00xx,xxxx
#define SPIF 0x80           // SPI Interrupt Flag
#define WCOL 0x40           // Write Collision Flag

declare_sfr(0xCE, SPCTL);  // SPI Control Register, Reset Value = 0000,0100
#define SSIG 0x80          // SS 引脚功能控制位，0：SS引脚确定器件是主机或从机；1：忽略SS引脚功能，使用MSTR确定器件是主机或从机
#define SPEN 0x40          // SPI Enable
#define DORD 0x20          // Data Order, 0: MSB, 1: LSB
#define MSTR 0x10          // Master/Slave Select
                           /**设置主机模式：
                            *   若SSIG=0，则SS管脚必须为高电平且设置MSTR=1
                            *   若SSIG=1, 则只需要设置MSTR=1，忽略SS引脚电平
                            * 设置从机模式：
                            *   若SSIG=0，则SS管脚必须为低电平，忽略MSTR
                            *   若SSIG=1，则只需要设置MSTR=0，忽略SS引脚电平
                            */
#define CPOL 0x08          // Clock Polarity
                           /* CPOL=0, SPI时钟线空闲时为低电平，SPICLK的前时钟沿为上升沿而后为下降沿
                            * CPOL=1, SPI时钟线空闲时为高电平, SPICLK的前时钟沿为下降沿而后为上升沿
                            */
#define CPHA 0x04          // Clock Phase
                           /* CPHA=0, 数据在SS非 为低(SSIG=0)时被驱动，在SPICLK的后时钟沿被改变，并在前时钟沿被采样
                            * CPHA=1, 数据在SPICLK的前时钟沿驱动，并在后时钟沿采样
                            */
#define SPR_MASK 0x03      // SPI时钟分频掩码

declare_sfr(0xCF, SPDAT);  // SPI Data Register, Reset Value = 0000,0000

declare_sfr(0xD0, PSW);      // Program Status Word, Reset Value = 0000,0000
declare_sbit(0xD0, 7, CY);   // Carry Flag
declare_sbit(0xD0, 6, AC);   // Auxiliary Carry Flag
declare_sbit(0xD0, 5, F0);   // User Flag 0
declare_sbit(0xD0, 4, RS1);  // Register Bank Select Bit 1
declare_sbit(0xD0, 3, RS0);  // Register Bank Select Bit 0
declare_sbit(0xD0, 2, OV);   // Overflow Flag
declare_sbit(0xD0, 1, F1);   // User Flag 1
declare_sbit(0xD0, 0, P);    // Parity Flag

declare_sfr(0xD1, T4T3M);  // Timer 4/3 Control, Reset Value = 0000,0000
#define T4R 0x80           // Timer 4 Run Control Bit
#define T4_CT 0x40         // Timer 4 Clock Type Select Bit, 0: Timer, 1: Counter
#define T4x12 0x20         // Timer 4 Prescaler Select Bit, 0: 12T, 1: 1T
#define T4CLKO 0x10        // Timer 4 Clock Output Control Bit, 0: Disable, 1: Enable T4CLKO/P0.7 toggle
#define T3R 0x08           // Timer 3 Run Control Bit
#define T3_CT 0x04         // Timer 3 Clock Type Select Bit, 0: Timer, 1: Counter
#define T3x12 0x02         // Timer 3 Prescaler Select Bit, 0: 12T, 1: 1T
#define T3CLKO 0x01        // Timer 3 Clock Output Control Bit, 0: Disable, 1: Enable T3CLKO/P0.5 toggle

declare_sfr(0xD2, T4H);  // Timer 4 High Byte
declare_sfr(0xD3, T4L);  // Timer 4 Low Byte
declare_sfr(0xD4, T3H);  // Timer 3 High Byte
declare_sfr(0xD5, T3L);  // Timer 3 Low Byte
declare_sfr(0xD6, T2H);  // Timer 2 High Byte
declare_sfr(0xD7, T2L);  // Timer 2 Low Byte

declare_sfr(0xDC, USBCLK);  // USB Clock Control Register, Reset Value = 0010,0000
#define ENCKM 0x80          // PLL 倍频控制，0：禁止PLL倍频；1：使能PLL倍频
#define PCKI_1 0x40
#define PCKI_0 0x20
#define CRE 0x10      // 时钟追频控制位，0：禁止时钟追频；1：使能时钟追频
#define TST_USB 0x08  // USB 测试模式控制位，0：禁止测试模式；1：使能测试模式
#define TST_PHY 0x04  // PHY 测试模式控制位，0：禁止测试模式；1：使能测试模式
#define PHYTST_1 0x02
#define PHYTST_0 0x01

declare_sfr(0xDD, MDU32SR);  // MDU32C Status Register，Reset Value = 0000,0000
declare_sfr(0xDE, ADCCFG);   // ADC Configuration Register，Reset Value = xx0x,0000
#define RESFMT 0x20          // ADC result format, 0: Left Align, 1: Right Align
#define ADC_SPEED_MASK 0x0F

declare_sfr(0xDF, IP3);  // Interrupt Priority Register 3, Reset Value = xxxx,x000
#define PRTC 0x04        // RTC interrupt priority
#define PS4 0x02         // Serial 4 interrupt priority
#define PS3 0x01         // Serial 3 interrupt priority

declare_sfr(0xE0, ACC);  // Accumulator, also known as A register, Reset Value = 0000,0000

declare_sfr(0xE1, P7M1);  // Port 7 Mode Register 1, Reset Value = 1111,1111
declare_sfr(0xE2, P7M0);  // Port 7 Mode Register 0, Reset Value = 0000,0000

declare_sfr(0xE3, DPS);  // DPTR select, Reset Value = 0000,0xx0
#define ID1 0x80         // DPTR1 increment/decrement control, 0: auto increment, 1: auto decrement
#define ID0 0x40         // DPTR0 increment/decrement control, 0: auto increment, 1: auto decrement
#define TSL 0x20         // DPTR0/1 auto switch (auto NOT SEL)
                         /**
                          * 当TSL=1,每当执行完成相关指令，芯片自动将SEL位取反
                          * 与TSL相关的指令有：
                          *   - MOV DPTR, #data16
                          *   - INC DPTR
                          *   - MOVC A, @A+DPTR
                          *   - MOVX A, @DPTR
                          *   - MOVX @DPTR, A
                          */
#define AU1 0x10         // ID1 enable
#define AU0 0x08         // ID0 enable
#define SEL 0x01         // DPTR0/1 select

declare_sfr(0xE4, DPL1);  // DPTR1 Low Byte, Reset Value = 0000,0000
declare_sfr(0xE5, DPH1);  // DPTR1 High Byte, Reset Value = 0000,0000

declare_sfr(0xE6, CMPCR1);  // Comparator 1 Control Register, Reset Value = 0000,0000
#define CMPEN 0x80
#define CMPIF 0x40
#define PIE 0x20
#define NIE 0x10
#define PIS 0x08
#define NIS 0x04
#define CMPOE 0x02
#define CMPRES 0x01

declare_sfr(0xE7, CMPCR2);  // Comparator 2 Control Register, Reset Value = 0000,0000
#define INVCMPO 0x80
#define DISFLT 0x40
#define LCDTY_MASK 0x3F

declare_sfr(0xE8, P6);     // Port 6, Reset Value = 1111,1111
declare_sbit(P6, 0, P60);  // P6.0
declare_sbit(P6, 1, P61);  // P6.1
declare_sbit(P6, 2, P62);  // P6.2
declare_sbit(P6, 3, P63);  // P6.3
declare_sbit(P6, 4, P64);  // P6.4
declare_sbit(P6, 5, P65);  // P6.5
declare_sbit(P6, 6, P66);  // P6.6
declare_sbit(P6, 7, P67);  // P6.7

declare_sfr(0xEC, USBDAT);   // USB Data Register, Reset Value = 0000,0000
declare_sfr(0xED, MDU32CR);  // MDU32C Control Register, Reset Value = xxx0,0000

declare_sfr(0xEE, IP3H);  // Interrupt Priority Register 3 High Byte, Reset Value = xxxx,x000
#define PRTCH 0x04        // RTC interrupt priority HIGH
#define PS4H 0x02         // Serial 4 interrupt priority HIGH
#define PS3H 0x01         // Serial 3 interrupt priority HIGH

declare_sfr(0xEF, AUXINTIF);  // Auxiliary Interrupt Flag Register, Reset Value = x000,x000
#define INT4IF 0x40
#define INT3IF 0x20
#define INT2IF 0x10
#define T4IF 0x04
#define T3IF 0x02
#define T2IF 0x01

/**
 * Register B, Reset Value = 0000,0000
 * 在乘法和除法运算中与累加器A配合使用：
 * `MUL A B`指令将A和B的8位无符号值相乘，16位结果低字节存储在A中，高字节存储在B中。
 * `DIV A B`指令将A的8位无符号值除以B的8位无符号值，整数商存储在A中，余数存储在B中。
 * 寄存器B还可以用作通用暂存器，存储临时数据或中间结果。
 */
declare_sfr(0xF0, B);

declare_sfr(0xF4, USBCON);  // USB Control Register, Reset Value = 0000,0000
#define ENUSB 0x80
#define ENUSBRST 0x40
#define PS2M 0x20
#define PUEN 0x10
#define PDEN 0x08
#define DFREC 0x04
#define DP 0x02
#define DM 0x01

declare_sfr(0xF5, IAP_TPS);  // IAP Wait Time Control Register, Reset Value = xx00,0000
                             /**
                              * IAP_TPS = round(MAIN_FOSC / 1000000) （四舍五入）
                              * e.g.
                              *   MAIN_FOSC = 5.5296MHz => IAP_TPS = 6
                              *   MAIN_FOSC = 11.0592MHz => IAP_TPS = 11
                              *   MAIN_FOSC = 12MHz => IAP_TPS = 12
                              */
#define IAP_TPS_MASK 0x3F

declare_sfr(0xF8, P7);     // Port 7, Reset Value = 1111,1111
declare_sbit(P7, 0, P70);  // P7.0
declare_sbit(P7, 1, P71);  // P7.1
declare_sbit(P7, 2, P72);  // P7.2
declare_sbit(P7, 3, P73);  // P7.3
declare_sbit(P7, 4, P74);  // P7.4
declare_sbit(P7, 5, P75);  // P7.5
declare_sbit(P7, 6, P76);  // P7.6
declare_sbit(P7, 7, P77);  // P7.7

declare_sfr(0xFC, USBADR);  // USB Address Register, Reset Value = 0000,0000
#define BUSY 0x80
#define AUTORD 0x40
#define UADR_MASK 0x3F

declare_sfr(0xFF, RSTCFG);  // Reset Configuration Register, Reset Value = x0x0,xx00
#define ENLVR 0x40
#define P54RST 0x10
#define LVDS_MASK 0x03

////////////////////////////// 扩展RAM区域特殊功能寄存器定义 //////////////////////////////

// 如下特殊功能寄存器位于扩展RAM区域
// 访问这些寄存器,需先将P_SW2的BIT7设置为1,才可正常读写

/////////////////////////////////////////////////
// FF00H-FFFFH
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// FE00H-FEFFH
/////////////////////////////////////////////////

#define CLKSEL (*(unsigned char volatile xdata *)0xFE00)
#define CLKDIV (*(unsigned char volatile xdata *)0xFE01)
#define HIRCCR (*(unsigned char volatile xdata *)0xFE02)
#define XOSCCR (*(unsigned char volatile xdata *)0xFE03)
#define IRC32KCR (*(unsigned char volatile xdata *)0xFE04)
#define MCLKOCR (*(unsigned char volatile xdata *)0xFE05)
#define IRCDB (*(unsigned char volatile xdata *)0xFE06)
#define IRC48MCR (*(unsigned char volatile xdata *)0xFE07)
#define X32KCR (*(unsigned char volatile xdata *)0xFE08)
#define HSCLKDIV (*(unsigned char volatile xdata *)0xFE0B)

#define P0PU (*(unsigned char volatile xdata *)0xFE10)
#define P1PU (*(unsigned char volatile xdata *)0xFE11)
#define P2PU (*(unsigned char volatile xdata *)0xFE12)
#define P3PU (*(unsigned char volatile xdata *)0xFE13)
#define P4PU (*(unsigned char volatile xdata *)0xFE14)
#define P5PU (*(unsigned char volatile xdata *)0xFE15)
#define P6PU (*(unsigned char volatile xdata *)0xFE16)
#define P7PU (*(unsigned char volatile xdata *)0xFE17)
#define P0NCS (*(unsigned char volatile xdata *)0xFE18)
#define P1NCS (*(unsigned char volatile xdata *)0xFE19)
#define P2NCS (*(unsigned char volatile xdata *)0xFE1A)
#define P3NCS (*(unsigned char volatile xdata *)0xFE1B)
#define P4NCS (*(unsigned char volatile xdata *)0xFE1C)
#define P5NCS (*(unsigned char volatile xdata *)0xFE1D)
#define P6NCS (*(unsigned char volatile xdata *)0xFE1E)
#define P7NCS (*(unsigned char volatile xdata *)0xFE1F)
#define P0SR (*(unsigned char volatile xdata *)0xFE20)
#define P1SR (*(unsigned char volatile xdata *)0xFE21)
#define P2SR (*(unsigned char volatile xdata *)0xFE22)
#define P3SR (*(unsigned char volatile xdata *)0xFE23)
#define P4SR (*(unsigned char volatile xdata *)0xFE24)
#define P5SR (*(unsigned char volatile xdata *)0xFE25)
#define P6SR (*(unsigned char volatile xdata *)0xFE26)
#define P7SR (*(unsigned char volatile xdata *)0xFE27)
#define P0DR (*(unsigned char volatile xdata *)0xFE28)
#define P1DR (*(unsigned char volatile xdata *)0xFE29)
#define P2DR (*(unsigned char volatile xdata *)0xFE2A)
#define P3DR (*(unsigned char volatile xdata *)0xFE2B)
#define P4DR (*(unsigned char volatile xdata *)0xFE2C)
#define P5DR (*(unsigned char volatile xdata *)0xFE2D)
#define P6DR (*(unsigned char volatile xdata *)0xFE2E)
#define P7DR (*(unsigned char volatile xdata *)0xFE2F)
#define P0IE (*(unsigned char volatile xdata *)0xFE30)
#define P1IE (*(unsigned char volatile xdata *)0xFE31)
#define P2IE (*(unsigned char volatile xdata *)0xFE32)
#define P3IE (*(unsigned char volatile xdata *)0xFE33)
#define P4IE (*(unsigned char volatile xdata *)0xFE34)
#define P5IE (*(unsigned char volatile xdata *)0xFE35)
#define P6IE (*(unsigned char volatile xdata *)0xFE36)
#define P7IE (*(unsigned char volatile xdata *)0xFE37)

#define LCMIFCFG (*(unsigned char volatile xdata *)0xFE50)
#define LCMIFCFG2 (*(unsigned char volatile xdata *)0xFE51)
#define LCMIFCR (*(unsigned char volatile xdata *)0xFE52)
#define LCMIFSTA (*(unsigned char volatile xdata *)0xFE53)
#define LCMIFDATL (*(unsigned char volatile xdata *)0xFE54)
#define LCMIFDATH (*(unsigned char volatile xdata *)0xFE55)

#define RTCCR (*(unsigned char volatile xdata *)0xFE60)
#define RTCCFG (*(unsigned char volatile xdata *)0xFE61)
#define RTCIEN (*(unsigned char volatile xdata *)0xFE62)
#define RTCIF (*(unsigned char volatile xdata *)0xFE63)
#define ALAHOUR (*(unsigned char volatile xdata *)0xFE64)
#define ALAMIN (*(unsigned char volatile xdata *)0xFE65)
#define ALASEC (*(unsigned char volatile xdata *)0xFE66)
#define ALASSEC (*(unsigned char volatile xdata *)0xFE67)
#define INIYEAR (*(unsigned char volatile xdata *)0xFE68)
#define INIMONTH (*(unsigned char volatile xdata *)0xFE69)
#define INIDAY (*(unsigned char volatile xdata *)0xFE6A)
#define INIHOUR (*(unsigned char volatile xdata *)0xFE6B)
#define INIMIN (*(unsigned char volatile xdata *)0xFE6C)
#define INISEC (*(unsigned char volatile xdata *)0xFE6D)
#define INISSEC (*(unsigned char volatile xdata *)0xFE6E)
#define YEAR (*(unsigned char volatile xdata *)0xFE70)
#define MONTH (*(unsigned char volatile xdata *)0xFE71)
#define DAY (*(unsigned char volatile xdata *)0xFE72)
#define HOUR (*(unsigned char volatile xdata *)0xFE73)
#define MIN (*(unsigned char volatile xdata *)0xFE74)
#define SEC (*(unsigned char volatile xdata *)0xFE75)
#define SSEC (*(unsigned char volatile xdata *)0xFE76)

#define I2CCFG (*(unsigned char volatile xdata *)0xFE80)
#define I2CMSCR (*(unsigned char volatile xdata *)0xFE81)
#define I2CMSST (*(unsigned char volatile xdata *)0xFE82)
#define I2CSLCR (*(unsigned char volatile xdata *)0xFE83)
#define I2CSLST (*(unsigned char volatile xdata *)0xFE84)
#define I2CSLADR (*(unsigned char volatile xdata *)0xFE85)
#define I2CTXD (*(unsigned char volatile xdata *)0xFE86)
#define I2CRXD (*(unsigned char volatile xdata *)0xFE87)
#define I2CMSAUX (*(unsigned char volatile xdata *)0xFE88)

#define SPFUNC (*(unsigned char volatile xdata *)0xFE98)
#define RSTFLAG (*(unsigned char volatile xdata *)0xFE99)

#define TM2PS (*(unsigned char volatile xdata *)0xFEA2)
#define TM3PS (*(unsigned char volatile xdata *)0xFEA3)
#define TM4PS (*(unsigned char volatile xdata *)0xFEA4)
#define ADCTIM (*(unsigned char volatile xdata *)0xFEA8)
#define T3T4PIN (*(unsigned char volatile xdata *)0xFEAC)
#define ADCEXCFG (*(unsigned char volatile xdata *)0xFEAD)
#define CMPEXCFG (*(unsigned char volatile xdata *)0xFEAE)

#define PWM1_ETRPS (*(unsigned char volatile xdata *)0xFEB0)
#define PWM1_ENO (*(unsigned char volatile xdata *)0xFEB1)
#define PWM1_PS (*(unsigned char volatile xdata *)0xFEB2)
#define PWM1_IOAUX (*(unsigned char volatile xdata *)0xFEB3)
#define PWM2_ETRPS (*(unsigned char volatile xdata *)0xFEB4)
#define PWM2_ENO (*(unsigned char volatile xdata *)0xFEB5)
#define PWM2_PS (*(unsigned char volatile xdata *)0xFEB6)
#define PWM2_IOAUX (*(unsigned char volatile xdata *)0xFEB7)
#define PWM1_CR1 (*(unsigned char volatile xdata *)0xFEC0)
#define PWM1_CR2 (*(unsigned char volatile xdata *)0xFEC1)
#define PWM1_SMCR (*(unsigned char volatile xdata *)0xFEC2)
#define PWM1_ETR (*(unsigned char volatile xdata *)0xFEC3)
#define PWM1_IER (*(unsigned char volatile xdata *)0xFEC4)
#define PWM1_SR1 (*(unsigned char volatile xdata *)0xFEC5)
#define PWM1_SR2 (*(unsigned char volatile xdata *)0xFEC6)
#define PWM1_EGR (*(unsigned char volatile xdata *)0xFEC7)
#define PWM1_CCMR1 (*(unsigned char volatile xdata *)0xFEC8)
#define PWM1_CCMR2 (*(unsigned char volatile xdata *)0xFEC9)
#define PWM1_CCMR3 (*(unsigned char volatile xdata *)0xFECA)
#define PWM1_CCMR4 (*(unsigned char volatile xdata *)0xFECB)
#define PWM1_CCER1 (*(unsigned char volatile xdata *)0xFECC)
#define PWM1_CCER2 (*(unsigned char volatile xdata *)0xFECD)
#define PWM1_CNTR (*(unsigned int volatile xdata *)0xFECE)
#define PWM1_CNTRH (*(unsigned char volatile xdata *)0xFECE)
#define PWM1_CNTRL (*(unsigned char volatile xdata *)0xFECF)
#define PWM1_PSCR (*(unsigned int volatile xdata *)0xFED0)
#define PWM1_PSCRH (*(unsigned char volatile xdata *)0xFED0)
#define PWM1_PSCRL (*(unsigned char volatile xdata *)0xFED1)
#define PWM1_ARR (*(unsigned int volatile xdata *)0xFED2)
#define PWM1_ARRH (*(unsigned char volatile xdata *)0xFED2)
#define PWM1_ARRL (*(unsigned char volatile xdata *)0xFED3)
#define PWM1_RCR (*(unsigned char volatile xdata *)0xFED4)
#define PWM1_CCR1 (*(unsigned int volatile xdata *)0xFED5)
#define PWM1_CCR1H (*(unsigned char volatile xdata *)0xFED5)
#define PWM1_CCR1L (*(unsigned char volatile xdata *)0xFED6)
#define PWM1_CCR2 (*(unsigned int volatile xdata *)0xFED7)
#define PWM1_CCR2H (*(unsigned char volatile xdata *)0xFED7)
#define PWM1_CCR2L (*(unsigned char volatile xdata *)0xFED8)
#define PWM1_CCR3 (*(unsigned int volatile xdata *)0xFED9)
#define PWM1_CCR3H (*(unsigned char volatile xdata *)0xFED9)
#define PWM1_CCR3L (*(unsigned char volatile xdata *)0xFEDA)
#define PWM1_CCR4 (*(unsigned int volatile xdata *)0xFEDB)
#define PWM1_CCR4H (*(unsigned char volatile xdata *)0xFEDB)
#define PWM1_CCR4L (*(unsigned char volatile xdata *)0xFEDC)
#define PWM1_BKR (*(unsigned char volatile xdata *)0xFEDD)
#define PWM1_DTR (*(unsigned char volatile xdata *)0xFEDE)
#define PWM1_OISR (*(unsigned char volatile xdata *)0xFEDF)
#define PWM2_CR1 (*(unsigned char volatile xdata *)0xFEE0)
#define PWM2_CR2 (*(unsigned char volatile xdata *)0xFEE1)
#define PWM2_SMCR (*(unsigned char volatile xdata *)0xFEE2)
#define PWM2_ETR (*(unsigned char volatile xdata *)0xFEE3)
#define PWM2_IER (*(unsigned char volatile xdata *)0xFEE4)
#define PWM2_SR1 (*(unsigned char volatile xdata *)0xFEE5)
#define PWM2_SR2 (*(unsigned char volatile xdata *)0xFEE6)
#define PWM2_EGR (*(unsigned char volatile xdata *)0xFEE7)
#define PWM2_CCMR1 (*(unsigned char volatile xdata *)0xFEE8)
#define PWM2_CCMR2 (*(unsigned char volatile xdata *)0xFEE9)
#define PWM2_CCMR3 (*(unsigned char volatile xdata *)0xFEEA)
#define PWM2_CCMR4 (*(unsigned char volatile xdata *)0xFEEB)
#define PWM2_CCER1 (*(unsigned char volatile xdata *)0xFEEC)
#define PWM2_CCER2 (*(unsigned char volatile xdata *)0xFEED)
#define PWM2_CNTR (*(unsigned int volatile xdata *)0xFEEE)
#define PWM2_CNTRH (*(unsigned char volatile xdata *)0xFEEE)
#define PWM2_CNTRL (*(unsigned char volatile xdata *)0xFEEF)
#define PWM2_PSCR (*(unsigned int volatile xdata *)0xFEF0)
#define PWM2_PSCRH (*(unsigned char volatile xdata *)0xFEF0)
#define PWM2_PSCRL (*(unsigned char volatile xdata *)0xFEF1)
#define PWM2_ARR (*(unsigned int volatile xdata *)0xFEF2)
#define PWM2_ARRH (*(unsigned char volatile xdata *)0xFEF2)
#define PWM2_ARRL (*(unsigned char volatile xdata *)0xFEF3)
#define PWM2_RCR (*(unsigned char volatile xdata *)0xFEF4)
#define PWM2_CCR1 (*(unsigned int volatile xdata *)0xFEF5)
#define PWM2_CCR1H (*(unsigned char volatile xdata *)0xFEF5)
#define PWM2_CCR1L (*(unsigned char volatile xdata *)0xFEF6)
#define PWM2_CCR2 (*(unsigned int volatile xdata *)0xFEF7)
#define PWM2_CCR2H (*(unsigned char volatile xdata *)0xFEF7)
#define PWM2_CCR2L (*(unsigned char volatile xdata *)0xFEF8)
#define PWM2_CCR3 (*(unsigned int volatile xdata *)0xFEF9)
#define PWM2_CCR3H (*(unsigned char volatile xdata *)0xFEF9)
#define PWM2_CCR3L (*(unsigned char volatile xdata *)0xFEFA)
#define PWM2_CCR4 (*(unsigned int volatile xdata *)0xFEFB)
#define PWM2_CCR4H (*(unsigned char volatile xdata *)0xFEFB)
#define PWM2_CCR4L (*(unsigned char volatile xdata *)0xFEFC)
#define PWM2_BKR (*(unsigned char volatile xdata *)0xFEFD)
#define PWM2_DTR (*(unsigned char volatile xdata *)0xFEFE)
#define PWM2_OISR (*(unsigned char volatile xdata *)0xFEFF)

#define PWMA_ETRPS (*(unsigned char volatile xdata *)0xFEB0)
#define PWMA_ENO (*(unsigned char volatile xdata *)0xFEB1)
#define PWMA_PS (*(unsigned char volatile xdata *)0xFEB2)
#define PWMA_IOAUX (*(unsigned char volatile xdata *)0xFEB3)
#define PWMB_ETRPS (*(unsigned char volatile xdata *)0xFEB4)
#define PWMB_ENO (*(unsigned char volatile xdata *)0xFEB5)
#define PWMB_PS (*(unsigned char volatile xdata *)0xFEB6)
#define PWMB_IOAUX (*(unsigned char volatile xdata *)0xFEB7)
#define PWMA_CR1 (*(unsigned char volatile xdata *)0xFEC0)
#define PWMA_CR2 (*(unsigned char volatile xdata *)0xFEC1)
#define PWMA_SMCR (*(unsigned char volatile xdata *)0xFEC2)
#define PWMA_ETR (*(unsigned char volatile xdata *)0xFEC3)
#define PWMA_IER (*(unsigned char volatile xdata *)0xFEC4)
#define PWMA_SR1 (*(unsigned char volatile xdata *)0xFEC5)
#define PWMA_SR2 (*(unsigned char volatile xdata *)0xFEC6)
#define PWMA_EGR (*(unsigned char volatile xdata *)0xFEC7)
#define PWMA_CCMR1 (*(unsigned char volatile xdata *)0xFEC8)
#define PWMA_CCMR2 (*(unsigned char volatile xdata *)0xFEC9)
#define PWMA_CCMR3 (*(unsigned char volatile xdata *)0xFECA)
#define PWMA_CCMR4 (*(unsigned char volatile xdata *)0xFECB)
#define PWMA_CCER1 (*(unsigned char volatile xdata *)0xFECC)
#define PWMA_CCER2 (*(unsigned char volatile xdata *)0xFECD)
#define PWMA_CNTR (*(unsigned int volatile xdata *)0xFECE)
#define PWMA_CNTRH (*(unsigned char volatile xdata *)0xFECE)
#define PWMA_CNTRL (*(unsigned char volatile xdata *)0xFECF)
#define PWMA_PSCR (*(unsigned int volatile xdata *)0xFED0)
#define PWMA_PSCRH (*(unsigned char volatile xdata *)0xFED0)
#define PWMA_PSCRL (*(unsigned char volatile xdata *)0xFED1)
#define PWMA_ARR (*(unsigned int volatile xdata *)0xFED2)
#define PWMA_ARRH (*(unsigned char volatile xdata *)0xFED2)
#define PWMA_ARRL (*(unsigned char volatile xdata *)0xFED3)
#define PWMA_RCR (*(unsigned char volatile xdata *)0xFED4)
#define PWMA_CCR1 (*(unsigned int volatile xdata *)0xFED5)
#define PWMA_CCR1H (*(unsigned char volatile xdata *)0xFED5)
#define PWMA_CCR1L (*(unsigned char volatile xdata *)0xFED6)
#define PWMA_CCR2 (*(unsigned int volatile xdata *)0xFED7)
#define PWMA_CCR2H (*(unsigned char volatile xdata *)0xFED7)
#define PWMA_CCR2L (*(unsigned char volatile xdata *)0xFED8)
#define PWMA_CCR3 (*(unsigned int volatile xdata *)0xFED9)
#define PWMA_CCR3H (*(unsigned char volatile xdata *)0xFED9)
#define PWMA_CCR3L (*(unsigned char volatile xdata *)0xFEDA)
#define PWMA_CCR4 (*(unsigned int volatile xdata *)0xFEDB)
#define PWMA_CCR4H (*(unsigned char volatile xdata *)0xFEDB)
#define PWMA_CCR4L (*(unsigned char volatile xdata *)0xFEDC)
#define PWMA_BKR (*(unsigned char volatile xdata *)0xFEDD)
#define PWMA_DTR (*(unsigned char volatile xdata *)0xFEDE)
#define PWMA_OISR (*(unsigned char volatile xdata *)0xFEDF)
#define PWMB_CR1 (*(unsigned char volatile xdata *)0xFEE0)
#define PWMB_CR2 (*(unsigned char volatile xdata *)0xFEE1)
#define PWMB_SMCR (*(unsigned char volatile xdata *)0xFEE2)
#define PWMB_ETR (*(unsigned char volatile xdata *)0xFEE3)
#define PWMB_IER (*(unsigned char volatile xdata *)0xFEE4)
#define PWMB_SR1 (*(unsigned char volatile xdata *)0xFEE5)
#define PWMB_SR2 (*(unsigned char volatile xdata *)0xFEE6)
#define PWMB_EGR (*(unsigned char volatile xdata *)0xFEE7)
#define PWMB_CCMR1 (*(unsigned char volatile xdata *)0xFEE8)
#define PWMB_CCMR2 (*(unsigned char volatile xdata *)0xFEE9)
#define PWMB_CCMR3 (*(unsigned char volatile xdata *)0xFEEA)
#define PWMB_CCMR4 (*(unsigned char volatile xdata *)0xFEEB)
#define PWMB_CCER1 (*(unsigned char volatile xdata *)0xFEEC)
#define PWMB_CCER2 (*(unsigned char volatile xdata *)0xFEED)
#define PWMB_CNTR (*(unsigned int volatile xdata *)0xFEEE)
#define PWMB_CNTRH (*(unsigned char volatile xdata *)0xFEEE)
#define PWMB_CNTRL (*(unsigned char volatile xdata *)0xFEEF)
#define PWMB_PSCR (*(unsigned int volatile xdata *)0xFEF0)
#define PWMB_PSCRH (*(unsigned char volatile xdata *)0xFEF0)
#define PWMB_PSCRL (*(unsigned char volatile xdata *)0xFEF1)
#define PWMB_ARR (*(unsigned int volatile xdata *)0xFEF2)
#define PWMB_ARRH (*(unsigned char volatile xdata *)0xFEF2)
#define PWMB_ARRL (*(unsigned char volatile xdata *)0xFEF3)
#define PWMB_RCR (*(unsigned char volatile xdata *)0xFEF4)
#define PWMB_CCR5 (*(unsigned int volatile xdata *)0xFEF5)
#define PWMB_CCR5H (*(unsigned char volatile xdata *)0xFEF5)
#define PWMB_CCR5L (*(unsigned char volatile xdata *)0xFEF6)
#define PWMB_CCR6 (*(unsigned int volatile xdata *)0xFEF7)
#define PWMB_CCR6H (*(unsigned char volatile xdata *)0xFEF7)
#define PWMB_CCR6L (*(unsigned char volatile xdata *)0xFEF8)
#define PWMB_CCR7 (*(unsigned int volatile xdata *)0xFEF9)
#define PWMB_CCR7H (*(unsigned char volatile xdata *)0xFEF9)
#define PWMB_CCR7L (*(unsigned char volatile xdata *)0xFEFA)
#define PWMB_CCR8 (*(unsigned int volatile xdata *)0xFEFB)
#define PWMB_CCR8H (*(unsigned char volatile xdata *)0xFEFB)
#define PWMB_CCR8L (*(unsigned char volatile xdata *)0xFEFC)
#define PWMB_BKR (*(unsigned char volatile xdata *)0xFEFD)
#define PWMB_DTR (*(unsigned char volatile xdata *)0xFEFE)
#define PWMB_OISR (*(unsigned char volatile xdata *)0xFEFF)

/////////////////////////////////////////////////
// FD00H-FDFFH
/////////////////////////////////////////////////

#define P0INTE (*(unsigned char volatile xdata *)0xFD00)
#define P1INTE (*(unsigned char volatile xdata *)0xFD01)
#define P2INTE (*(unsigned char volatile xdata *)0xFD02)
#define P3INTE (*(unsigned char volatile xdata *)0xFD03)
#define P4INTE (*(unsigned char volatile xdata *)0xFD04)
#define P5INTE (*(unsigned char volatile xdata *)0xFD05)
#define P6INTE (*(unsigned char volatile xdata *)0xFD06)
#define P7INTE (*(unsigned char volatile xdata *)0xFD07)
#define P0INTF (*(unsigned char volatile xdata *)0xFD10)
#define P1INTF (*(unsigned char volatile xdata *)0xFD11)
#define P2INTF (*(unsigned char volatile xdata *)0xFD12)
#define P3INTF (*(unsigned char volatile xdata *)0xFD13)
#define P4INTF (*(unsigned char volatile xdata *)0xFD14)
#define P5INTF (*(unsigned char volatile xdata *)0xFD15)
#define P6INTF (*(unsigned char volatile xdata *)0xFD16)
#define P7INTF (*(unsigned char volatile xdata *)0xFD17)
#define P0IM0 (*(unsigned char volatile xdata *)0xFD20)
#define P1IM0 (*(unsigned char volatile xdata *)0xFD21)
#define P2IM0 (*(unsigned char volatile xdata *)0xFD22)
#define P3IM0 (*(unsigned char volatile xdata *)0xFD23)
#define P4IM0 (*(unsigned char volatile xdata *)0xFD24)
#define P5IM0 (*(unsigned char volatile xdata *)0xFD25)
#define P6IM0 (*(unsigned char volatile xdata *)0xFD26)
#define P7IM0 (*(unsigned char volatile xdata *)0xFD27)
#define P0IM1 (*(unsigned char volatile xdata *)0xFD30)
#define P1IM1 (*(unsigned char volatile xdata *)0xFD31)
#define P2IM1 (*(unsigned char volatile xdata *)0xFD32)
#define P3IM1 (*(unsigned char volatile xdata *)0xFD33)
#define P4IM1 (*(unsigned char volatile xdata *)0xFD34)
#define P5IM1 (*(unsigned char volatile xdata *)0xFD35)
#define P6IM1 (*(unsigned char volatile xdata *)0xFD36)
#define P7IM1 (*(unsigned char volatile xdata *)0xFD37)
#define P0WKUE (*(unsigned char volatile xdata *)0xFD40)
#define P1WKUE (*(unsigned char volatile xdata *)0xFD41)
#define P2WKUE (*(unsigned char volatile xdata *)0xFD42)
#define P3WKUE (*(unsigned char volatile xdata *)0xFD43)
#define P4WKUE (*(unsigned char volatile xdata *)0xFD44)
#define P5WKUE (*(unsigned char volatile xdata *)0xFD45)
#define P6WKUE (*(unsigned char volatile xdata *)0xFD46)
#define P7WKUE (*(unsigned char volatile xdata *)0xFD47)

#define PINIPL (*(unsigned char volatile xdata *)0xFD60)
#define PINIPH (*(unsigned char volatile xdata *)0xFD61)

#define CRECR (*(unsigned char volatile xdata *)0xFDA8)
#define CRECNTH (*(unsigned char volatile xdata *)0xFDA9)
#define CRECNTL (*(unsigned char volatile xdata *)0xFDAA)
#define CRERES (*(unsigned char volatile xdata *)0xFDAB)

#define CHIPID ((unsigned char volatile xdata *)0xFDE0)

#define CHIPID0 (*(unsigned char volatile xdata *)0xFDE0)
#define CHIPID1 (*(unsigned char volatile xdata *)0xFDE1)
#define CHIPID2 (*(unsigned char volatile xdata *)0xFDE2)
#define CHIPID3 (*(unsigned char volatile xdata *)0xFDE3)
#define CHIPID4 (*(unsigned char volatile xdata *)0xFDE4)
#define CHIPID5 (*(unsigned char volatile xdata *)0xFDE5)
#define CHIPID6 (*(unsigned char volatile xdata *)0xFDE6)
#define CHIPID7 (*(unsigned char volatile xdata *)0xFDE7)
#define CHIPID8 (*(unsigned char volatile xdata *)0xFDE8)
#define CHIPID9 (*(unsigned char volatile xdata *)0xFDE9)
#define CHIPID10 (*(unsigned char volatile xdata *)0xFDEA)
#define CHIPID11 (*(unsigned char volatile xdata *)0xFDEB)
#define CHIPID12 (*(unsigned char volatile xdata *)0xFDEC)
#define CHIPID13 (*(unsigned char volatile xdata *)0xFDED)
#define CHIPID14 (*(unsigned char volatile xdata *)0xFDEE)
#define CHIPID15 (*(unsigned char volatile xdata *)0xFDEF)
#define CHIPID16 (*(unsigned char volatile xdata *)0xFDF0)
#define CHIPID17 (*(unsigned char volatile xdata *)0xFDF1)
#define CHIPID18 (*(unsigned char volatile xdata *)0xFDF2)
#define CHIPID19 (*(unsigned char volatile xdata *)0xFDF3)
#define CHIPID20 (*(unsigned char volatile xdata *)0xFDF4)
#define CHIPID21 (*(unsigned char volatile xdata *)0xFDF5)
#define CHIPID22 (*(unsigned char volatile xdata *)0xFDF6)
#define CHIPID23 (*(unsigned char volatile xdata *)0xFDF7)
#define CHIPID24 (*(unsigned char volatile xdata *)0xFDF8)
#define CHIPID25 (*(unsigned char volatile xdata *)0xFDF9)
#define CHIPID26 (*(unsigned char volatile xdata *)0xFDFA)
#define CHIPID27 (*(unsigned char volatile xdata *)0xFDFB)
#define CHIPID28 (*(unsigned char volatile xdata *)0xFDFC)
#define CHIPID29 (*(unsigned char volatile xdata *)0xFDFD)
#define CHIPID30 (*(unsigned char volatile xdata *)0xFDFE)
#define CHIPID31 (*(unsigned char volatile xdata *)0xFDFF)

/////////////////////////////////////////////////
// FC00H-FCFFH
/////////////////////////////////////////////////

#define MD3 (*(unsigned char volatile xdata *)0xFCF0)
#define MD2 (*(unsigned char volatile xdata *)0xFCF1)
#define MD1 (*(unsigned char volatile xdata *)0xFCF2)
#define MD0 (*(unsigned char volatile xdata *)0xFCF3)
#define MD5 (*(unsigned char volatile xdata *)0xFCF4)
#define MD4 (*(unsigned char volatile xdata *)0xFCF5)
#define ARCON (*(unsigned char volatile xdata *)0xFCF6)
#define OPCON (*(unsigned char volatile xdata *)0xFCF7)

/////////////////////////////////////////////////
// FB00H-FBFFH
/////////////////////////////////////////////////

#define COMEN (*(unsigned char volatile xdata *)0xFB00)
#define SEGENL (*(unsigned char volatile xdata *)0xFB01)
#define SEGENH (*(unsigned char volatile xdata *)0xFB02)
#define LEDCTRL (*(unsigned char volatile xdata *)0xFB03)
#define LEDCKS (*(unsigned char volatile xdata *)0xFB04)
#define COM0_DA_L (*(unsigned char volatile xdata *)0xFB10)
#define COM1_DA_L (*(unsigned char volatile xdata *)0xFB11)
#define COM2_DA_L (*(unsigned char volatile xdata *)0xFB12)
#define COM3_DA_L (*(unsigned char volatile xdata *)0xFB13)
#define COM4_DA_L (*(unsigned char volatile xdata *)0xFB14)
#define COM5_DA_L (*(unsigned char volatile xdata *)0xFB15)
#define COM6_DA_L (*(unsigned char volatile xdata *)0xFB16)
#define COM7_DA_L (*(unsigned char volatile xdata *)0xFB17)
#define COM0_DA_H (*(unsigned char volatile xdata *)0xFB18)
#define COM1_DA_H (*(unsigned char volatile xdata *)0xFB19)
#define COM2_DA_H (*(unsigned char volatile xdata *)0xFB1A)
#define COM3_DA_H (*(unsigned char volatile xdata *)0xFB1B)
#define COM4_DA_H (*(unsigned char volatile xdata *)0xFB1C)
#define COM5_DA_H (*(unsigned char volatile xdata *)0xFB1D)
#define COM6_DA_H (*(unsigned char volatile xdata *)0xFB1E)
#define COM7_DA_H (*(unsigned char volatile xdata *)0xFB1F)
#define COM0_DC_L (*(unsigned char volatile xdata *)0xFB20)
#define COM1_DC_L (*(unsigned char volatile xdata *)0xFB21)
#define COM2_DC_L (*(unsigned char volatile xdata *)0xFB22)
#define COM3_DC_L (*(unsigned char volatile xdata *)0xFB23)
#define COM4_DC_L (*(unsigned char volatile xdata *)0xFB24)
#define COM5_DC_L (*(unsigned char volatile xdata *)0xFB25)
#define COM6_DC_L (*(unsigned char volatile xdata *)0xFB26)
#define COM7_DC_L (*(unsigned char volatile xdata *)0xFB27)
#define COM0_DC_H (*(unsigned char volatile xdata *)0xFB28)
#define COM1_DC_H (*(unsigned char volatile xdata *)0xFB29)
#define COM2_DC_H (*(unsigned char volatile xdata *)0xFB2A)
#define COM3_DC_H (*(unsigned char volatile xdata *)0xFB2B)
#define COM4_DC_H (*(unsigned char volatile xdata *)0xFB2C)
#define COM5_DC_H (*(unsigned char volatile xdata *)0xFB2D)
#define COM6_DC_H (*(unsigned char volatile xdata *)0xFB2E)
#define COM7_DC_H (*(unsigned char volatile xdata *)0xFB2F)

#define TSCHEN1 (*(unsigned char volatile xdata *)0xFB40)
#define TSCHEN2 (*(unsigned char volatile xdata *)0xFB41)
#define TSCFG1 (*(unsigned char volatile xdata *)0xFB42)
#define TSCFG2 (*(unsigned char volatile xdata *)0xFB43)
#define TSWUTC (*(unsigned char volatile xdata *)0xFB44)
#define TSCTRL (*(unsigned char volatile xdata *)0xFB45)
#define TSSTA1 (*(unsigned char volatile xdata *)0xFB46)
#define TSSTA2 (*(unsigned char volatile xdata *)0xFB47)
#define TSRT (*(unsigned char volatile xdata *)0xFB48)
#define TSDAT (*(unsigned int volatile xdata *)0xFB49)
#define TSDATH (*(unsigned char volatile xdata *)0xFB49)
#define TSDATL (*(unsigned char volatile xdata *)0xFB4A)
#define TSTH00 (*(unsigned int volatile xdata *)0xFB50)
#define TSTH00H (*(unsigned char volatile xdata *)0xFB50)
#define TSTH00L (*(unsigned char volatile xdata *)0xFB51)
#define TSTH01 (*(unsigned int volatile xdata *)0xFB52)
#define TSTH01H (*(unsigned char volatile xdata *)0xFB52)
#define TSTH01L (*(unsigned char volatile xdata *)0xFB53)
#define TSTH02 (*(unsigned int volatile xdata *)0xFB54)
#define TSTH02H (*(unsigned char volatile xdata *)0xFB54)
#define TSTH02L (*(unsigned char volatile xdata *)0xFB55)
#define TSTH03 (*(unsigned int volatile xdata *)0xFB56)
#define TSTH03H (*(unsigned char volatile xdata *)0xFB56)
#define TSTH03L (*(unsigned char volatile xdata *)0xFB57)
#define TSTH04 (*(unsigned int volatile xdata *)0xFB58)
#define TSTH04H (*(unsigned char volatile xdata *)0xFB58)
#define TSTH04L (*(unsigned char volatile xdata *)0xFB59)
#define TSTH05 (*(unsigned int volatile xdata *)0xFB5A)
#define TSTH05H (*(unsigned char volatile xdata *)0xFB5A)
#define TSTH05L (*(unsigned char volatile xdata *)0xFB5B)
#define TSTH06 (*(unsigned int volatile xdata *)0xFB5C)
#define TSTH06H (*(unsigned char volatile xdata *)0xFB5C)
#define TSTH06L (*(unsigned char volatile xdata *)0xFB5D)
#define TSTH07 (*(unsigned int volatile xdata *)0xFB5E)
#define TSTH07H (*(unsigned char volatile xdata *)0xFB5E)
#define TSTH07L (*(unsigned char volatile xdata *)0xFB5F)
#define TSTH08 (*(unsigned int volatile xdata *)0xFB60)
#define TSTH08H (*(unsigned char volatile xdata *)0xFB60)
#define TSTH08L (*(unsigned char volatile xdata *)0xFB61)
#define TSTH09 (*(unsigned int volatile xdata *)0xFB62)
#define TSTH09H (*(unsigned char volatile xdata *)0xFB62)
#define TSTH09L (*(unsigned char volatile xdata *)0xFB63)
#define TSTH10 (*(unsigned int volatile xdata *)0xFB64)
#define TSTH10H (*(unsigned char volatile xdata *)0xFB64)
#define TSTH10L (*(unsigned char volatile xdata *)0xFB65)
#define TSTH11 (*(unsigned int volatile xdata *)0xFB66)
#define TSTH11H (*(unsigned char volatile xdata *)0xFB66)
#define TSTH11L (*(unsigned char volatile xdata *)0xFB67)
#define TSTH12 (*(unsigned int volatile xdata *)0xFB68)
#define TSTH12H (*(unsigned char volatile xdata *)0xFB68)
#define TSTH12L (*(unsigned char volatile xdata *)0xFB69)
#define TSTH13 (*(unsigned int volatile xdata *)0xFB6A)
#define TSTH13H (*(unsigned char volatile xdata *)0xFB6A)
#define TSTH13L (*(unsigned char volatile xdata *)0xFB6B)
#define TSTH14 (*(unsigned int volatile xdata *)0xFB6C)
#define TSTH14H (*(unsigned char volatile xdata *)0xFB6C)
#define TSTH14L (*(unsigned char volatile xdata *)0xFB6D)
#define TSTH15 (*(unsigned int volatile xdata *)0xFB6E)
#define TSTH15H (*(unsigned char volatile xdata *)0xFB6E)
#define TSTH15L (*(unsigned char volatile xdata *)0xFB6F)

#define LCDCFG (*(unsigned char volatile xdata *)0xFB80)
#define LCDCFG2 (*(unsigned char volatile xdata *)0xFB81)
#define DBLEN (*(unsigned char volatile xdata *)0xFB82)
#define COMLENL (*(unsigned char volatile xdata *)0xFB83)
#define COMLENM (*(unsigned char volatile xdata *)0xFB84)
#define COMLENH (*(unsigned char volatile xdata *)0xFB85)
#define BLINKRATE (*(unsigned char volatile xdata *)0xFB86)
#define LCDCR (*(unsigned char volatile xdata *)0xFB87)
#define COMON (*(unsigned char volatile xdata *)0xFB88)
#define SEGON1 (*(unsigned char volatile xdata *)0xFB8A)
#define SEGON2 (*(unsigned char volatile xdata *)0xFB8B)
#define SEGON3 (*(unsigned char volatile xdata *)0xFB8C)
#define SEGON4 (*(unsigned char volatile xdata *)0xFB8D)
#define SEGON5 (*(unsigned char volatile xdata *)0xFB8E)
#define C0SEGV0 (*(unsigned char volatile xdata *)0xFB90)
#define C0SEGV1 (*(unsigned char volatile xdata *)0xFB91)
#define C0SEGV2 (*(unsigned char volatile xdata *)0xFB92)
#define C0SEGV3 (*(unsigned char volatile xdata *)0xFB93)
#define C0SEGV4 (*(unsigned char volatile xdata *)0xFB94)
#define C1SEGV0 (*(unsigned char volatile xdata *)0xFB98)
#define C1SEGV1 (*(unsigned char volatile xdata *)0xFB99)
#define C1SEGV2 (*(unsigned char volatile xdata *)0xFB9A)
#define C1SEGV3 (*(unsigned char volatile xdata *)0xFB9B)
#define C1SEGV4 (*(unsigned char volatile xdata *)0xFB9C)
#define C2SEGV0 (*(unsigned char volatile xdata *)0xFBA0)
#define C2SEGV1 (*(unsigned char volatile xdata *)0xFBA1)
#define C2SEGV2 (*(unsigned char volatile xdata *)0xFBA2)
#define C2SEGV3 (*(unsigned char volatile xdata *)0xFBA3)
#define C2SEGV4 (*(unsigned char volatile xdata *)0xFBA4)
#define C3SEGV0 (*(unsigned char volatile xdata *)0xFBA8)
#define C3SEGV1 (*(unsigned char volatile xdata *)0xFBA9)
#define C3SEGV2 (*(unsigned char volatile xdata *)0xFBAA)
#define C3SEGV3 (*(unsigned char volatile xdata *)0xFBAB)
#define C3SEGV4 (*(unsigned char volatile xdata *)0xFBAC)

#define HSPWMA_CFG (*(unsigned char volatile xdata *)0xFBF0)
#define HSPWMA_ADR (*(unsigned char volatile xdata *)0xFBF1)
#define HSPWMA_DAT (*(unsigned char volatile xdata *)0xFBF2)

#define HSPWMB_CFG (*(unsigned char volatile xdata *)0xFBF4)
#define HSPWMB_ADR (*(unsigned char volatile xdata *)0xFBF5)
#define HSPWMB_DAT (*(unsigned char volatile xdata *)0xFBF6)

/////////////////////////////////////////////////
// FA00H-FAFFH
/////////////////////////////////////////////////

#define DMA_M2M_CFG (*(unsigned char volatile xdata *)0xFA00)
#define DMA_M2M_CR (*(unsigned char volatile xdata *)0xFA01)
#define DMA_M2M_STA (*(unsigned char volatile xdata *)0xFA02)
#define DMA_M2M_AMT (*(unsigned char volatile xdata *)0xFA03)
#define DMA_M2M_DONE (*(unsigned char volatile xdata *)0xFA04)
#define DMA_M2M_TXAH (*(unsigned char volatile xdata *)0xFA05)
#define DMA_M2M_TXAL (*(unsigned char volatile xdata *)0xFA06)
#define DMA_M2M_RXAH (*(unsigned char volatile xdata *)0xFA07)
#define DMA_M2M_RXAL (*(unsigned char volatile xdata *)0xFA08)

#define DMA_ADC_CFG (*(unsigned char volatile xdata *)0xFA10)
#define DMA_ADC_CR (*(unsigned char volatile xdata *)0xFA11)
#define DMA_ADC_STA (*(unsigned char volatile xdata *)0xFA12)
#define DMA_ADC_RXA (*(unsigned int volatile xdata *)0xFA17)
#define DMA_ADC_RXAH (*(unsigned char volatile xdata *)0xFA17)
#define DMA_ADC_RXAL (*(unsigned char volatile xdata *)0xFA18)
#define DMA_ADC_CFG2 (*(unsigned char volatile xdata *)0xFA19)
#define DMA_ADC_CHSW0 (*(unsigned char volatile xdata *)0xFA1A)
#define DMA_ADC_CHSW1 (*(unsigned char volatile xdata *)0xFA1B)

#define DMA_SPI_CFG (*(unsigned char volatile xdata *)0xFA20)
#define DMA_SPI_CR (*(unsigned char volatile xdata *)0xFA21)
#define DMA_SPI_STA (*(unsigned char volatile xdata *)0xFA22)
#define DMA_SPI_AMT (*(unsigned char volatile xdata *)0xFA23)
#define DMA_SPI_DONE (*(unsigned char volatile xdata *)0xFA24)
#define DMA_SPI_TXA (*(unsigned int volatile xdata *)0xFA25)
#define DMA_SPI_TXAH (*(unsigned char volatile xdata *)0xFA25)
#define DMA_SPI_TXAL (*(unsigned char volatile xdata *)0xFA26)
#define DMA_SPI_RXA (*(unsigned int volatile xdata *)0xFA27)
#define DMA_SPI_RXAH (*(unsigned char volatile xdata *)0xFA27)
#define DMA_SPI_RXAL (*(unsigned char volatile xdata *)0xFA28)
#define DMA_SPI_CFG2 (*(unsigned char volatile xdata *)0xFA29)

#define DMA_UR1T_CFG (*(unsigned char volatile xdata *)0xFA30)
#define DMA_UR1T_CR (*(unsigned char volatile xdata *)0xFA31)
#define DMA_UR1T_STA (*(unsigned char volatile xdata *)0xFA32)
#define DMA_UR1T_AMT (*(unsigned char volatile xdata *)0xFA33)
#define DMA_UR1T_DONE (*(unsigned char volatile xdata *)0xFA34)
#define DMA_UR1T_TXA (*(unsigned int volatile xdata *)0xFA35)
#define DMA_UR1T_TXAH (*(unsigned char volatile xdata *)0xFA35)
#define DMA_UR1T_TXAL (*(unsigned char volatile xdata *)0xFA36)
#define DMA_UR1R_CFG (*(unsigned char volatile xdata *)0xFA38)
#define DMA_UR1R_CR (*(unsigned char volatile xdata *)0xFA39)
#define DMA_UR1R_STA (*(unsigned char volatile xdata *)0xFA3A)
#define DMA_UR1R_AMT (*(unsigned char volatile xdata *)0xFA3B)
#define DMA_UR1R_DONE (*(unsigned char volatile xdata *)0xFA3C)
#define DMA_UR1R_RXA (*(unsigned int volatile xdata *)0xFA3D)
#define DMA_UR1R_RXAH (*(unsigned char volatile xdata *)0xFA3D)
#define DMA_UR1R_RXAL (*(unsigned char volatile xdata *)0xFA3E)

#define DMA_UR2T_CFG (*(unsigned char volatile xdata *)0xFA40)
#define DMA_UR2T_CR (*(unsigned char volatile xdata *)0xFA41)
#define DMA_UR2T_STA (*(unsigned char volatile xdata *)0xFA42)
#define DMA_UR2T_AMT (*(unsigned char volatile xdata *)0xFA43)
#define DMA_UR2T_DONE (*(unsigned char volatile xdata *)0xFA44)
#define DMA_UR2T_TXA (*(unsigned int volatile xdata *)0xFA45)
#define DMA_UR2T_TXAH (*(unsigned char volatile xdata *)0xFA45)
#define DMA_UR2T_TXAL (*(unsigned char volatile xdata *)0xFA46)
#define DMA_UR2R_CFG (*(unsigned char volatile xdata *)0xFA48)
#define DMA_UR2R_CR (*(unsigned char volatile xdata *)0xFA49)
#define DMA_UR2R_STA (*(unsigned char volatile xdata *)0xFA4A)
#define DMA_UR2R_AMT (*(unsigned char volatile xdata *)0xFA4B)
#define DMA_UR2R_DONE (*(unsigned char volatile xdata *)0xFA4C)
#define DMA_UR2R_RXA (*(unsigned int volatile xdata *)0xFA4D)
#define DMA_UR2R_RXAH (*(unsigned char volatile xdata *)0xFA4D)
#define DMA_UR2R_RXAL (*(unsigned char volatile xdata *)0xFA4E)

#define DMA_UR3T_CFG (*(unsigned char volatile xdata *)0xFA50)
#define DMA_UR3T_CR (*(unsigned char volatile xdata *)0xFA51)
#define DMA_UR3T_STA (*(unsigned char volatile xdata *)0xFA52)
#define DMA_UR3T_AMT (*(unsigned char volatile xdata *)0xFA53)
#define DMA_UR3T_DONE (*(unsigned char volatile xdata *)0xFA54)
#define DMA_UR3T_TXA (*(unsigned int volatile xdata *)0xFA55)
#define DMA_UR3T_TXAH (*(unsigned char volatile xdata *)0xFA55)
#define DMA_UR3T_TXAL (*(unsigned char volatile xdata *)0xFA56)
#define DMA_UR3R_CFG (*(unsigned char volatile xdata *)0xFA58)
#define DMA_UR3R_CR (*(unsigned char volatile xdata *)0xFA59)
#define DMA_UR3R_STA (*(unsigned char volatile xdata *)0xFA5A)
#define DMA_UR3R_AMT (*(unsigned char volatile xdata *)0xFA5B)
#define DMA_UR3R_DONE (*(unsigned char volatile xdata *)0xFA5C)
#define DMA_UR3R_RXA (*(unsigned int volatile xdata *)0xFA5D)
#define DMA_UR3R_RXAH (*(unsigned char volatile xdata *)0xFA5D)
#define DMA_UR3R_RXAL (*(unsigned char volatile xdata *)0xFA5E)

#define DMA_UR4T_CFG (*(unsigned char volatile xdata *)0xFA60)
#define DMA_UR4T_CR (*(unsigned char volatile xdata *)0xFA61)
#define DMA_UR4T_STA (*(unsigned char volatile xdata *)0xFA62)
#define DMA_UR4T_AMT (*(unsigned char volatile xdata *)0xFA63)
#define DMA_UR4T_DONE (*(unsigned char volatile xdata *)0xFA64)
#define DMA_UR4T_TXA (*(unsigned int volatile xdata *)0xFA65)
#define DMA_UR4T_TXAH (*(unsigned char volatile xdata *)0xFA65)
#define DMA_UR4T_TXAL (*(unsigned char volatile xdata *)0xFA66)
#define DMA_UR4R_CFG (*(unsigned char volatile xdata *)0xFA68)
#define DMA_UR4R_CR (*(unsigned char volatile xdata *)0xFA69)
#define DMA_UR4R_STA (*(unsigned char volatile xdata *)0xFA6A)
#define DMA_UR4R_AMT (*(unsigned char volatile xdata *)0xFA6B)
#define DMA_UR4R_DONE (*(unsigned char volatile xdata *)0xFA6C)
#define DMA_UR4R_RXA (*(unsigned int volatile xdata *)0xFA6D)
#define DMA_UR4R_RXAH (*(unsigned char volatile xdata *)0xFA6D)
#define DMA_UR4R_RXAL (*(unsigned char volatile xdata *)0xFA6E)

#define DMA_LCM_CFG (*(unsigned char volatile xdata *)0xFA70)
#define DMA_LCM_CR (*(unsigned char volatile xdata *)0xFA71)
#define DMA_LCM_STA (*(unsigned char volatile xdata *)0xFA72)
#define DMA_LCM_AMT (*(unsigned char volatile xdata *)0xFA73)
#define DMA_LCM_DONE (*(unsigned char volatile xdata *)0xFA74)
#define DMA_LCM_TXA (*(unsigned int volatile xdata *)0xFA75)
#define DMA_LCM_TXAH (*(unsigned char volatile xdata *)0xFA75)
#define DMA_LCM_TXAL (*(unsigned char volatile xdata *)0xFA76)
#define DMA_LCM_RXA (*(unsigned int volatile xdata *)0xFA77)
#define DMA_LCM_RXAH (*(unsigned char volatile xdata *)0xFA77)
#define DMA_LCM_RXAL (*(unsigned char volatile xdata *)0xFA78)

/////////////////////////////////////////////////
// USB Control Regiter
/////////////////////////////////////////////////

// sfr       USBCLK      =           0xdc;
// sfr       USBDAT      =           0xec;
// sfr       USBCON      =           0xf4;
// sfr       USBADR      =           0xfc;

// 使用方法:
//       char dat;
//
//       READ_USB(CSR0, dat);        //读USB寄存器
//       WRITE_USB(FADDR, 0x00);     //写USB寄存器

#define READ_USB(reg, dat)     \
    {                          \
        while (USBADR & 0x80); \
        USBADR = (reg) | 0x80; \
        while (USBADR & 0x80); \
        (dat) = USBDAT;        \
    }

#define WRITE_USB(reg, dat)    \
    {                          \
        while (USBADR & 0x80); \
        USBADR = (reg) & 0x7F; \
        USBDAT = (dat);        \
    }

#define USBBASE 0
#define FADDR (USBBASE + 0)
#define UPDATE 0x80
#define POWER (USBBASE + 1)
#define ISOUD 0x80
#define USBRST 0x08
#define USBRSU 0x04
#define USBSUS 0x02
#define ENSUS 0x01
#define INTRIN1 (USBBASE + 2)
#define EP5INIF 0x20
#define EP4INIF 0x10
#define EP3INIF 0x08
#define EP2INIF 0x04
#define EP1INIF 0x02
#define EP0IF 0x01
#define INTROUT1 (USBBASE + 4)
#define EP5OUTIF 0x20
#define EP4OUTIF 0x10
#define EP3OUTIF 0x08
#define EP2OUTIF 0x04
#define EP1OUTIF 0x02
#define INTRUSB (USBBASE + 6)
#define SOFIF 0x08
#define RSTIF 0x04
#define RSUIF 0x02
#define SUSIF 0x01
#define INTRIN1E (USBBASE + 7)
#define EP5INIE 0x20
#define EP4INIE 0x10
#define EP3INIE 0x08
#define EP2INIE 0x04
#define EP1INIE 0x02
#define EP0IE 0x01
#define INTROUT1E (USBBASE + 9)
#define EP5OUTIE 0x20
#define EP4OUTIE 0x10
#define EP3OUTIE 0x08
#define EP2OUTIE 0x04
#define EP1OUTIE 0x02
#define INTRUSBE (USBBASE + 11)
#define SOFIE 0x08
#define RSTIE 0x04
#define RSUIE 0x02
#define SUSIE 0x01
#define FRAME1 (USBBASE + 12)
#define FRAME2 (USBBASE + 13)
#define INDEX (USBBASE + 14)
#define INMAXP (USBBASE + 16)
#define CSR0 (USBBASE + 17)
#define SSUEND 0x80
#define SOPRDY 0x40
#define SDSTL 0x20
#define SUEND 0x10
#define DATEND 0x08
#define STSTL 0x04
#define IPRDY 0x02
#define OPRDY 0x01
#define INCSR1 (USBBASE + 17)
#define INCLRDT 0x40
#define INSTSTL 0x20
#define INSDSTL 0x10
#define INFLUSH 0x08
#define INUNDRUN 0x04
#define INFIFONE 0x02
#define INIPRDY 0x01
#define INCSR2 (USBBASE + 18)
#define INAUTOSET 0x80
#define INISO 0x40
#define INMODEIN 0x20
#define INMODEOUT 0x00
#define INENDMA 0x10
#define INFCDT 0x08
#define OUTMAXP (USBBASE + 19)
#define OUTCSR1 (USBBASE + 20)
#define OUTCLRDT 0x80
#define OUTSTSTL 0x40
#define OUTSDSTL 0x20
#define OUTFLUSH 0x10
#define OUTDATERR 0x08
#define OUTOVRRUN 0x04
#define OUTFIFOFUL 0x02
#define OUTOPRDY 0x01
#define OUTCSR2 (USBBASE + 21)
#define OUTAUTOCLR 0x80
#define OUTISO 0x40
#define OUTENDMA 0x20
#define OUTDMAMD 0x10
#define COUNT0 (USBBASE + 22)
#define OUTCOUNT1 (USBBASE + 22)
#define OUTCOUNT2 (USBBASE + 23)
#define FIFO0 (USBBASE + 32)
#define FIFO1 (USBBASE + 33)
#define FIFO2 (USBBASE + 34)
#define FIFO3 (USBBASE + 35)
#define FIFO4 (USBBASE + 36)
#define FIFO5 (USBBASE + 37)
#define UTRKCTL (USBBASE + 48)
#define UTRKSTS (USBBASE + 49)

////////////////////////////// NVIC //////////////////////////////

#define INT0_VECTOR 0    // 0003H
#define TMR0_VECTOR 1    // 000BH
#define INT1_VECTOR 2    // 0013H
#define TMR1_VECTOR 3    // 001BH
#define UART1_VECTOR 4   // 0023H
#define ADC_VECTOR 5     // 002BH
#define LVD_VECTOR 6     // 0033H
#define UART2_VECTOR 8   // 0043H
#define SPI_VECTOR 9     // 004BH
#define INT2_VECTOR 10   // 0053H
#define INT3_VECTOR 11   // 005BH
#define TMR2_VECTOR 12   // 0063H
#define USER_VECTOR 13   // 006BH
#define INT4_VECTOR 16   // 0083H
#define UART3_VECTOR 17  // 008BH
#define UART4_VECTOR 18  // 0093H
#define TMR3_VECTOR 19   // 009BH
#define TMR4_VECTOR 20   // 00A3H
#define CMP_VECTOR 21    // 00ABH
#define I2C_VECTOR 24    // 00C3H
#define USB_VECTOR 25    // 00CBH
#define PWMA_VECTOR 26   // 00D3H
#define PWMB_VECTOR 27   // 00DBH

#define TKSU_VECTOR 35      // 011BH
#define RTC_VECTOR 36       // 0123H
#define P0INT_VECTOR 37     // 012BH
#define P1INT_VECTOR 38     // 0133H
#define P2INT_VECTOR 39     // 013BH
#define P3INT_VECTOR 40     // 0143H
#define P4INT_VECTOR 41     // 014BH
#define P5INT_VECTOR 42     // 0153H
#define P6INT_VECTOR 43     // 015BH
#define P7INT_VECTOR 44     // 0163H
#define DMA_M2M_VECTOR 47   // 017BH
#define DMA_ADC_VECTOR 48   // 0183H
#define DMA_SPI_VECTOR 49   // 018BH
#define DMA_UR1T_VECTOR 50  // 0193H
#define DMA_UR1R_VECTOR 51  // 019BH
#define DMA_UR2T_VECTOR 52  // 01A3H
#define DMA_UR2R_VECTOR 53  // 01ABH
#define DMA_UR3T_VECTOR 54  // 01B3H
#define DMA_UR3R_VECTOR 55  // 01BBH
#define DMA_UR4T_VECTOR 56  // 01C3H
#define DMA_UR4R_VECTOR 57  // 01CBH
#define DMA_LCM_VECTOR 58   // 01D3H
#define LCM_VECTOR 59       // 01DBH

////////////////////////////// HELPERS //////////////////////////////

#define enable_extram() AUXR &= ~EXTRAM  // 访问内部扩展RAM
#define disable_extram() AUXR |= EXTRAM  // 禁用内部扩展RAM

#define enable_xsfr() P_SW2 |= EAXFR   /* MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展SFR(XSFR) */
#define disable_xsfr() P_SW2 &= ~EAXFR /* MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展RAM(XRAM) */

#define sys_reset() IAP_CONTR = (SWRST | SWBS) /* 复位到ISP */

#define enable_irq() EA = 1   // 使能全局中断
#define disable_irq() EA = 0  // 禁用全局中断

////////////////////////////// Timer 0 //////////////////////////////

/**
 * | T0_M1 | T0_M0 | 计数器/定时器0工作方式                                                         |
 * |-------|-------|--------------------------------------------------------------------------------|
 * |   0   |   0   | 16位自动重载: 溢出时系统自动将内部16位重载寄存器的重载值装入TH0/TL0            |
 * |   0   |   1   | 16位不自动重载：溢出时从0开始计数                                              |
 * |   1   |   0   | 8位自动重载：TL0计数器溢出时，系统自动将TH0的重载值装入TL0                     |
 * |   1   |   1   | 不可屏蔽中断的16位自动重载模式：中断优先级最高，可用作操作系统节拍或监控定时器 |
 */

/* 定时器0模式0: 16位自动重载 */
#define t0_mode0_16bit_auto_reload() \
    do {                             \
        TMOD &= ~(T0_M1 | T0_M0);    \
    } while (0)

/* 定时器0模式1: 16位不自动重载 */
#define t0_mode1_16bit() \
    do {                 \
        TMOD &= ~T0_M1;  \
        TMOD |= T0_M0;   \
    } while (0)

/* 定时器0模式2: 8位自动重载 */
#define t0_mode2_8bit_auto_reload() \
    do {                            \
        TMOD |= T0_M1;              \
        TMOD &= ~T0_M0;             \
    } while (0)

/* 定时器0模式3: 不可屏蔽中断的16位自动重载 */
#define t0_mode3_16bit_auto_reload_nmi() \
    do {                                 \
        TMOD |= T0_M1;                   \
        TMOD |= T0_M0;                   \
    } while (0)

#define t0_run() TR0 = 1                             // 启动定时器0
#define t0_stop() TR0 = 0                            // 停止定时器0
#define t0_1t() AUXR |= T0x12                        // 定时器0工作在1T模式
#define t0_12t() AUXR &= ~T0x12                      // 定时器0工作在12T模式
#define t0_as_timer() TMOD &= ~T0_CT                 // 定时器0用作定时器
#define t0_as_counter() TMOD |= T0_CT                // 定时器0用作计数器
#define t0_enable_clk_output() INT_CLKO |= T0CLKO    // 定时器0溢出脉冲是否输出到INT0引脚
#define t0_disable_clk_output() INT_CLKO &= ~T0CLKO  // 定时器0溢出脉冲不输出到INT0引脚
#define t0_enable_irq() ET0 = 1                      // 使能定时器0中断
#define t0_disable_irq() ET0 = 0                     // 禁用定时器0中断

/* 定时器0装载初值 */
#define t0_load(load)               \
    do {                            \
        TL0 = (load) & 0xFF;        \
        TH0 = ((load) >> 8) & 0xFF; \
    } while (0)

// T0 中断优先级0最低
#define t0_nvic_priority_0() \
    do {                     \
        IPH &= ~PT0H;        \
        PT0 = 0;             \
    } while (0)

// T0 中断优先级1低
#define t0_nvic_priority_1() \
    do {                     \
        IPH &= ~PT0H;        \
        PT0 = 1;             \
    } while (0)

// T0 中断优先级2高
#define t0_nvic_priority_2() \
    do {                     \
        IPH |= PT0H;         \
        PT0 = 0;             \
    } while (0)

// T0 中断优先级3最高
#define t0_nvic_priority_3() \
    do {                     \
        IPH |= PT0H;         \
        PT0 = 1;             \
    } while (0)

////////////////////////////// Timer 1 //////////////////////////////

/**
 * | T1_M1 | T1_M0 | 计数器/定时器1工作方式                                              |
 * |-------|-------|---------------------------------------------------------------------|
 * |   0   |   0   | 16位自动重载: 溢出时系统自动将内部16位重载寄存器的重载值装入TH1/TL1 |
 * |   0   |   1   | 16位不自动重载：溢出时从0开始计数                                   |
 * |   1   |   0   | 8位自动重载：TL1计数器溢出时，系统自动将TH1的重载值装入TL1          |
 * |   1   |   1   | T1停止工作                                                          |
 */

/* 定时器1模式0: 16位自动重载 */
#define t1_mode0_16bit_auto_reload() \
    do {                             \
        TMOD &= ~(T1_M1 | T1_M0);    \
    } while (0)

/* 定时器1模式1: 16位不自动重载 */
#define t1_mode1_16bit() \
    do {                 \
        TMOD &= ~T1_M1;  \
        TMOD |= T1_M0;   \
    } while (0)

/* 定时器1模式2: 8位自动重载 */
#define t1_mode2_8bit_auto_reload() \
    do {                            \
        TMOD |= T1_M1;              \
        TMOD &= ~T1_M0;             \
    } while (0)

/* 定时器1模式3: T1停止工作 */
#define t1_mode3_stop() \
    do {                \
        TMOD |= T1_M1;  \
        TMOD |= T1_M0;  \
    } while (0)

#define t1_run() TR1 = 1                             // 启动定时器1
#define t1_stop() TR1 = 0                            // 停止定时器1
#define t1_1t() AUXR |= T1x12                        // 定时器1工作在1T模式
#define t1_12t() AUXR &= ~T1x12                      // 定时器1工作在12T模式
#define t1_as_timer() TMOD &= ~T1_CT                 // 定时器1用作定时器
#define t1_as_counter() TMOD |= T1_CT                // 定时器1用作计数器
#define t1_enable_clk_output() INT_CLKO |= T1CLKO    // 定时器1溢出脉冲是否输出到INT1引脚
#define t1_disable_clk_output() INT_CLKO &= ~T1CLKO  // 定时器1溢出脉冲不输出到INT1引脚
#define t1_enable_irq() ET1 = 1                      // 使能定时器1中断
#define t1_disable_irq() ET1 = 0                     // 禁用定时器1中断

/* 定时器1装载初值 */
#define t1_load(load)               \
    do {                            \
        TL1 = (load) & 0xFF;        \
        TH1 = ((load) >> 8) & 0xFF; \
    } while (0)

// T1 中断优先级0最低
#define t1_nvic_priority_0() \
    do {                     \
        IPH &= ~PT1H;        \
        PT1 = 0;             \
    } while (0)

// T1 中断优先级1低
#define t1_nvic_priority_1() \
    do {                     \
        IPH &= ~PT1H;        \
        PT1 = 1;             \
    } while (0)

// T1 中断优先级2高
#define t1_nvic_priority_2() \
    do {                     \
        IPH |= PT1H;         \
        PT1 = 0;             \
    } while (0)

// T1 中断优先级3最高
#define t1_nvic_priority_3() \
    do {                     \
        IPH |= PT1H;         \
        PT1 = 1;             \
    } while (0)

////////////////////////////// Timer 2 //////////////////////////////

/**
 * T2 只有一种工作模式：16位自动重装，有中断但不可设置中断优先级，
 * 考虑 TM2PS预分频，可以作为24位定时器使用
 * | 速度 |             定时器周期计算公式                  |
 * | ---- | ----------------------------------------------- |
 * |  1T  | 周期 = (65536 - load) / [fosc / (TM2PS+1)]      |
 * | 12T  | 周期 = (65536 - load) / [fosc / (TM2PS+1)] * 12 |
 */

#define t2_run() AUXR |= T2R                         // 启动定时器2
#define t2_stop() AUXR &= ~T2R                       // 停止定时器2
#define t2_1t() AUXR |= T2x12                        // 定时器2工作在1T模式
#define t2_12t() AUXR &= ~T2x12                      // 定时器2工作在12T模式
#define t2_as_timer() TMOD &= ~T2_CT                 // 定时器2用作定时器
#define t2_as_counter() TMOD |= T2_CT                // 定时器2用作计数器
#define t2_enable_clk_output() INT_CLKO |= T2CLKO    // 定时器2溢出脉冲是否输出到INT2引脚
#define t2_disable_clk_output() INT_CLKO &= ~T2CLKO  // 定时器2溢出脉冲不输出到INT2引脚
#define t2_enable_irq() IE2 |= ET2                   // 使能定时器2中断
#define t2_disable_irq() IE2 &= ~ET2                 // 禁用定时器2中断
#define t2_prescaler(x) TM2PS = x                    // 设置定时器2预分频器

/* 定时器2装载初值 */
#define t2_load(load)               \
    do {                            \
        T2L = (load) & 0xFF;        \
        T2H = ((load) >> 8) & 0xFF; \
    } while (0)

////////////////////////////// Timer 3/4 //////////////////////////////

/**
 * T3/T4 只有一种工作模式：16位自动重装，有中断但不可设置中断优先级，
 * 考虑 T3T4PS预分频，可以作为24位定时器使用
 * | 速度 |             定时器周期计算公式                   |
 * | ---- | ------------------------------------------------ |
 * |  1T  | 周期 = (65536 - load) / [fosc / (T3T4PS+1)]      |
 * | 12T  | 周期 = (65536 - load) / [fosc / (T3T4PS+1)] * 12 |
 */

#define t3_run() T4T3M |= T3R                     // 启动定时器3
#define t3_stop() T4T3M &= ~T3R                   // 停止定时器3
#define t3_1t() T4T3M |= T3x12                    // 定时器3工作在1T模式
#define t3_12t() T4T3M &= ~T3x12                  // 定时器3工作在12T模式
#define t3_as_timer() T4T3M &= ~T3_CT             // 定时器3用作定时器
#define t3_as_counter() T4T3M |= T3_CT            // 定时器3用作计数器
#define t3_enable_clk_output() T4T3M |= T3CLKO    // 定时器3溢出脉冲是否输出到INT3引脚
#define t3_disable_clk_output() T4T3M &= ~T3CLKO  // 定时器3溢出脉冲不输出到INT3引脚
#define t3_enable_irq() IE2 |= ET3                // 使能定时器3中断
#define t3_disable_irq() IE2 &= ~ET3              // 禁用定时器3中断
#define t3_prescaler(x) TM3PS = x                 // 设置定时器3预分频器

/* 定时器3装载初值 */
#define t3_load(load)               \
    do {                            \
        T3L = (load) & 0xFF;        \
        T3H = ((load) >> 8) & 0xFF; \
    } while (0)

#define t4_run() T4T3M |= T4R                     // 启动定时器4
#define t4_stop() T4T3M &= ~T4R                   // 停止定时器4
#define t4_1t() T4T3M |= T4x12                    // 定时器4工作在1T模式
#define t4_12t() T4T3M &= ~T4x12                  // 定时器4工作在12T模式
#define t4_as_timer() T4T3M &= ~T4_CT             // 定时器4用作定时器
#define t4_as_counter() T4T3M |= T4_CT            // 定时器4用作计数器
#define t4_enable_clk_output() T4T3M |= T4CLKO    // 定时器4溢出脉冲是否输出到INT4引脚
#define t4_disable_clk_output() T4T3M &= ~T4CLKO  // 定时器4溢出脉冲不输出到INT4引脚
#define t4_enable_irq() IE2 |= ET4                // 使能定时器4中断
#define t4_disable_irq() IE2 &= ~ET4              // 禁用定时器4中断
#define t4_prescaler(x) TM4PS = x                 // 设置定时器4预分频器

/* 定时器4装载初值 */
#define t4_load(load)               \
    do {                            \
        T4L = (load) & 0xFF;        \
        T4H = ((load) >> 8) & 0xFF; \
    } while (0)

////////////////////////////// UARTx //////////////////////////////

// 以主频 fosc 和波特率 buad 计算定时器装载值，定时器工作在1T模式
#define uart_calc_load_1t(fosc, buad) (65536U - fosc / 4 / buad)
// 以主频 fosc 和波特率 buad 计算定时器装载值，定时器工作在12T模式
#define uart_calc_load_12t(fosc, buad) (65536U - fosc / 48 / buad)

////////////////////////////// UART1 //////////////////////////////

/**
 * 串口1功能脚切换
 * | S1_S[1:0] | RxD  | TxD  |
 * |-----------|------|------|
 * |    00     | P3.0 | P3.1 |
 * |    01     | P3.6 | P3.7 |
 * |    10     | P1.6 | P1.7 |
 * |    11     | P4.3 | P4.4 |
 */

#define uart1_use_p30_p31() P_SW1 &= ~S1_S_MASK
#define uart1_use_p36_p37() P_SW1 = (P_SW1 & ~S1_S_MASK) | 0x40
#define uart1_use_p16_p17() P_SW1 = (P_SW1 & ~S1_S_MASK) | 0x80
#define uart1_use_p43_p44() P_SW1 |= S1_S_MASK

#define uart1_brt_use_timer1() AUXR &= ~S1ST2  // 串口1使用定时器1作为波特率发生器
#define uart1_brt_use_timer2() AUXR |= S1ST2   // 串口1使用定时器2作为波特率发生器

/**
 * |SM0|SM1|串口1工作模式|功能说明               |
 * |---|---|-------------|-----------------------|
 * | 0 | 0 | 模式0       | 同步移位串行方式      |
 * | 0 | 1 | 模式1       | 可变波特率8位数据方式 |
 * | 1 | 0 | 模式2       | 固定波特率9位数据方式 |
 * | 1 | 1 | 模式3       | 可变波特率9位数据方式 |
 */

#define uart1_mode0_shift_right() SM0_FE = SM1 = 0  // 串口1模式0：同步移位串行

// 串口1模式1：8位数据位，可变波特率
#define uart1_mode1_8bit_brtx() \
    do {                        \
        SM0_FE = 0;             \
        SM1 = 1;                \
    } while (0)

// 串口1模式2：9位数据位，固定波特率
#define uart1_mode2_9bit() \
    do {                   \
        SM0_FE = 1;        \
        SM1 = 0;           \
    } while (0)

#define uart1_mode3_9bit_brtx() SM0_FE = SM1 = 1  // 串口1模式3：9位数据位，可变波特率

/* 串口1中断优先级设置
 * | PSH | PS | 中断优先级 |
 * |-----|----|----------  |
 * | 0   | 0  |     0      |
 * | 0   | 1  |     1      |
 * | 1   | 0  |     2      |
 * | 1   | 1  |     3      |
 */

// 串口1优先级0：最低
#define uart1_nvic_priority0() \
    do {                       \
        IPH &= ~PSH;           \
        PS = 0;                \
    } while (0)

// 串口1优先级1：低
#define uart1_nvic_priority1() \
    do {                       \
        IPH &= ~PSH;           \
        PS = 1;                \
    } while (0)

// 串口1优先级2：高
#define uart1_nvic_priority2() \
    do {                       \
        IPH |= PSH;            \
        PS = 0;                \
    } while (0)

// 串口1优先级3：最高
#define uart1_nvic_priority3() \
    do {                       \
        IPH |= PSH;            \
        PS = 1;                \
    } while (0)

#define uart1_enable_irq() ES = 1     // 使能串口1中断
#define uart1_disable_irq() ES = 0    // 禁用串口1中断
#define uart1_enable_recv() REN = 1   // 使能串口1接收
#define uart1_disable_recv() REN = 0  // 禁用串口1接收

////////////////////////////// UART2 //////////////////////////////

// 串口2 只能使用定时器2作为波特率发生器

#define uart2_mode0_8bit_brtx() S2CON &= ~S2SM0  // 串口2模式0：8位数据位，可变波特率
#define uart2_mode1_9bit_brtx() S2CON |= S2SM0   // 串口2模式1：9位数据位，可变波特率

/* 串口2中断优先级设置
 * | PS2H | PS2 | 中断优先级 |
 * |------|-----|----------  |
 * |  0   |  0  |     0      |
 * |  0   |  1  |     1      |
 * |  1   |  0  |     2      |
 * |  1   |  1  |     3      |
 */

// 串口2优先级0：最低
#define uart2_nvic_priority_0() \
    do {                        \
        IP2H &= ~PS2H;          \
        IP2 &= ~PS2;            \
    } while (0)

// 串口2优先级1：低
#define uart2_nvic_priority_1() \
    do {                        \
        IP2H &= ~PS2H;          \
        IP2 |= PS2;             \
    } while (0)

// 串口2优先级2：高
#define uart2_nvic_priority_2() \
    do {                        \
        IP2H |= PS2H;           \
        IP2 &= ~PS2;            \
    } while (0)

// 串口2优先级3：最高
#define uart2_nvic_priority_3() \
    do {                        \
        IP2H |= PS2H;           \
        IP2 |= PS2;             \
    } while (0)

#define uart2_enable_irq() IE2 |= ES2         // 使能串口2中断
#define uart2_disable_irq() IE2 &= ~ES2       // 禁用串口2中断
#define uart2_enable_recv() S2CON |= S2REN    // 使能串口2接收
#define uart2_disable_recv() S2CON &= ~S2REN  // 禁用串口2接收
#define uart2_tst_ri() (S2CON & S2RI)         // 串口2接收中断标志
#define uart2_tst_ti() (S2CON & S2TI)         // 串口2发送中断标志
#define uart2_clr_ri() S2CON &= ~S2RI         // 清除串口2接收中断标志
#define uart2_clr_ti() S2CON &= ~S2TI         // 清除串口2发送中断标志
#define RI2 uart2_tst_ri()                    // 串口2接收中断标志
#define TI2 uart2_tst_ti()                    // 串口2发送中断标志
#define CLR_RI2() uart2_clr_ri()              // 清除串口2接收中断标志
#define CLR_TI2() uart2_clr_ti()              // 清除串口2发送中断标志

////////////////////////////// IIC //////////////////////////////

/**
 * | I2C_S[1:0] | SCL  | SDA  |
 * |------------|------|------|
 * |     00     | P1.5 | P1.4 |
 * |     01     | P2.5 | P2.4 |
 * |     10     | P7.7 | P7.6 |
 * |     11     | P3.2 | P3.3 |
 */

// I2C_S = 00, SCL=P1.5, SDA=P1.4
#define i2c_s00_scl_p15_sda_p14() P_SW2 &= ~I2C_S_MASK

// I2C_S = 01, SCL=P2.5, SDA=P2.4
#define i2c_s01_scl_p25_sda_p24() \
    do {                          \
        P_SW2 &= ~I2C_S_MASK;     \
        P_SW2 |= 0x10;            \
    } while (0)

// I2C_S = 10, SCL=P7.7, SDA=P7.6
#define i2c_s10_scl_p77_sda_p76() \
    do {                          \
        P_SW2 &= ~I2C_S_MASK;     \
        P_SW2 |= 0x20;            \
    } while (0);

// I2C_S = 11, SCL=P3.2, SDA=P3.3
#define i2c_s11_scl_p32_sda_p33() \
    do {                          \
        P_SW2 &= ~I2C_S_MASK;     \
        P_SW2 |= 0x30;            \
    } while (0)

////////////////////////////// IAP //////////////////////////////

#define IAP_BASE_ADDR 0x0000  // IAP 基地址
#define IAP_SECTOR_SIZE 512   // IAP 扇区大小

// 四舍五入函数
#ifndef round
#define round(x) ((x) + ((x) >= 0 ? 0.5 : -0.5))
#endif

#define iap_calc_tps(fosc) round((fosc) / 1000000)  // 以主频计算IAP_TPS, 放心用，编译器会自动计算出立即数
#define iap_tps(tps) IAP_TPS = tps                  // 设置IAP_TPS
#define iap_enable() IAP_CONTR = IAPEN              // 启用 IAP 功能
#define iap_disable() (IAP_CONTR &= ~IAPEN)         // 禁用 IAP 功能
#define iap_idle()                                       \
    do {                                                 \
        IAP_CONTR = 0;    /* disable iap */              \
        IAP_CMD = 0;      /* clear cmd to standby */     \
        IAP_TRIG = 0;     /* clear trigger register */   \
        IAP_ADDRH = 0x80; /* set addr to non-iap area */ \
        IAP_ADDRL = 0x00; /* to avoid false operation */ \
    } while (0)

/**
 * |MS1|MS0| 命令、操作 模式选择                           |
 * |---|---|-----------------------------------------------|
 * | 0 | 0 | standby 待机模式，无ISP操作                   |
 * | 0 | 1 | 从用户程序区对 DATA FLASH/EEPROM 进行字节读   |
 * | 1 | 0 | 从用户程序区对 DATA FLASH/EEPROM 进行字节写   |
 * | 1 | 1 | 从用户程序区对 DATA FLASH/EEPROM 进行字节擦除 |
 */

#define iap_cmd_standby() IAP_CMD = 0x00  // 待机模式，无ISP操作
#define iap_cmd_read() IAP_CMD = 0x01     // 从用户程序区对 DATA FLASH/EEPROM 进行字节读
#define iap_cmd_write() IAP_CMD = 0x02    // 从用户程序区对 DATA FLASH/EEPROM 进行字节写
#define iap_cmd_erase() IAP_CMD = 0x03    // 从用户程序区对 DATA FLASH/EEPROM 进行字节擦除

// 设置 IAP 地址
#define iap_set_addr(addr)                \
    do {                                  \
        IAP_ADDRH = ((addr) >> 8) & 0xFF; \
        IAP_ADDRL = (addr) & 0xFF;        \
    } while (0)

// IAP 触发操作
/*
    官方例程还包括以下延时操作，待测试
    _nop_();
    _nop_();
    _nop_();
    _nop_();
*/
#define iap_trigger()    \
    do {                 \
        IAP_TRIG = 0x5A; \
        IAP_TRIG = 0xA5; \
    } while (0)

// IAP 是否失败
#define iap_is_fail() (IAP_CONTR & CMDFAIL)

// 清理 IAP 失败状态
#define iap_clear_fail() (IAP_CONTR &= ~CMDFAIL)  // 清除 IAP 失败状态


//////////////////////// IAP 函数 //////////////////////////
// 注意在执行操作之前调用 iap_tps(tps)

#ifdef __cplusplus
extern "C" {
#endif

void iap_erase_page(uint16_t addr);
bool iap_erase_page_check(uint16_t addr);
uint8_t iap_read_byte(uint16_t addr);
void iap_write_byte(uint16_t addr, uint8_t b);
bool iap_write_byte_check(uint16_t addr, uint8_t b);
void iap_read_bytes(uint16_t addr, uint8_t *buf, uint16_t len);
void iap_write_bytes(uint16_t addr, uint8_t *buf, uint16_t len);
bool iap_write_bytes_check(uint16_t addr, uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

//////////////////////// CHIPID //////////////////////////

#define STC_MCU_ID_MAP(XX)      \
    XX(0xF721, STC8H1K16)       \
    XX(0xF722, STC8H1K20)       \
    XX(0xF723, STC8H1K24)       \
    XX(0xF724, STC8H1K28)       \
    XX(0xF725, STC8H1K33)       \
    XX(0xF7D1, STC8C2K16S4)     \
    XX(0xF7D2, STC8C2K32S4)     \
    XX(0xF7D5, STC8C2K48S4)     \
    XX(0xF7D3, STC8C2K60S4)     \
    XX(0xF7D4, STC8C2K64S4)     \
    XX(0xF821, STC8H4K16LCD)    \
    XX(0xF822, STC8H4K32LCD)    \
    XX(0xF825, STC8H4K48LCD)    \
    XX(0xF823, STC8H4K60LCD)    \
    XX(0xF824, STC8H4K64LCD)    \
    XX(0xF7C9, STC8H4K16TLR)    \
    XX(0xF7CA, STC8H4K32TLR)    \
    XX(0xF7CD, STC8H4K48TLR)    \
    XX(0xF7CB, STC8H4K60TLR)    \
    XX(0xF7CC, STC8H4K64TLR)    \
    XX(0xF741, STC8H3K16S4)     \
    XX(0xF742, STC8H3K32S4)     \
    XX(0xF745, STC8H3K48S4)     \
    XX(0xF743, STC8H3K60S4)     \
    XX(0xF744, STC8H3K64S4)     \
    XX(0xF749, STC8H3K16S2)     \
    XX(0xF74A, STC8H3K32S2)     \
    XX(0xF74D, STC8H3K48S2)     \
    XX(0xF74B, STC8H3K60S2)     \
    XX(0xF74C, STC8H3K64S2)     \
    XX(0xF781, STC8H8K16U)      \
    XX(0xF782, STC8H8K32U)      \
    XX(0xF785, STC8H8K48U)      \
    XX(0xF783, STC8H8K60U)      \
    XX(0xF784, STC8H8K64U)      \
    XX(0xF761, STC8G2K16S4)     \
    XX(0xF762, STC8G2K32S4)     \
    XX(0xF765, STC8G2K48S4)     \
    XX(0xF763, STC8G2K60S4)     \
    XX(0xF764, STC8G2K64S4)     \
    XX(0xF769, STC8G2K16S2)     \
    XX(0xF76A, STC8G2K32S2)     \
    XX(0xF76D, STC8G2K48S2)     \
    XX(0xF76B, STC8G2K60S2)     \
    XX(0xF76C, STC8G2K64S2)     \
    XX(0xF793, STC8G1KO8A_8PIN) \
    XX(0xF794, STC8G1K08_8PIN)  \
    XX(0xF795, STC8G1K10A_8PIN) \
    XX(0xF796, STC8G1K12A_8PIN) \
    XX(0xF797, STC8G1K17A_8PIN) \
    XX(0xF7A3, STC8G1K06_8PIN)  \
    XX(0xF7A4, STC8G1K08_8PIN)  \
    XX(0xF7A5, STC8G1K10_8PIN)  \
    XX(0xF7A6, STC8G1K12_8PIN)  \
    XX(0xF7A7, STC8G1K17_8PIN)  \
    XX(0xF7F1, STC8A8K16D4)     \
    XX(0xF7F2, STC8A8K32D4)     \
    XX(0xF7F5, STC8A8K48D4)     \
    XX(0xF7F3, STC8A8K60D4)     \
    XX(0xF7F4, STC8A8K64D4)     \
    XX(0xF843, STC8H1K06T)      \
    XX(0xF844, STC8H1K08T)      \
    XX(0xF845, STC8H1K10T)      \
    XX(0xF846, STC8H1K12T)      \
    XX(0xF847, STC8H1K17T)

#define STC_PACKAGE_MAP(XX) \
    XX(0x00, DIP8)          \
    XX(0x01, SOP8)          \
    XX(0x02, DFN8)          \
    XX(0x10, DIP16)         \
    XX(0x11, SOP16)         \
    XX(0x20, DIP18)         \
    XX(0x21, SOP18)         \
    XX(0x30, DIP20)         \
    XX(0x31, SOP20)         \
    XX(0x32, TSSOP20)       \
    XX(0x33, LSSOP20)       \
    XX(0x34, QFN20)         \
    XX(0x40, SKDIP28)       \
    XX(0x41, SOP28)         \
    XX(0x42, TSSOP28)       \
    XX(0x43, QFN28)         \
    XX(0x50, SOP32)         \
    XX(0x51, LQFP32)        \
    XX(0x52, QFN32)         \
    XX(0x53, PLCC32)        \
    XX(0x54, QFN32S)        \
    XX(0x60, PDIP40)        \
    XX(0x70, LQFP44)        \
    XX(0x71, PLCC44)        \
    XX(0x72, PQFP44)        \
    XX(0x80, LQFP48)        \
    XX(0x81, QFN48)         \
    XX(0x90, LQFP64)        \
    XX(0x91, LQFP64S)       \
    XX(0x92, LQFP64L)       \
    XX(0x93, LQFP64M)       \
    XX(0x94, QFN64)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(VSCODE) || !defined(__C51__)
#pragma pack(1)
#endif
// STC 7位全球唯一ID号
typedef union {
    uint8_t b[7];
    struct {
        uint16_t mcu;              // MCU ID
        uint16_t test_machine_no;  // 测试机台编号
        uint8_t test_no[3];        // 测试流水编号
    } st;
} stc_unique_id_t;

typedef union {
    uint8_t b[32];
    struct {
        stc_unique_id_t unique_id;    // 7位全球唯一ID号
        uint16_t bgv;                 // 内部1.19V参考信号源电压值
        uint16_t inner_32k_irc_freq;  // 内部32K的IRC震荡频率
        uint8_t id11;
        uint8_t id12;
        uint8_t id13;
        uint8_t id14;
        uint8_t id15;
        uint8_t id16;
        uint8_t id17;
        uint8_t id18;
        uint8_t id19;
        uint8_t id20;
        uint8_t vrtrim_6m;                       // 6M频段的VRTRIM参数
        uint8_t vrtrim_10m;                      // 10M频段的VRTRIM参数
        uint8_t vrtrim_27m;                      // 27M频段的VRTRIM参数
        uint8_t vrtrim_44m;                      // 44M频段的VRTRIM参数
        uint8_t id25;                            // 00H
        uint8_t user_program_space_end_addr_hi;  // 用户程序空间结束地址高字节
        struct {
            uint8_t year;   // 芯片测试时间年 BCD
            uint8_t month;  // 芯片测试时间月 BCD
            uint8_t day;    // 芯片测试时间日 BCD
        } test_date;
        uint8_t package;  // 封装编号
        uint8_t id31;     // 5AH
    } st;
} stc_chipid_t;
#if defined(VSCODE) || !defined(__C51__)
#pragma pack()
#endif

#ifdef __C51__
// STC 同型号的芯片有不同版本，如A,B,C,D等
// char stc_chip_version();
#define stc_chip_version() (IAP_ADDRL = 2, DID + 'A')

#define STC_CHIPID() (*(volatile stc_chipid_t xdata *)CHIPID)

// STC 7位全球唯一ID号
#define STC_UNIQUE_ID() (*(volatile stc_unique_id_t xdata *)(CHIPID))

// STC 内部1.19V参考信号源电压值
#define STC_BGV() (*(volatile uint16_t xdata *)0xFDE7)

// STC 内部32K的IRC震荡频率
#define STC_32K_IRC_FREQ() (*(volatile uint16_t xdata *)0xFDE9)

// STC 测试时间年月日BCD
#define STC_TEST_YEAR() (*(volatile uint8_t xdata *)0xFDFB)
#define STC_TEST_MONTH() (*(volatile uint8_t xdata *)0xFDFC)
#define STC_TEST_DAY() (*(volatile uint8_t xdata *)0xFDFD)

// STC 封装编号
#define STC_PACKAGE() (*(volatile uint8_t xdata *)0xFDFE)

#endif /* __C51__ */

#ifdef __cplusplus
}
#endif

#endif /* __STC8H_H__ */
