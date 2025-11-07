//// 2.1 - Turning on Red LED while button S1 pushed
//#include <msp430fr6989.h>
//#define red BIT0                        // red led to 1.0 (port 1, bit 0)
//#define green BIT7                      // green led to 9.7
//#define BUTTON1 BIT1                    // button 1 to 1.1
//#define BUTTON2 BIT2                    // button 2 to 1.2
//
//void main(void)
//{
//	WDTCTL = WDTPW | WDTHOLD;	        // stop watchdog timer
//	PM5CTL0 &= ~LOCKLPM5;               // enable GPIO pins
//
//	// configure/initialize LEDs
//	P1DIR |= red;                       // direct pin1 as output for red (1.0)
//	P9DIR |= green;                     // direct pin9 as output for green (9.7)
//	P1OUT &= ~red;                      // red led off (clear)
//	P9OUT &= ~green;                    // green led off
//
//	//configure the buttons
//	P1DIR &= ~BUTTON1;                  // direct pin as input
//
//	P1REN |= BUTTON1;                   // enable the built in resistor (sets "port 1 resistor enable")
//	P1OUT |= BUTTON1;                   // set resistor as pull up
//	                                    // pull up resistor makes input pin state known when button not pressed
//
//	for(;;) {
//	    if((BUTTON1 & P1IN)== 0) {      // checks if button is pushed
//	        P1OUT |= red;               // toggle LED on (set "red" bit)
//	    }
//	    else {                          // checks if button is not pushed
//	        P1OUT &= ~red;              // clear LED (turn off)
//	    }
//	}
//}

// ------------------------------------------------------------------------------------------------------------

//// 2.2 - Using 2 push buttons independently of each other
//#include <msp430fr6989.h>
//#define red BIT0                        // red led to 1.0 (port 1, bit 0)
//#define green BIT7                      // green led to 9.7
//#define BUTTON1 BIT1                    // button 1 to 1.1
//#define BUTTON2 BIT2                    // button 2 to 1.2
//
//void main(void)
//{
//    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;               // enable GPIO pins
//
//    // configure/initialize LEDs
//    P1DIR |= red;                       // direct pin1 as output for red (1.0)
//    P9DIR |= green;                     // direct pin9 as output for green (9.7)
//    P1OUT &= ~red;                      // red led off (clear)
//    P9OUT &= ~green;                    // green led off
//
//    //configure button 1
//    P1DIR &= ~BUTTON1;                  // direct pin as input
//    P1REN |= BUTTON1;                   // enable the built in resistor (sets "port 1 resistor enable")
//    P1OUT |= BUTTON1;                   // set resistor as pull up
//                                        // pull up resistor makes input pin state known when button not pressed
//
//    //configure button 2
//    P1DIR &= ~BUTTON2;                  // direct pin as input
//    P1REN |= BUTTON2;                   // enable the built in resistor (sets "port 1 resistor enable")
//    P1OUT |= BUTTON2;                   // set resistor as pull up
//                                        // pull up resistor makes input pin state known when button not pressed
//
//    for(;;) {                           // code runs simultaneously, always checking the conditions inside
//        if((BUTTON1 & P1IN)== 0) {      // checks if button 1 is pushed by ANDing BUTTON1 mask with input reg mask
//            P1OUT |= red;               // toggle LED on (set "red" bit)
//        }
//        else {                          // checks if button 1 is not pushed
//            P1OUT &= ~red;              // clear LED (turn off)
//        }
//
//        if((BUTTON2 & P1IN)== 0) {      // checks if button 2 is pushed by ANDing BUTTON1 mask with input reg mask
//            P9OUT |= green;             // toggle LED on (set "green" bit)
//        }
//        else {                          // checks if button 2 is not pushed
//            P9OUT &= ~green;            // clear LED (turn off)
//        }
//    }
//}

// ------------------------------------------------------------------------------------------------------------

//// 2.3 - First Request Only
//#include <msp430fr6989.h>
//#define red BIT0                        // red led to 1.0 (port 1, bit 0)
//#define green BIT7                      // green led to 9.7
//#define BUTTON1 BIT1                    // button 1 to 1.1
//#define BUTTON2 BIT2                    // button 2 to 1.2
//
//void main(void)
//{
//    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;               // enable GPIO pins
//
//    // configure/initialize LEDs
//    P1DIR |= red;                       // direct pin1 as output for red (1.0)
//    P9DIR |= green;                     // direct pin9 as output for green (9.7)
//    P1OUT &= ~red;                      // red led off (clear)
//    P9OUT &= ~green;                    // green led off
//
//    //configure button 1
//    P1DIR &= ~BUTTON1;                  // direct pin as input
//    P1REN |= BUTTON1;                   // enable the built in resistor (sets "port 1 resistor enable")
//    P1OUT |= BUTTON1;                   // set resistor as pull up
//                                        // pull up resistor makes input pin state known when button not pressed
//
//    //configure button 2
//    P1DIR &= ~BUTTON2;                  // direct pin as input
//    P1REN |= BUTTON2;                   // enable the built in resistor (sets "port 1 resistor enable")
//    P1OUT |= BUTTON2;                   // set resistor as pull up
//                                        // pull up resistor makes input pin state known when button not pressed
//
//    for(;;) {                           // code runs simultaneously, always checking the conditions inside
//        while((BUTTON1 & P1IN)== 0) {      // checks if button 1 is pushed by ANDing BUTTON1 mask with input reg mask
//            P1OUT |= red;                 // toggle LED on (set "red" bit) until button released
//        }
//        P1OUT &= ~red;                    // clear LED (turn off)
//
//        while((BUTTON2 & P1IN)== 0) {      // checks if button 2 is pushed by ANDing BUTTON1 mask with input reg mask
//            P9OUT |= green;             // toggle LED on (set "green" bit) until button released
//        }
//        P9OUT &= ~green;                // clear LED (turn off)
//
//    }
//}

// ------------------------------------------------------------------------------------------------------------

// 2.4 - Both buttons initiate, either button keeps, both released turns off LEDs
#include <msp430fr6989.h>
#define red BIT0                        // red led to 1.0 (port 1, bit 0)
#define green BIT7                      // green led to 9.7
#define BUTTON1 BIT1                    // button 1 to 1.1
#define BUTTON2 BIT2                    // button 2 to 1.2

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               // enable GPIO pins

    // configure/initialize LEDs
    P1DIR |= red;                       // direct pin1 as output for red (1.0)
    P9DIR |= green;                     // direct pin9 as output for green (9.7)
    P1OUT &= ~red;                      // red led off (clear)
    P9OUT &= ~green;                    // green led off

    //configure button 1
    P1DIR &= ~BUTTON1;                  // direct pin as input
    P1REN |= BUTTON1;                   // enable the built in resistor (sets "port 1 resistor enable")
    P1OUT |= BUTTON1;                   // set resistor as pull up
                                        // pull up resistor makes input pin state known when button not pressed

    //configure button 2
    P1DIR &= ~BUTTON2;                  // direct pin as input
    P1REN |= BUTTON2;                   // enable the built in resistor (sets "port 1 resistor enable")
    P1OUT |= BUTTON2;                   // set resistor as pull up
                                        // pull up resistor makes input pin state known when button not pressed


    for(;;) {                           // code runs simultaneously, always checking the conditions inside

        if ((P1IN & BUTTON1) == 0) {     // if button 1 (red) pressed first
            // Both buttons are pressed, keep red LED lit
            P1OUT |= red;
            if (((P1IN & BUTTON1) == 0) && ((P1IN & BUTTON2) == 0)){
                while (((P1IN & BUTTON1) == 0) || ((P1IN & BUTTON2) == 0)){
                    P1OUT &= ~red;           // Turn on red LED
                }

            }

        }

        if ((P1IN & BUTTON2) == 0) {     // if button 1 (red) pressed first
             // Both buttons are pressed, keep red LED lit
             P9OUT |= green;
             if (((P1IN & BUTTON1) == 0) && ((P1IN & BUTTON2) == 0)){
                 while (((P1IN & BUTTON1) == 0) || ((P1IN & BUTTON2) == 0)){
                     P9OUT &= ~green;           // Turn on red LED
                 }

             }

         }

        P1OUT &= ~red;
        P9OUT &= ~green;

//        if ((P1IN & BUTTON2) == 0) {     // if button 1 (green) pressed first
//            // Both buttons are pressed, keep red LED lit
//            while (((P1IN & BUTTON1) == 0) || ((P1IN & BUTTON2) == 0)){
//                P9OUT |= green;           // Turn on red LED
//            }
//
//        }
//
//        P1OUT &= ~red;    // Turn off red LED
//        P9OUT &= ~green;  // Turn off green LED


// improper instructions by TA
//    for(;;) {                           // code runs simultaneously, always checking the conditions inside
//
//
//
//        if (((P1IN & BUTTON1) == 0) && ((P1IN & BUTTON2) == 0)) {
//            // Both buttons are pressed, turn off both LEDs
//            P1OUT &= ~red;   // Turn off red LED
//            P9OUT &= ~green; // Turn off green LED
//
//            while((P1IN & BUTTON1) == 0 || (P1IN & BUTTON2) == 0) {
//                P1OUT &= ~red;   // Turn off red LED
//                P9OUT &= ~green; // Turn off green LED
//            }
//
//        }
//        else {
//            // Neither button is pressed, turn on both LEDs
//            P1OUT |= red;    // Turn on red LED
//            P9OUT |= green;  // Turn on green LED
//        }
//
//

//    }
    }
}
