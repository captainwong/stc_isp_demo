STC IAP DEMO
============

[English](./README.md)

## 0. STC CPU 启动过程

上电后，执行STC自带内部ISP程序，没有检测到上位机要求下载的串口指令时，转入用户程序.

`PC=0000H`，即从`ROM`空间第`0`字节取指令执行（也可以类比为 `stm32` 的 `Reset_Handler` 即复位中断入口），一般这里是 `02 addr16` 即 `LJMP addr16`长跳转指令，`addr16` 处一般是 `STARTUP.A51` 的内容，
如初始化 `idata`, 设置 `sp` 等，然后执行 `LJMP ?C_START` 跳转到用户的 `main` 函数.

此时我们可以接管`CPU`了，可以编写一个专用的`BOOTLOADER` 程序来进行 `UART/USB/CAN` 等通信, 以 `IAP` 更新 `USER_APP`——真正的业务程序，以设置标记、检测某个 `PIN`、检测 `USER_APP` 是否合法等方式来决定是要跳转到 `USER_APP` 还是继续停留在 `BOOTLOADER` 等待上位机更新.

有两处重点：

1. 合理划分 `BOOTLOADER/USER_APP` 的空间
2. 将中断重定向到 `USER_APP` 空间

其中1可以使用 `STC` 的官方下载软件 `AiCube-ISP` 来设置用户 `EEPROM` 大小，

2可以用汇编在原中断地址入口（中断入口地址是芯片厂家决定的，不过一般都是在8051中断地址的扩展和小改）处写一条 `LJMP #USER_ISR_ADDR` 来跳转到 `USER_APP` 的中断函数.

下面我将在 `STC` 官方示例 `STC-Official-user-UART-ISP-bootloader-demo-STC8H8K64U-series` 的基础上逆向其串口协议、修改代码风格、增添一些功能.

## 1. common.h

```c
/**
 * chip: STC8H8K64U
 * ram: 256B idata, 8KB xdata
 * flash: 64KB
 *   - 4KB for bootloader
 *   - 60KB for application
 */

#ifndef __COMMON_H__
#define __COMMON_H__

// #define MAIN_Fosc 11520000UL
// #define MAIN_Fosc 12000000UL
// #define MAIN_Fosc 22118400UL
#define MAIN_Fosc 24000000UL

#define STC_RAM_SIZE 0x2000  // STC8H8K64U has 8KB xdata

#define LDR_SIZE 0x1000     // bootloader flash space = 4KB
#define LDR_VERSION 0x0100  // bootloader version 1.0

#define DFU_TAG 0x12ABCD34UL  // force DFU mode
#define DFU_ADDR (STC_RAM_SIZE - sizeof(DFU_TAG))

#endif /* __COMMON_H__ */
```

由于`8H8K64U`的主频是`AiCube-ISP` 设置的，所以 `BOOTLOADER` 和 `USER_APP` 只能使用相同的主频.

`FLASH` 地址划分也是 `AiCube-ISP` 设置的，所以这里先定义好 `LDR_SIZE` 即 `BOOTLOADER` 占用的 `FLASH` 空间大小，方便 `BOOTLOADER` 更新 `isr` 重映射计算和 `USER_APP` 的 `INTVECTOR/CLASSES` 计算.

由于使用了`KEIL`自带的 `STARTUP.A51`, `XDATALEN EQU 0`，所以芯片软复位时不会初始化 `XDATA`，即可以在 `8KB RAM` 的结尾即`DFU_ADDR`处设置一个 `dfuflag`，一旦`USER_APP` 检测到了某个引脚变化、收到了串口指令等任何要触发更新 `USER_APP`的动作，`USER_APP` 即可设置 `dfuflag = DFU_TAG`并复位，`BOOTLOADER` 检测到 `dfuflag == DFU_TAG` 则停留在 `BOOTLOADER` 等待上位机下载指令而暂时不跳转到 `USER_APP`.

## 2. BOOTLOADER

`BOOTLOADER` 要实现：

1. 重映射 `ISR`
2. 检测是否要跳转到 `USER_APP` 
3. 接收上位机下载指令更新 `USER_APP`

### 2.1 ISR 重映射

为了方便调整 `FLASH` 空间划分，编写了 `update_isr.sh` ：

```bash
#!/bin/bash

set -e

CURRENT_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd ${CURRENT_DIR}

COMMON_H=${CURRENT_DIR}/../common.h
ISR_ASM=${CURRENT_DIR}/src/isr.asm

# parse LDR_SIZE from COMMON_H like `#define LDR_SIZE 0x1000  // bootloader flash space`
LDR_SIZE=$(grep -oP '#define LDR_SIZE \K[0x0-9A-F]+' ${COMMON_H})

echo "LDR_SIZE=${LDR_SIZE}"

# convert the LDR_SIZE from `0xXXXX` to `XXXXH`
LDR_SIZE_HEX=$(printf "%04XH" ${LDR_SIZE})
echo "LDR_SIZE_HEX=${LDR_SIZE_HEX}"

# replace LDR_SIZE in ISR_ASM like `LDR_SIZE EQU 1000H`
sed -i "s/LDR_SIZE EQU .*/LDR_SIZE EQU ${LDR_SIZE_HEX}/" ${ISR_ASM}
```

可以使用 `git-bash` 调用，来从 `common.h` 处定义的 `LDR_SIZE` 计算重映射 `ISR` 的基地址并更新 `isr.asm`:

```ASM
LDR_SIZE EQU 1000H

MAPISR MACRO ADDR
CSEG AT ADDR
LJMP LDR_SIZE + $
ENDM

MAPISR  0003H
MAPISR  000BH
MAPISR  0013H

...

END
```

### 2.2 跳转 USER_APP

应跳转时执行：
```c
((void(code *)())(LDR_SIZE))(); // LJMP #LDR_SIZE, from here the CPU is running application code
```
即可

### 2.3 串口 IAP

协议参考 [protocol.h](./bootloader/src/protocol.h) 和 [protocol.txt](./bootloader/src/protocol.txt)

## 3. USER_APP

重点是在编译时设置 `INTVECTOR` 即中断向量基地址为 `LDR_SIZE`，在链接时设置 `CLASSES`，将生成的代码放在`LDR_SIZE`之后.

来看一下 ![DEMO_APP.HEX](./img/image.png)

生成的第一条指令位于 `0000H`, 机器码为 `02 100EH`，所以我前面说可以类比为 `STM32` 的 `Reset_Handler`，但51并不把`0000H`认为是复位中断入口地址，仅仅是上电第一条指令的取指地址.
`100EH` 为 `STARTUP.A51` 内容.

可以看到`1003H` 为 `INT0` 中断，由于`DEMO_APP`没写该中断，指向了错误的地址 `0800H`，不知道 `C51` 是怎么处理的.
`100BH` 为 `TIMER 0` 中断，指向 `111B` 是正确的.

这种偏移量分布的`HEX`是不能直接交给 `BOOTLOADER` 来烧写的，`BOOTLOADER` 的 `IAP` 操作基地址是`0`，会被映射到真实`FLASH`空间的`LDR_SIZE`处，因此要先将`HEX`转换为`bin`格式，且将第一行的位于 `0000H` 的 `02 10 0E` 代码移动到 `1000H` 处，与后面的组合起来，再整体前移 `1000H`，就得到了可以用来烧写的 `bin` 数据.

## 4. PROGRAMMER

有了理论+协议，就可以编写一个自己的烧录软件了，当然只能用于已经烧录好 `BOOTLOADER` 程序的情况.

点击 `Open` 打开 `DEMO_APP.HEX`, 转换为 `bin` 后是如下分布：

![step1](./img/image2.png)

![step2](./img/image3.png)

再点击 `Patch` 将 `1003H` 处的代码整体前移 `1000H` ，就得到了可以直接烧写的数据：

![step3](./img/image4.png)

连接串口，`Erase All` 再 `Program`, `Reboot`，`USER_APP` 可以正常运行.
