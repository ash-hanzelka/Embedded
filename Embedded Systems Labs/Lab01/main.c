//// 1.3A - code flashes the green and red LEDs simultaneously
//#include <msp430.h>
//#define red BIT0;                     // establishes the red LED at P1.0
//#define green BIT7;                   // establishes green LED at P9.7
//
//int main(void) {
//     volatile unsigned int i;
//     WDTCTL = WDTPW | WDTHOLD;        // stop (watch dog timer -- protect against SW freezes) -- not needed
//     PM5CTL0 &= ~LOCKLPM5;            //disable GPIO power-on default high-impedance mode
//     P1DIR |= red;                    // direct pin as output
//     P9DIR |= green;                  // direct pin as output
//     P1OUT &= ~red;                   // turn LED off
//     P9OUT &= ~green;                 // turn LED off
//     for(;;) {
//        for(i=0; i < 20000; i++) {}   // loop used for delay
//        P1OUT ^= red;                 // toggle LED
//        P9OUT ^= green;               // toggle LED
//     }
//}
//
//// --------------------------------------------------------------------------------------------------------------
//
////// 1.3B - code flashes the green and red LEDs alternating
////#include <msp430.h>
////#define red BIT0;                     // establishes the red LED at P1.0
////#define green BIT7;                   // establishes green LED at P9.7
////
////int main(void) {
////    volatile unsigned int i;
////    WDTCTL = WDTPW | WDTHOLD;         // stop (watch dog timer -- protect against SW freezes) -- not needed
////    PM5CTL0 &= ~LOCKLPM5;             //disable GPIO power-on default high-impedance mode
////    P1DIR |= red;                     // direct pin as output
////    P9DIR |= green;                   // direct pin as output
////    P1OUT &= ~red;                    // turn LED off
////    P9OUT |= green;                   // turn LED on
////    for(;;) {
////        for(i=0; i < 20000; i++) {}   // loop used for delay; restricted to 16 bits so for ex. 80,000 wouldn't work
////        P1OUT ^= red;                 // toggles LED
////        P9OUT ^= green;               // toggles LED
////    }
////}

// --------------------------------------------------------------------------------------------------------------

//// 1.4A1 - code flashes the green and red LEDs simultaneously
//#include <msp430.h>
//#include <stdint.h>
//#define red BIT0;                 // establishes the red LED at P1.0
//#define green BIT7;               // establishes green LED at P9.7
//
//int main(void) {
//    volatile uint32_t i;
//    WDTCTL = WDTPW | WDTHOLD;         // stop (watch dog timer -- protect against SW freezes) -- not needed
//    PM5CTL0 &= ~LOCKLPM5;             //disable GPIO power-on default high-impedance mode
//    P1DIR |= red;                     // direct pin as output
//    P9DIR |= green;                   // direct pin as output
//    P1OUT &= ~red;                    // turn LED off
//    P9OUT &= ~green;                  // turn LED off
//    for(;;) {
//        for(i=0; i < 80000; i++) {}   // loop used for delay
//        P1OUT ^= red;                 // toggles LED
//        P9OUT ^= green;               // toggles LED
//
//
//    }
//}

// --------------------------------------------------------------------------------------------------------------

//// 1.4B1 - code flashes the green and red LEDs simultaneously
//#include <msp430.h>
//#include <stdint.h>
//#define red BIT0;                 // establishes the red LED at P1.0
//#define green BIT7;               // establishes green LED at P9.7
//
//int main(void) {
//    volatile unsigned int i;
//    WDTCTL = WDTPW | WDTHOLD;     // stop (watch dog timer -- protect against SW freezes) -- not needed
//    PM5CTL0 &= ~LOCKLPM5;         //disable GPIO power-on default high-impedance mode
//    P1DIR |= red;                 // direct pin as output
//    P9DIR |= green;               // direct pin as output
//    P1OUT &= ~red;                // turn LED off
//    P9OUT &= ~green;              // turn LED off
//    for(;;) {
//        _delay_cycles(80000);     // delay 80,000 cycles
//        P1OUT ^= red;             // toggles LED
//        P9OUT ^= green;           // toggles LED
//
//
//    }
//}

// --------------------------------------------------------------------------------------------------------------

//// 1.4B2 - code flashes the green and red LEDs alternating
//#include <msp430.h>
//#include <stdint.h>
//#define red BIT0;                   // establishes the red LED at P1.0
//#define green BIT7;                 // establishes green LED at P9.7
//
//int main(void) {
//    volatile unsigned int i;
//    WDTCTL = WDTPW | WDTHOLD;       // stop (watch dog timer -- protect against SW freezes) -- not needed
//    PM5CTL0 &= ~LOCKLPM5;           //disable GPIO power-on default high-impedance mode
//    P1DIR |= red;                   // direct pin as output
//    P9DIR |= green;                 // direct pin as output
//    P1OUT &= ~red;                  // turn LED off
//    P9OUT |= green;                 // turn LED on
//    for(;;) {
//        _delay_cycles(400000);      // very slow delay so the alternation is visible
//        P1OUT ^= red;               // toggles LED
//        P9OUT ^= green;             // toggles LED
//
//
//    }
//}

// --------------------------------------------------------------------------------------------------------------

// 1.5 - Firetruck
#include <msp430.h>
#include <stdint.h>
#define red BIT0;                       // establishes the red LED at P1.0
#define green BIT7;                     // establishes green LED at P9.7

int main(void) {
    volatile unsigned int delayCount;
    volatile unsigned int betweenDelay;
    WDTCTL = WDTPW | WDTHOLD;           // stop (watch dog timer -- protect against SW freezes) -- not needed
    PM5CTL0 &= ~LOCKLPM5;               //disable GPIO power-on default high-impedance mode
    P1DIR |= red;                       // direct pin as output
    P9DIR |= green;                     // direct pin as output
    P1OUT &= ~red;                      // turn LED off
    P9OUT &= ~green;                    // turn LED off
    for(;;) {
        for(delayCount = 150; delayCount > 0; delayCount--) {
            if (delayCount % 2 != 0) {
                P1OUT &= ~red;          // turn LED off
                P9OUT &= ~green;        // turn LED off
            }
            else {
                if ((delayCount % 4) == 0) {
                    P1OUT ^= red;       // toggle red led
                }
                else {
                    P9OUT ^= green;     // toggle green led
                }

                for (betweenDelay = 0; betweenDelay < (delayCount*50); betweenDelay++) {} // delay in between flashing so you can see it
            }
        }

        for (delayCount = 10; delayCount > 0; delayCount--) {
            if(delayCount % 2 == 1) {
                P1OUT &= ~red;
                P9OUT &= ~green;
            }
            else {
                P1OUT |= red;
                P9OUT |= green;
            }
            _delay_cycles(80000);
        }

    }
}

// --------------------------------------------------------------------------------------------------------------

//// 1.5 - Multiple Blinks Alternating
//#include <msp430.h>
//#include <stdint.h>
//#define red BIT0;                       // establishes the red LED at P1.0
//#define green BIT7;                     // establishes green LED at P9.7
//
//int main(void) {
//    volatile unsigned int i;
//    volatile unsigned int j;
//    WDTCTL = WDTPW | WDTHOLD;           // stop (watch dog timer -- protect against SW freezes) -- not needed
//    PM5CTL0 &= ~LOCKLPM5;               //disable GPIO power-on default high-impedance mode
//    P1DIR |= red;                       // direct pin as output
//    P9DIR |= green;                     // direct pin as output
//    P1OUT &= ~red;                      // turn LED off
//    P9OUT &= ~green;                    // turn LED off
//    for(;;) {
//        for (i = 10; i > 0; i--) {
//            P9OUT &= ~green;
//            for(j = 30; j > 0; j--) {
//                P1OUT ^= red;           // invert red
//                _delay_cycles(30000);
//            }
//            P1OUT &= ~ red;
//            for(j = 0; j < 30; j++) {   // for loop can be done either incrementing or decrementing (line 188& this line)
//                P9OUT ^= green;
//                _delay_cycles(30000);
//            }
//            for(j = 15; j > 0; j--) {
//                P1OUT ^= red;           // invert red
//                _delay_cycles(30000);
//            }
//            P1OUT &= ~ red;
//            for(j = 0; j < 15; j++) {   // for loop can be done either incrementing or decrementing (line 188& this line)
//                P9OUT ^= green;
//                _delay_cycles(30000);
//            }
//        }
//    }
//}






