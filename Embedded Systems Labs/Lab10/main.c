//// Lab 10
//
//// 10.1 -------------------------------------------------------------------------------------
//#include <msp430fr6989.h>
//#define red BIT0                    // Red at P1.0
//#define green BIT7                  // Green at P9.7
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
//    WDTCTL = WDTPW | WDTHOLD;       // Stop WDT
//    PM5CTL0 &= ~LOCKLPM5;             // Enable GPIO pins
//    config_ACLK_to_32KHz_crystal();
//
//    P1DIR |= red;
//    P1OUT &=  ~red;
//    P9DIR |= green;
//    P9OUT &= ~green;
//
//    // Configure Channel 0
//    // TA0CCR0 = ... // Find # cycles
//    // 0.1 seconds
//    TA0CCR0 = 819;
//    TA0CCTL0 |= CCIE;
//    TA0CCTL0 &= ~CCIFG;
//
//    // Configure Channel 1
//    // 0.5 seconds
//    TA0CCR1 = 4095;
//    TA0CCTL1 |= CCIE;
//    TA0CCTL1 &=  ~CCIFG;
//
//    // Start the timer (divide ACLK by 4)
//    TA0CTL = TASSEL_1 |         // sets to ACLK
//            ID_2 |              // divides by 4
//            MC_2 |              // sets to continuous mode
//            TACLR;              // Timer_A clear, sets TAR to 0
//
//    _enable_interrupts();
//
//    // Engage a low-power mode
//    _low_power_mode_3();        // LPM 3 shuts down all clocks besides ACLK -- most efficient choice
//
//    return;
//}
//
//// ISR of Channel 0 (A0 vector) -- given in lab manual
//#pragma vector = TIMER0_A0_VECTOR
//__interrupt void T0A0_ISR() {
//    P1OUT ^= red;               // Toggle the red LED
//    TA0CCR0 += 819;             // Schedule the next interrupt
//    // Hardware clears Channel 0 flag (CCIFG in TA0CCTL0)
//}
//
//#pragma vector = TIMER0_A1_VECTOR
//__interrupt void T0A1_ISR() {
//     P9OUT ^= green;            // Toggle the green LED
//     TA0CCR1 += 4095;
//     TA0CCTL1 &= ~CCIFG;        // Clear the flag
//}


//// 10.2 -------------------------------------------------------------------------------------
//#include <msp430fr6989.h>
//#define red BIT0                    // Red at P1.0
//#define green BIT7                  // Green at P9.7
//
//int flashing;                       // where 0 is false and 1 is true
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
//    WDTCTL = WDTPW | WDTHOLD;       // Stop WDT
//    PM5CTL0 &= ~LOCKLPM5;             // Enable GPIO pins
//    config_ACLK_to_32KHz_crystal();
//
//    P1DIR |= red;
//    P1OUT &=  ~red;
//    P9DIR |= green;
//    P9OUT &= ~green;
//
//    // Configure Channel 0
//    // TA0CCR0 = ... // Find # cycles
//    // 0.1 seconds
//    TA0CCR0 = 819;
//    TA0CCTL0 |= CCIE;
//    TA0CCTL0 &= ~CCIFG;
//
//    // Configure Channel 1
//    // 0.5 seconds
//    TA0CCR1 = 4095;
//    TA0CCTL1 |= CCIE;
//    TA0CCTL1 &=  ~CCIFG;
//
//    // Configure Channel 2
//    // 4 seconds
//    TA0CCR2 = 32760;
//    TA0CCTL2 |= CCIE;
//    TA0CCTL2 &=  ~CCIFG;
//
//    // Start the timer (divide ACLK by 4)
//    TA0CTL = TASSEL_1 |         // sets to ACLK
//            ID_2 |              // divides by 4
//            MC_2 |              // sets to continuous mode
//            TACLR;              // Timer_A clear, sets TAR to 0
//
//    _enable_interrupts();
//
//    // Engage a low-power mode
//    _low_power_mode_3();        // LPM 3 shuts down all clocks besides ACLK -- most efficient choice
//
//    return;
//}
//
//// ISR of Channel 0 (A0 vector) -- given in lab manual
//// ISR of Channel 0 (A0 vector)
//#pragma vector = TIMER0_A0_VECTOR
//__interrupt void T0A0_ISR() {
//     P1OUT ^= red; // Toggle the red LED
//     TA0CCR0 += 819; // Schedule the next interrupt
//     // Hardware clears Channel 0 flag (CCIFG in TA0CCTL0)
//}
//
//// ISR of A1 vector
//#pragma vector = TIMER0_A1_VECTOR
//__interrupt void T0A1_ISR() {
//    // Detect Channel 1 interrupt (check the flag)
//    flashing = 0; //Have a state/mode to configure in between
//    if((TA0CCTL1 & CCIFG)!=0) {
//         P9OUT ^= green; // Toggle the green LED
//         TA0CCR1 += 4095;
//         TA0CCTL1 &= ~CCIFG; // Clear the flag
//    }
//
//    // Detect Channel 2 interrupt
//    if((TA0CCTL2 & CCIFG)!=0) {
//     // During Channel 2 keep the lights off for 4 seconds
//         if(flashing == 0) {
//             // leds off
//             P1OUT &= ~red;
//             P9OUT &= ~green;
//
//             // interrupt disable so flashing sequences dont occur
//             TA0CCTL0 &= ~CCIE;
//             TA0CCTL1 &= ~CCIE;
//
//             // flags disabled
//             TA0CCTL0 &= ~CCIFG;
//             TA0CCTL1 &= ~CCIFG;
//
//             // change state
//             flashing = 1;
//         }
//
//         if(flashing == 1) {
//             // flags enable so sequence can resume
//             TA0CCTL0 |= CCIE;
//             TA0CCTL1 |= CCIE;
//
//             //timing the next interrupt (4 seconds)
//             TA0CCR2 = TA0R + 32760;
//             TA0CCR1 = TA0R + 32760;
//             TA0CCR0 = TA0R + 32760;
//
//             // flags are cleared
//             TA0CCTL0 &= ~CCIFG;
//             TA0CCTL1 &= ~CCIFG;
//
//             // change state
//             flashing = 0;
//          }
//
//         TA0CCR2 += 32760;          // 4 seconds interval
//         TA0CCTL2 &= ~CCIFG;        // flag disable
//        }
//}

// 10.3 -------------------------------------------------------------------------------------
#include <msp430fr6989.h>
#define PWM_PIN BIT0

#define red BIT0            // Red at P1.0
#define green BIT7           //Green LED at P9.7

#define FLAGS UCA1IFG        //Contains the transmit & receive flag
#define RXFLAG UCRXIFG      //Recieve flag
#define TXFLAG UCTXIFG      //Transmit buffer
#define TXBUFFER UCA1TXBUF  //Transmit buffer
#define RXBUFFER UCA1RXBUF  //Receive buffer

#define midLow 1990         // lower level of joystick's natural position
#define midHigh 2100        // upper level of joystick's natural position
#define midpoint 2048        // midpoint of joystick at rest
#define maximum 4095        // max value of joystick

int prevBrightnessLevel = 32; // stores the last brightness
int y;

void main(void) {

     WDTCTL = WDTPW | WDTHOLD; // Stop WDT
     PM5CTL0 &= ~LOCKLPM5;

     // Configure pin to TA0.1 functionality (complete last two lines)
     P1DIR |= PWM_PIN; // P1DIR bit = 1
     P1SEL1 &= PWM_PIN; // P1SEL1 bit = 0
     P1SEL0 |= PWM_PIN; // P1SEL0 bit = 1

     P9DIR |= green;
     P9OUT &= ~green;

     config_ACLK_to_32KHz_crystal();
     Initialize_ADC();

     TA0CCTL1 |= OUTMOD_7; // Modify OUTMOD field to 7
     TA0CCR1 = 32; // Modify this value between 0 and
     // 32 to adjust the brightness level
     // Starting the timer in the up mode; period = 0.001 seconds
     // (ACLK @ 32 KHz) (Divide by 1) (Up mode)
     TA0CCR0 = 33; // @ 32 KHz --> 0.001 seconds (1000 Hz)
     TA0CTL = TASSEL_1 |
                 ID_0 |
                 MC_1 |
                 TACLR;
     for (;;) {
         // Start ADC conversion for horizontal position
         ADC12CTL0 |= ADC12SC;

         while ((ADC12CTL1 & ADC12BUSY) != 0) {} // Wait for conversion to complete

         // set brightness to previous level
             TA0CCR1 = prevBrightnessLevel;

         if (ADC12MEM0 <= midLow || ADC12MEM0 >= midHigh) { // joystick has moved outside of resting positions
             int newBrightnessLevel = (ADC12MEM0 * 32) / maximum;
             if(newBrightnessLevel < 1){
                 TA0CCR1 = 1;                               // PWM will adjust brightness level
             }
             else{
                 TA0CCR1 = newBrightnessLevel;              // PWM duty cycle altered so that new brightness changed
             }
             prevBrightnessLevel = newBrightnessLevel;      // lastbrightness is now current brightness for the next cycle
         }
     __delay_cycles(50000);
     }
}

void config_ACLK_to_32KHz_crystal() {
     PJSEL1 &= ~BIT4;
     PJSEL0 |= BIT4;
     CSCTL0 = CSKEY;
     do {
         CSCTL5 &= ~LFXTOFFG;
         SFRIFG1 &= ~OFIFG;
     } while((CSCTL5 & LFXTOFFG) != 0);
     CSCTL0_H = 0;
     return;
}

void Initialize_ADC() {
     P9SEL1 |= BIT2;
     P9SEL0 |= BIT2;
     ADC12CTL0 |= ADC12ON;
     ADC12CTL0 &= ~ADC12ENC;
     ADC12CTL0|=ADC12MSC|ADC12SHT0_4; //32 ADC12CLK cycles
     ADC12CTL1= ADC12SHS_0|ADC12SHP|ADC12DIV_7|ADC12SSEL_0;
     ADC12CTL1 |= ADC12CONSEQ_1;
     ADC12CTL2|= ADC12RES_2;
     ADC12CTL3 = ADC12CSTARTADD_0;
     ADC12MCTL0|= ADC12INCH_10|ADC12VRSEL_0;
     ADC12MCTL1 |= ADC12INCH_4 | ADC12VRSEL_0 | ADC12EOS;
     ADC12CTL0 |= ADC12ENC;
     return;
}
