// Lab 3 - Timer Module
// Basics: Using timer to set delays instead of delay function
// uses continuous or up mode
// uses a polling technique to read a flag continuously

// ------------------------------------------------------------------------------------------------------------------------

//// 3.1 - The Continuous Mode
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
//             TACLR;         // Timer_A clear, sets TAR to 0
//
//    TA0CTL &= ~TAIFG;       // ensures flag is clear at start
//
//	for (;;) {
//	    while (!(TA0CTL & TAIFG)){
//	        // empty loop
//	    }
//
//	    P1OUT ^= red;
//	    TA0CTL &= ~TAIFG;  // clears flag
//
//
//
//
//	}
//}

// ------------------------------------------------------------------------------------------------------------------------

//// 3.2 - The Up Mode
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
//    TA0CCR0 = 32768;                    // sets the LED toggle to 1 sec, using the HZ specified in lab manual
//                                        // sets num cycles needed in up mode
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
//    for (;;) {
//        while (!(TA0CTL & TAIFG)){
//            // empty loop
//        }
//
//        P1OUT ^= red;
//        TA0CTL &= ~TAIFG;  // clears flag
//
//
//
//
//    }
//}


// ------------------------------------------------------------------------------------------------------------------------

// 3.3 - Application Signal Repeater
#include <msp430fr6989.h>
#define red BIT0                        // red led to 1.0 (port 1, bit 0)
#define green BIT7                      // green led to 9.7
#define button BIT1                     // Button S2 at P1.1
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
    P1OUT &= ~red;
    P9OUT &= ~green;
    config_ACLK_to_32KHz_crystal();
    // configure Timer A
    TA0CTL = TASSEL_1 |     // sets to ACLK
             ID_0 |         // divides by 1
             MC_1 |         // sets to up mode
             TACLR;         // Timer_A clear, sets TAR to 0
    TA0CTL &= ~TAIFG;       // ensures flag is clear at start
    P1DIR &= ~button;                   // Set P1.1 as input
    P1REN |= button;                    // Enable pull resistor
    P1OUT |= button;                    // Set pull-up resistor
    unsigned int duration = 0;             // tracks length of pulse (button press and led lighting up)
    unsigned int uneligible = 0;             // tracks if the duration
    for (;;) {
            if (uneligible) {
                // Wait for button press to clear the error state
                if ((P1IN & button) == 0) {
                    if ((P1IN & button) == 0) {
                        P9OUT &= ~green;    // Turn off green LED
                        uneligible = 0;
                    }
                }
            } else {
                // Wait for button press to start sampling the pulse
                while ((P1IN & button) != 0);
                if ((P1IN & button) == 0) {
                    // Start sampling the pulse duration
                    TA0CTL = MC_0 | TACLR;  // Stop the timer and clear TAR
                    TA0CCR0 = 65535;       // Set the maximum duration for sampling
                    TA0CTL = TASSEL_1 | MC_1 | TACLR; // ACLK, up mode, clear TAR
                    // Wait for the pulse to end
                    while ((P1IN & button) == 0);
                    duration = TA0R;  // Capture the pulse duration
                    TA0CTL = MC_0 | TACLR;  // Stop the timer
                    if (duration >= 65535) {
                        // duration exceeded 65535 cycles, set uneligible
                        P9OUT |= green;    // Turn on red LED
                        uneligible = 1;
                    } else {
                        // Replay the pulse on the red LED
                        P1OUT |= red;       // Turn on red LED
                        TA0CCR0 = duration;
                        TA0CTL = TASSEL_1 | MC_1 | TACLR; // ACLK, up mode, clear TAR
                        // Wait for the timer to finish
                        while ((TA0CTL & TAIFG) == 0);
                        TA0CTL = MC_0 | TACLR; // Stop the timer
                        P1OUT &= ~red;      // Turn off red LED
                    }
                }
            }
        }
}

