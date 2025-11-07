//// Lab 11
//
////// 11.1 -----------------------------------------------------------------------------------------------------------------
////
////#include <msp430fr6989.h>
////#define red BIT0                        // red led to 1.0 (port 1, bit 0) -- compressor indicator
////#define green BIT7                      // green led to 9.7
////#define b1 BIT1                         // the button
////#define b2 BIT2
////
////int x;
////
////void config_ACLK_to_32KHz_crystal() {
////    // by default, ACLK runs on LFMDCLK at 5MHz / 128 = 39 KHz
////    // this function configures ACLK to 32 KHz crystal
////
////    //reroute pins to LFXIN/LFXOUT
////    PJSEL1 &= ~BIT4;
////    PJSEL0 |= BIT4;
////
////    // wait until oscillator fault flags remain cleared
////    CSCTL0 = CSKEY;                         // unlock cs registers
////    do {
////        CSCTL5 &= ~LFXTOFFG;                // local fault flag
////        SFRIFG1 &= ~OFIFG;                   // global fault flag
////        } while ((CSCTL5 & LFXTOFFG) != 0);
////
////    CSCTL0_H = 0;                           // lock CS registers
////    return;
//// }
////
////void main(void) {
////
////    WDTCTL = WDTPW | WDTHOLD;             // stop watchdog timer
////    PM5CTL0 &= ~LOCKLPM5;                 // enable GPIO pins
////    config_ACLK_to_32KHz_crystal();
////
////
////    // configure LEDs
////    P1DIR |= red;                         // direct pin1 as output for red (1.0)
////    P1OUT &= ~red;
////    P9DIR |= green;
////    P9OUT &= ~green;
////
////
////    // configure button
////    P1DIR &= ~b1;
////    P1REN |= b1;
////    P1OUT |= b1;
////    P1IES |= b1;                // clear - 0 for rising edge
////    P1IFG &= ~b1;
////    P1IE |= b1;
////
////    // configure timer A1 on channel 1 - use timer A1 because that is the vector channel 1 belongs to
////    // TA1CCR1 = 16383;             // half second (65535 = 2 sec, so /4 to get the cycles of 0.5 sec)
////    // divider is set to /4, so this number minimizes to:
////    TA1CCR1 = 4095;
////    TA1CCTL1 |= CCIE;            // enable channel 1 ccie bit
////    TA1CCTL1 &= ~CCIFG;          // clear channel 1 ccifg bit
////
////
////    // configure timer A1 on channel 2 - use timer A1 because that is the vector channel 2 belongs to
////    // 32K = 32 * 1024 = 32768      --> ID = 4, so:   32768/4  = 8192
////    // 3 seconds: 8192hz * 3sec = 24576
////    TA1CCR2 = 24576;             // half second (65535 = 2 sec, so /4 to get the cycles of 0.5 sec)
////    TA1CCTL2 |= CCIE;            // enable channel 2 ccie bit
////    TA1CCTL2 &= ~CCIFG;          // clear channel 2 ccifg bit
////
////    // timer A in continuous mode
////    TA1CTL = TASSEL_1 |     // sets to ACLK
////            ID_2 |         // divides by 4
////            MC_2 |         // sets to continuous mode
////            TACLR|         // Timer_A clear, sets TAR to 0
////            TAIE;
////
////    _enable_interrupts();
////    x = 0;
////    // engage an LPM
////    // LPM leaves ACLK on and disables all the other clocks
////    // most efficient LPM in this situation
////    _low_power_mode_3();
////
////
////    while(1) {
////
////    }
////
////}
////
////
////
////// channel 1 ISR
////    // 0.5 sec interval to flash green LED
////// channel 2 ISR
////    // 3 second interval timer
////    // xIE set to 0 when not in use
////#pragma vector = TIMER1_A1_VECTOR
////__interrupt void T1A1_ISR() {
////    // if the channel flag 1 is set
////    if((TA1CCTL1 & CCIFG) != 0) {
////        P9OUT ^= green;
////        TA1CCR1 += 4095;        // Set next for 0.5 second later
////        TA1CCTL1 &= ~CCIFG;     // Clear Flag
////    }
////    // if the channel 2 flag is set and the channel 2 interrupt is engaged (channel 2 ISR conditions)
////    if(((TA1CCTL2 & CCIFG) != 0) && ((TA1CCTL2 & CCIE) != 0)) {
////        if(x==0) {
////            P1OUT &= ~red;
////        }
////        else {
////            P1OUT ^= red;
////        }
////
////        TA1CCR2 += 24576;       // Set CCR2 for 3 seconds later
////        TA1CCTL2 &= ~CCIE;      // disable Channel 2 interrupt
////        P1IE |= b1;             // enable the button interrupt again
////        TA1CCTL2 &= ~CCIFG;     // Clear Flag
////    }
////}
////
////// button press ISR with debouncing
////#pragma vector = PORT1_VECTOR
////__interrupt void Port_ISR() {
////    if((P1IN & b1) == 0){       // check if button is pressed
////        x++;
////        // TA1CCTL2 &= ~CCIFG;     // Clear Flag
////
////        TA1CCR1 = 4095;         // 0.5 Seconds
////        TA1CCTL1 |= CCIE;       // enable interrupt bit
////        TA1CCTL1 &= ~CCIFG;     // clear flag
////
////        TA1CCR2 = 24576;        // 3 Seconds
////        TA1CCTL2 &= ~CCIFG;     // clear flag
////        TA1CCTL2 |= CCIE;       // interrupt enabled
////
////        // timer A in continuous mode
////        TA1CTL = TASSEL_1 |     // ACLK
////             ID_2 |             // divides by 4
////             MC_2 |             // cont mode
////             TACLR|             // Timer_A clear, sets TAR to 0
////             TAIE;              // set timer A interrupt
////
////        P1IE &= ~b1;            // interrupt disable
////        P1IFG &= ~b1;           // flag clear
////     }
////}
////
//
//// 11.2 -----------------------------------------------------------------------------------------------------------------
//
//#include <msp430fr6989.h>
//#define red BIT0                        // red led to 1.0 (port 1, bit 0) -- compressor indicator
//#define green BIT7                      // green led to 9.7
//#define b1 BIT1                         // the button
//#define b2 BIT2
//
//int x;
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
//void main(void) {
//
//    WDTCTL = WDTPW | WDTHOLD;             // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;                 // enable GPIO pins
//    config_ACLK_to_32KHz_crystal();
//
//
//    // configure LEDs
//    P1DIR |= red;                         // direct pin1 as output for red (1.0)
//    P1OUT &= ~red;
//    P9DIR |= green;
//    P9OUT &= ~green;
//
//
//    // configure button
//    P1DIR &= ~b1;
//    P1REN |= b1;
//    P1OUT |= b1;
//    P1IES |= b1;                // clear - 0 for rising edge
//    P1IFG &= ~b1;
//    P1IE |= b1;
//
//    // configure timer A1 on channel 1 - use timer A1 because that is the vector channel 1 belongs to
//    // TA1CCR1 = 16383;             // half second (65535 = 2 sec, so /4 to get the cycles of 0.5 sec)
//    // divider is set to /4, so this number minimizes to:
//    TA1CCR1 = 4095;
//    TA1CCTL1 |= CCIE;            // enable channel 1 ccie bit
//    TA1CCTL1 &= ~CCIFG;          // clear channel 1 ccifg bit
//
//
//    // configure timer A1 on channel 2 - use timer A1 because that is the vector channel 2 belongs to
//    // 32K = 32 * 1024 = 32768      --> ID = 4, so:   32768/4  = 8192
//    // 3 seconds: 8192hz * 3sec = 24576
//    TA1CCR2 = 24576;             // half second (65535 = 2 sec, so /4 to get the cycles of 0.5 sec)
//    TA1CCTL2 |= CCIE;            // enable channel 2 ccie bit
//    TA1CCTL2 &= ~CCIFG;          // clear channel 2 ccifg bit
//
//    // timer A in continuous mode
//    TA1CTL = TASSEL_1 |     // sets to ACLK
//            ID_2 |         // divides by 4
//            MC_2 |         // sets to continuous mode
//            TACLR|         // Timer_A clear, sets TAR to 0
//            TAIE;
//
//    _enable_interrupts();
//    x = 0;
//    // engage an LPM
//    // LPM leaves ACLK on and disables all the other clocks
//    // most efficient LPM in this situation
//    _low_power_mode_3();
//
//
//    while(1) {
//
//    }
//
//}
//
//
//
//// channel 1 ISR
//    // 0.5 sec interval to flash green LED
//// channel 2 ISR
//    // 3 second interval timer
//    // xIE set to 0 when not in use
//#pragma vector = TIMER1_A1_VECTOR
//__interrupt void T1A1_ISR() {
//    // if the channel flag 1 is set
//    if((TA1CCTL1 & CCIFG) != 0) {
//        P9OUT ^= green;
//        TA1CCR1 += 4095;        // Set next for 0.5 second later
//        TA1CCTL1 &= ~CCIFG;     // Clear Flag
//    }
//    // if the channel 2 flag is set and the channel 2 interrupt is engaged (channel 2 ISR conditions)
//    if(((TA1CCTL2 & CCIFG) != 0) && ((TA1CCTL2 & CCIE) != 0)) {
//        if(x==0) {
//            P1OUT &= ~red;
//        }
//        else {
//            P1OUT ^= red;
//        }
//
//        TA1CCR2 += 24576;       // Set CCR2 for 3 seconds later
//        TA1CCTL2 &= ~CCIE;      // disable Channel 2 interrupt
//        P1IE |= b1;             // enable the button interrupt again
//        TA1CCTL2 &= ~CCIFG;     // Clear Flag
//    }
//}
//
//// button press ISR with debouncing
//#pragma vector = PORT1_VECTOR
//__interrupt void Port_ISR() {
//    if((P1IN & b1) == 0){       // check if button is pressed
//        x++;
//        // TA1CCTL2 &= ~CCIFG;     // Clear Flag
//
//        TA1CCR1 = 4095;         // 0.5 Seconds
//        TA1CCTL1 |= CCIE;       // enable interrupt bit
//        TA1CCTL1 &= ~CCIFG;     // clear flag
//
//        TA1CCR2 = 24576;        // 3 Seconds
//        TA1CCTL2 &= ~CCIFG;     // clear flag
//        TA1CCTL2 |= CCIE;       // interrupt enabled
//
//        // timer A in continuous mode
//        TA1CTL = TASSEL_1 |     // ACLK
//             ID_2 |             // divides by 4
//             MC_2 |             // cont mode
//             TACLR|             // Timer_A clear, sets TAR to 0
//             TAIE;              // set timer A interrupt
//
//        // P1IE &= ~b1;            // interrupt disable
//        P1IFG &= ~b1;           // flag clear
//     }
//}
//
////// 11.3 -----------------------------------------------------------------------------------------------------------------
////
////#include <msp430fr6989.h>
////#define red BIT0                        // red led to 1.0 (port 1, bit 0) -- compressor indicator
////#define green BIT7                      // green led to 9.7
////#define b1 BIT1                         // the button
////#define b2 BIT2
////
////void main(void) {
////
////    WDTCTL = WDTPW | WDTHOLD;             // stop watchdog timer
////    PM5CTL0 &= ~LOCKLPM5;                 // enable GPIO pins
////    // config_ACLK_to_32KHz_crystal();
////
////
////    // configure LEDs
////    P1DIR |= red;                         // direct pin1 as output for red (1.0)
////    P1OUT &= ~red;
////
////
////    // configure button
////    P1DIR &= ~b1;
////    P1REN |= b1;
////    P1OUT |= b1;
////    P1IES |= b1;                // clear - 0 for rising edge
////    P1IFG &= ~b1;
////    P1IE |= b1;
////
////
////
////    _enable_interrupts();
////
////    _low_power_mode_3();
////
////
////    while(1) {
////
////
////    }
////
////}
////
////// button press ISR with debouncing
////#pragma vector = PORT1_VECTOR
////__interrupt void Port_ISR() {
////    P1IE &= ~b1;
////    TA0CCR0 = 655;              // 20ms max debounce
////    TA0CCTL0 |= CCIE;
////    TA0CTL = TASSEL_1 |         // ACLK
////         ID_0 |                 // divides by 1
////         MC_1 |                 // up mode --> no divisor is big enough to use cont. mode for 20 ms
////         TACLR;                 // Timer_A clear, sets TAR to 0
////    TA0CCTL0 &= ~CCIFG;
////}
////
////
////// red led toggle
////// button press ISR with debouncing
////#pragma vector = TIMER0_A0_VECTOR
////__interrupt void T0A0_ISR(){
////    TA0CCTL0 &= ~CCIE;
////    P1IE |= b1;
////    if ((P1IN & b1) == 0){
////        P1OUT ^= red;        //Toggle redLED when button is pressed
////    }
////    TA0CCTL0 &= ~CCIFG;
////    P1IFG &= ~b1;
////}






























// 11.2 -----------------------------------------------------------------------------------------------------------
#include <msp430fr6989.h>
#define red BIT0                        // red led to 1.0 (port 1, bit 0) -- compressor indicator
#define green BIT7                      // green led to 9.7
#define b1 BIT1                         // the button
#define b2 BIT2
int x;
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
    WDTCTL = WDTPW | WDTHOLD;             // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                 // enable GPIO pins
    config_ACLK_to_32KHz_crystal();
    // configure LEDs
    P1DIR |= red;                         // direct pin1 as output for red (1.0)
    P1OUT &= ~red;
    P9DIR |= green;
    P9OUT &= ~green;
    // configure button
    P1DIR &= ~b1;
    P1REN |= b1;
    P1OUT |= b1;
    P1IES |= b1;                // clear - 0 for rising edge
    P1IFG &= ~b1;
    P1IE |= b1;
    // configure timer A1 on channel 1 - use timer A1 because that is the vector channel 1 belongs to
    // TA1CCR1 = 16383;             // half second (65535 = 2 sec, so /4 to get the cycles of 0.5 sec)
    // divider is set to /4, so this number minimizes to:
    TA1CCR1 = 4095;
    TA1CCTL1 |= CCIE;            // enable channel 1 ccie bit
    TA1CCTL1 &= ~CCIFG;          // clear channel 1 ccifg bit
    // configure timer A1 on channel 2 - use timer A1 because that is the vector channel 2 belongs to
    // 32K = 32 * 1024 = 32768      --> ID = 4, so:   32768/4  = 8192
    // 3 seconds: 8192hz * 3sec = 24576
    TA1CCR2 = 24576;             // half second (65535 = 2 sec, so /4 to get the cycles of 0.5 sec)
    TA1CCTL2 |= CCIE;            // enable channel 2 ccie bit
    TA1CCTL2 &= ~CCIFG;          // clear channel 2 ccifg bit
    // timer A in continuous mode
    TA1CTL = TASSEL_1 |     // sets to ACLK
            ID_2 |         // divides by 4
            MC_2 |         // sets to continuous mode
            TACLR|         // Timer_A clear, sets TAR to 0
            TAIE;
    _enable_interrupts();
    x = 0;
    // engage an LPM
    // LPM leaves ACLK on and disables all the other clocks
    // most efficient LPM in this situation
    _low_power_mode_3();
    while(1) {
    }
}
// channel 1 ISR
    // 0.5 sec interval to flash green LED
// channel 2 ISR
    // 3 second interval timer
    // xIE set to 0 when not in use
#pragma vector = TIMER1_A1_VECTOR
__interrupt void T1A1_ISR() {
    // if the channel flag 1 is set
    if((TA1CCTL1 & CCIFG) != 0) {
        P9OUT ^= green;
        TA1CCR1 += 4095;        // Set next for 0.5 second later
        TA1CCTL1 &= ~CCIFG;     // Clear Flag
    }
    // if the channel 2 flag is set and the channel 2 interrupt is engaged (channel 2 ISR conditions)
    if(((TA1CCTL2 & CCIFG) != 0) && ((TA1CCTL2 & CCIE) != 0)) {
        if(x==0) {
            P1OUT &= ~red;
        }
        else {
            P1OUT ^= red;
        }
        TA1CCR2 += 24576;       // Set CCR2 for 3 seconds later
        TA1CCTL2 &= ~CCIE;      // disable Channel 2 interrupt
        P1IE |= b1;             // enable the button interrupt again
        TA1CCTL2 &= ~CCIFG;     // Clear Flag
    }
}
// button press ISR with debouncing
#pragma vector = PORT1_VECTOR
__interrupt void Port_ISR() {
    if((P1IN & b1) == 0){       // check if button is pressed
        x++;
        // TA1CCTL2 &= ~CCIFG;     // Clear Flag
        TA1CCR1 = 4095;         // 0.5 Seconds
        TA1CCTL1 |= CCIE;       // enable interrupt bit
        TA1CCTL1 &= ~CCIFG;     // clear flag
        TA1CCR2 = 24576;        // 3 Seconds
        TA1CCTL2 &= ~CCIFG;     // clear flag
        TA1CCTL2 |= CCIE;       // interrupt enabled
        // timer A in continuous mode
        TA1CTL = TASSEL_1 |     // ACLK
             ID_2 |             // divides by 4
             MC_2 |             // cont mode
             TACLR|             // Timer_A clear, sets TAR to 0
             TAIE;              // set timer A interrupt
        // P1IE &= ~b1;            // interrupt disable
        P1IFG &= ~b1;           // flag clear
     }
}
