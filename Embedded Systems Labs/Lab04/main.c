//// 4.1: Timer_A continuous mode, with interrupts, flashes LEDs
//#include <msp430fr6989.h>
//#define red BIT0                        // red led to 1.0 (port 1, bit 0)
//#define green BIT7                      // green led to 9.7
//
//
//void config_ACLK_to_32KHz_crystal() {
//    // by default, ACLK runs on LFMDCLK at 5MHz / 128 = 39 KHz
//    // this function configures ACLK to 32 KHz crystal
//
//    //reroute pins to LFXIN/LFXOUT
//    PJSEL1 &= ~BIT4;
//    PJSEL0 |= BIT4;
//
//    // wait until oscillator fault flags remain cleared
//    CSCTL0 = CSKEY;                         // unlock cs registers
//    do {
//        CSCTL5 &= ~LFXTOFFG;                // local fault flag
//        SFRIFG1 &= ~OFIFG;                   // global fault flag
//        } while ((CSCTL5 & LFXTOFFG) != 0);
//
//    CSCTL0_H = 0;                           // lock CS registers
//    return;
// }
//
//
//void main(void) {
//    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;               // enable GPIO pins
//
//    // configure/initialize LEDs
//    P1DIR |= red;                       // direct pin1 as output for red (1.0)
//    P9DIR |= green;                     // direct pin9 as output for green (9.7)
//
//    P1OUT &= ~red;
//    P9OUT &= ~green;
//
//    config_ACLK_to_32KHz_crystal();
//
//    // configure Timer A
//
//    TA0CTL = TASSEL_1 |     // sets to ACLK
//             ID_0 |         // divides by 1
//             MC_2 |         // sets to continuous mode
//             TACLR|         // Timer_A clear, sets TAR to 0
//             TAIE;
//
//    TA0CTL &= ~TAIFG;       // ensures flag is clear at start
//
//    _enable_interrupts();
//
//  for (;;) {
//      }
//
//      // P1OUT ^= red;
//      // TA0CTL &= ~TAIFG;  // clears flag
//
//}
//
//#pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
//__interrupt void T0A1_ISR() {
//    // Interrupt Response
//    if (TA0CTL & TAIFG) {       // check interrupt
//        P1OUT ^= red;           // toggle the red LED
//        TA0CTL &= ~TAIFG;       // clear interrupt flag
//    }
//}

// ---------------------------------------------------------------------------------------------------------------

// 4.2: Timer's up mode with interrupt
#include <msp430fr6989.h>
#define red BIT0                        // red led to 1.0 (port 1, bit 0)
#define green BIT7                      // green led to 9.7


void config_ACLK_to_32KHz_crystal() {
    // by default, ACLK runs on LFMDCLK at 5MHz / 128 = 39 KHz
    // this function configures ACLK to 32 KHz crystal

    //reroute pins to LFXIN/LFXOUT
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    // wait until oscillator fault flags remain cleared
    CSCTL0 = CSKEY;                         // unlock cs registers
    do {
        CSCTL5 &= ~LFXTOFFG;                // local fault flag
        SFRIFG1 &= ~OFIFG;                   // global fault flag
        } while ((CSCTL5 & LFXTOFFG) != 0);

    CSCTL0_H = 0;                           // lock CS registers
    return;
 }


void main(void) {
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               // enable GPIO pins

    // configure/initialize LEDs
    P1DIR |= red;                       // direct pin1 as output for red (1.0)
    P9DIR |= green;                     // direct pin9 as output for green (9.7)

    P1OUT &= ~red;                      // given, red LED off
    P9OUT |= ~green;                    // given, green LED on

    config_ACLK_to_32KHz_crystal();

    // configure channel 0 for up mode w interrupts
     TA0CCR0 = 32767;                    // 1 second @32 KHz - 1 second would be half of the 2 seconds made by 65535 cycles
     //TA0CCR0 = 16363;                    // 0.5 sec
     // TA0CCR0 = 3276;
    TA0CCTL0 = CCIE;                    // enable channel 0 ccie bit
     TA0CCTL0 &= ~CCIFG;                 // clear channel 0 ccifg bit

    // configure Timer A

    TA0CTL = TASSEL_1 |     // sets to ACLK
             ID_0 |         // divides by 1
             MC_1 |         // sets to up mode
             TACLR;         // Timer_A clear, sets TAR to 0

    TA0CTL &= ~TAIFG;       // ensures flag is clear at start

    _enable_interrupts();

  for (;;) {

      }

      // P1OUT ^= red;
      // TA0CTL &= ~TAIFG;  // clears flag

}

#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
    // Interrupt Response       // check interrupt
        P1OUT ^= red;           // toggle the red LED
        P9OUT ^= green;           // toggle the red LED

}

// ---------------------------------------------------------------------------------------------------------------

//// 4.3: Timer's up mode with interrupt and buttons
//#include <msp430fr6989.h>
//#define red BIT0                        // red led to 1.0 (port 1, bit 0)
//#define green BIT7                      // green led to 9.7
//#define b1 BIT1
//#define b2 BIT2
//
//
//void config_ACLK_to_32KHz_crystal() {
//    // by default, ACLK runs on LFMDCLK at 5MHz / 128 = 39 KHz
//    // this function configures ACLK to 32 KHz crystal
//
//    //reroute pins to LFXIN/LFXOUT
//    PJSEL1 &= ~BIT4;
//    PJSEL0 |= BIT4;
//
//    // wait until oscillator fault flags remain cleared
//    CSCTL0 = CSKEY;                         // unlock cs registers
//    do {
//        CSCTL5 &= ~LFXTOFFG;                // local fault flag
//        SFRIFG1 &= ~OFIFG;                   // global fault flag
//        } while ((CSCTL5 & LFXTOFFG) != 0);
//
//    CSCTL0_H = 0;                           // lock CS registers
//    return;
// }
//
//
//void main(void) {
//    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;               // enable GPIO pins
//
//    // configure/initialize LEDs
//    P1DIR |= red;                       // direct pin1 as output for red (1.0)
//    P9DIR |= green;                     // direct pin9 as output for green (9.7)
//
//    P1OUT &= ~red;                      // given, red LED off
//    P9OUT |= green;                    // given, green LED on
//
//    P1DIR &= ~(b1 | b2);
//    P1REN |= (b1 | b2);
//    P1OUT |= (b1 | b2);
//    P1IES &= ~(b1 | b2);                // clear - 0 for rising edge
//    P1IFG &= ~(b1 | b2);
//    P1IE |= (b1 | b2);
//
//    config_ACLK_to_32KHz_crystal();
//
//    // configure channel 0 for up mode w interrupts
//    // TA0CCR0 = 32767;                    // 1 second @32 KHz - 1 second would be half of the 2 seconds made by 65535 cycles
//    TA0CCR0 = 16363;                    // 0.5 sec
//    TA0CCTL0 = CCIE;                    // enable channel 0 ccie bit
//    // TA0CCTL0 &= ~CCIFG;                 // clear channel 0 ccifg bit
//
//    // configure Timer A
//
//    TA0CTL = TASSEL_1 |     // sets to ACLK
//             ID_0 |         // divides by 1
//             MC_1 |         // sets to up mode
//             TACLR;         // Timer_A clear, sets TAR to 0
//
//    TA0CTL &= ~TAIFG;       // ensures flag is clear at start
//
//    _enable_interrupts();
//
//  for (;;) {
//
//      }
//
//      // P1OUT ^= red;
//      // TA0CTL &= ~TAIFG;  // clears flag
//
//}
//
//#pragma vector = PORT1_VECTOR // Link the ISR to the vector
//__interrupt void Port1_ISR() {
//
//    if((P1IFG & b1) == b1 ){
//        P1OUT ^= red;
//        P1IFG &= ~b1;
//    }
//
//    if((P1IFG & b2) == b2 ) {
//        P9OUT ^= green;
//        P1IFG &= ~b2;
//    }
//
//
////    // Interrupt Response       // check interrupt
////        P1OUT ^= red;           // toggle the red LED
////        P9OUT ^= green;           // toggle the red LED
//
//}
