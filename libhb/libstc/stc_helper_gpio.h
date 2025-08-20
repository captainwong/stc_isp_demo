/**
 * @file stc_helper_gpio.h
 * @brief GPIO helper functions for STC microcontrollers.
 * @author captainwong (1281261856#qq.com)
 * @date 2025-07-24
 */

#ifndef __STC_HELPER_GPIO_H__
#define __STC_HELPER_GPIO_H__

////////////////////////////// GPIO 工作模式 //////////////////////////////

/*
| PnM1.x | PnM0.x | Pn.x工作模式 |
|--------|--------|--------------|
| 0      | 0      |  准双向口    |
| 0      | 1      |  推挽输出    |
| 1      | 0      |  高阻输入    |
| 1      | 1      |  开漏输出    |
*/
#define port_m1(port) P##port##M1
#define port_m0(port) P##port##M0

// 准双向口 quasi bidirectional / weak pull-up mode (standard 8051 output mode)
#define pin_mode_io_pup(port, pin)    \
    do {                              \
        port_m1(port) &= ~(1 << pin); \
        port_m0(port) &= ~(1 << pin); \
    } while (0)

// 推挽输出 push-pull output / strong pull-up mode
#define pin_mode_out_pp(port, pin)    \
    do {                              \
        port_m1(port) &= ~(1 << pin); \
        port_m0(port) |= (1 << pin);  \
    } while (0)

// 高阻输入 high-impedance input (where current can neither flow in nor out)
#define pin_mode_in_hiz(port, pin)    \
    do {                              \
        port_m1(port) |= (1 << pin);  \
        port_m0(port) &= ~(1 << pin); \
    } while (0)

// 开漏输出 open-drain mode
#define pin_mode_out_od(port, pin)   \
    do {                             \
        port_m1(port) |= (1 << pin); \
        port_m0(port) |= (1 << pin); \
    } while (0)

#define _pin_mode(mode) pin_mode_##mode
#define pin_mode(port, pin, mode) _pin_mode(mode)(port, pin)  // 设置GPIO工作模式

////////////////////////////// 内部4.1K上拉设置 PnPU //////////////////////////////

#define port_pu(port) P##port##PU
#define pin_pu(port, pin) port_pu(port) |= (1 << pin)      // 设置内部4.1K上拉
#define pin_no_pu(port, pin) port_pu(port) &= ~(1 << pin)  // 取消内部4.1K上拉

////////////////////////////// 施密特触发设置 PnNCS //////////////////////////////

#define port_st(port) P##port##NCS
#define pin_st(port, pin) port_st(port) |= (1 << pin)      // 设置施密特触发
#define pin_no_st(port, pin) port_st(port) &= ~(1 << pin)  // 取消施密特触发

////////////////////////////// GPIO 电平转换速度 PnSR //////////////////////////////

#define port_sr(port) P##port##SR
#define pin_speed_low(port, pin) port_sr(port) |= (1 << pin)    // 设置电平转换慢速，相应的上下冲比较小
#define pin_speed_high(port, pin) port_sr(port) &= ~(1 << pin)  // 取消电平转换快速，相应的上下冲比较大

////////////////////////////// GPIO 驱动电流控制 PnDR //////////////////////////////

#define port_dr(port) P##port##DR
#define pin_dr_low(port, pin) port_dr(port) |= (1 << pin)    // 一般驱动能力
#define pin_dr_high(port, pin) port_dr(port) &= ~(1 << pin)  // 增强驱动能力

////////////////////////////// GPIO 数字信号输入使能 PnIE //////////////////////////////

#define port_ie(port) P##port##IE
#define pin_ie_enable(port, pin) port_ie(port) |= (1 << pin)    // 使能数字信号输入
#define pin_ie_disable(port, pin) port_ie(port) &= ~(1 << pin)  // 禁用数字信号输入

#endif /* __STC_HELPER_GPIO_H__ */
