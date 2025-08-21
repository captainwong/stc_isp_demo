/**
 * chip: STC8H8K64U
 * ram: 256B idata, 8KB xdata
 * flash: 64KB
 *   - 4KB for bootloader
 *   - 60KB for application
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#define STC_RAM_SIZE 0x2000  // STC8H8K64U has 8KB xdata

#define LDR_SIZE 0x1000     // bootloader flash space = 4KB
#define LDR_VERSION 0x0100  // bootloader version 1.0

#define DFU_TAG 0x12ABCD34UL  // force DFU mode
#define DFU_ADDR (STC_RAM_SIZE - sizeof(DFU_TAG))

#endif /* __COMMON_H__ */
