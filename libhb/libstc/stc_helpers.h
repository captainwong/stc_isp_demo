/**
 * @file stc_helpers.h
 * @brief Helper functions and macros for STC microcontrollers.
 * @author captainwong (1281261856#qq.com)
 * @date 2025-07-24
 */

#ifndef __STC_HELPERS_H__
#define __STC_HELPERS_H__

#include "stc_helper_gpio.h"

////////////////////////////// 辅助宏函数 //////////////////////////////

//////////////////// bit ////////////////////

#define BIT7 0x80
#define BIT6 0x40
#define BIT5 0x20
#define BIT4 0x10
#define BIT3 0x08
#define BIT2 0x04
#define BIT1 0x02
#define BIT0 0x01

// bitn in BIT0, BIT1, ... , BIT7
#define set_bit(x, bitn) ((x) |= (bitn))
// bitn in BIT0, BIT1, ... , BIT7
#define clr_bit(x, bitn) ((x) &= ~(bitn))
// bitn in BIT0, BIT1, ... , BIT7
#define tst_bit(x, bitn) ((x) & (bitn))

//////////////////// nop ////////////////////

#ifdef __C51__
#define nop1() _nop_()
#elif defined(__SDCC)
#define nop1() __asm__("nop")
#endif  // __C51__

#define nop2() nop1(), nop1()
#define nop3() nop2(), nop1()
#define nop4() nop3(), nop1()
#define nop5() nop4(), nop1()
#define nop6() nop5(), nop1()
#define nop7() nop6(), nop1()
#define nop8() nop7(), nop1()
#define nop9() nop8(), nop1()
#define nop10() nop9(), nop1()
#define nop11() nop10(), nop1()
#define nop12() nop11(), nop1()
#define nop13() nop12(), nop1()
#define nop14() nop13(), nop1()
#define nop15() nop14(), nop1()
#define nop16() nop15(), nop1()
#define nop17() nop16(), nop1()
#define nop18() nop17(), nop1()
#define nop19() nop18(), nop1()
#define nop20() nop19(), nop1()
#define nop21() nop20(), nop1()
#define nop22() nop21(), nop1()
#define nop23() nop22(), nop1()
#define nop24() nop23(), nop1()
#define nop25() nop24(), nop1()
#define nop26() nop25(), nop1()
#define nop27() nop26(), nop1()
#define nop28() nop27(), nop1()
#define nop29() nop28(), nop1()
#define nop30() nop29(), nop1()
#define nop31() nop30(), nop1()
#define nop32() nop31(), nop1()
#define nop33() nop32(), nop1()
#define nop34() nop33(), nop1()
#define nop35() nop34(), nop1()
#define nop36() nop35(), nop1()
#define nop37() nop36(), nop1()
#define nop38() nop37(), nop1()
#define nop39() nop38(), nop1()
#define nop40() nop39(), nop1()
#define nop(n) nop##n()

#endif /* __STC_HELPERS_H__ */
