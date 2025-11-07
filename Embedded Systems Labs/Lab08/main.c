// Lab 8 - ADC

////// 8.1 - Using the ADC SAR-type
//#include <msp430fr6989.h>
//#define red BIT0            // Red @ P1.0
//#define green BIT7          // Green @ P9.7
//#define b1 BIT1             // button 1 at P1.1
//#define b2 BIT2             // button 2 at P1.2
//
//// flag definitions provided in the the lab manual
//#define FLAGS UCA1IFG       // Contains the transmit & receive flags
//#define RXFLAG UCRXIFG      // Receive flag
//#define TXFLAG UCTXIFG      // Transmit flag
//#define TXBUFFER UCA1TXBUF  // Transmit buffer
//#define RXBUFFER UCA1RXBUF  // Receive buffer
//
//// function headers
//void Initialize_ADC();
//void Initialize_UART();
//void uart_write_uint16(unsigned int n);
//void uart_write_string(char *str);
//void uart_write_char(unsigned char ch);
//
//
//void main(void) {
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;       // enable gen purpose input output (GPIO) pins
//
//    P1DIR |= red;
//    P1OUT &= ~red;
//
//    Initialize_UART();
//    Initialize_ADC();
//
//    while(1){
//       ADC12CTL0 |= ADC12SC;
//
//        while ((ADC12CTL0 & ADC12BUSY)!=0);   // awaiting conversion completion
//
//        unsigned int result = ADC12MEM0;
//        uart_write_string("X: ");
//        uart_write_uint16(result);                   // 16 bit #
//        uart_write_char('\r');
//        uart_write_char('\n');
//        _delay_cycles(150000);
//
//        P1OUT ^= red;
//    }
//}
//
//
//void Initialize_ADC() {
//     // Divert the pins to analog functionality
//     // X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
//     P9SEL1 |= BIT2;
//     P9SEL0 |= BIT2;
//
//     // Turn on the ADC module
//     ADC12CTL0 |= ADC12ON;
//     // Turn off ENC (Enable Conversion) bit while modifying the configuration
//     ADC12CTL0 &= ~ADC12ENC;
//     //*************** ADC12CTL0 ***************
//     // Set ADC12SHT0 (select the number of cycles that you determined)
//     // pg 895 Fam User Guide
//     ADC12CTL0 |= ADC12ON | ADC12SHT0_8;
//     //*************** ADC12CTL1 ***************
//     // ADC12SC bit as the trigger/ADC12SHS BIT as the trigger)/ ADC12SSEL (select MODOSC)/
//     // Set ADC12SHP bit
//     /// Set ADC12DIV divider
//     ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0;
//     //*************** ADC12CTL2 ***************
//     // Set ADC12RES (select 12-bit resolution)
//     // Set ADC12DF (select unsigned binary format)
//     ADC12CTL2 |= ADC12ENC;
//     //*************** ADC12CTL3 ***************
//     // Leave all fields at default values
//     //*************** ADC12MCTL0 ***************
//     // Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
//     // Set ADC12INCH (select channel A10)
//     ADC12MCTL0 |= ADC12INCH_10 | ADC12VRSEL_0;
//     // Turn on ENC (Enable Conversion) bit at the end of the configuration
//     ADC12CTL0 |= ADC12ENC;
//     return;
//}
//
//
//
//void Initialize_UART() {
//     // Configure pins to UART functionality
//     P3SEL1 &= ~(BIT4 | BIT5);
//     P3SEL0 |= (BIT4 | BIT5);
//     // Main configuration register
//     UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
//     // Most fields in this register, when set to zero, correspond to the
//     // popular configuration
//     UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK
//     // Configure the clock dividers and modulators (and enable oversampling)
//     UCA1BRW = 6; // divider
//     // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
//     // UCBRS = 0x22 = 0010 0010
//     UCA1MCTLW = UCBRS5 | UCBRS1 | UCBRF3 | UCBRF2 | UCBRF0 | UCOS16 ;
//     // Exit the reset state
//     UCA1CTLW0 &= ~UCSWRST;
//}
//
//void uart_write_uint16(unsigned int n) {
//     if (n == 0) {
//         uart_write_char('0'); // If n is 0, transmit '0'
//         return;
//     }
//     // Create a temporary buffer to store the digits
//     char buffer[6]; // Max 5 digits + null-terminator
//     int count = 0;// Parse the integer into digits and store them in the buffer
//     while (n > 0) {buffer[count] = '0' + (n % 10); // Convert the digit to ASCII
//         n /= 10;
//         count++;
//     }
//     // Transmit the digits in reverse order (from the buffer)
//     while (count > 0) {
//         count--;
//         uart_write_char(buffer[count]);
//     }
//}
//void uart_write_string(char *str) {
//    while (*str) {
//        uart_write_char(*str++); // moves to the next character in the string
//    }
//}
//void uart_write_char(unsigned char ch){
//    // Wait for any ongoing transmission to complete
//    while ((FLAGS & TXFLAG) == 0) {}
//    // Copy the byte to the transmit buffer
//    TXBUFFER = ch; // Tx flag goes to 0 and Tx begins!
//}
//

//------------------------------------------------------------------------------------------------------------------------------

////// 8.2 - X & Y
//#include <msp430fr6989.h>
//#define red BIT0            // Red @ P1.0
//#define green BIT7          // Green @ P9.7
//#define b1 BIT1             // button 1 at P1.1
//#define b2 BIT2             // button 2 at P1.2
//
//// flag definitions provided in the the lab manual
//#define FLAGS UCA1IFG       // Contains the transmit & receive flags
//#define RXFLAG UCRXIFG      // Receive flag
//#define TXFLAG UCTXIFG      // Transmit flag
//#define TXBUFFER UCA1TXBUF  // Transmit buffer
//#define RXBUFFER UCA1RXBUF  // Receive buffer
//
//// function headers
//void Initialize_ADC();
//void Initialize_UART();
//void uart_write_uint16(unsigned int n);
//void uart_write_string(char *str);
//void uart_write_char(unsigned char ch);
//
//
//void main(void) {
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;       // enable gen purpose input output (GPIO) pins
//
//    P1DIR |= red;
//    P1OUT &= ~red;
//
//    Initialize_UART();
//    Initialize_ADC();
//
//    while(1){
//       ADC12CTL0 |= ADC12SC;
//
//        while ((ADC12CTL0 & ADC12BUSY)!=0);   // awaiting conversion completion
//
//        unsigned int resultX = ADC12MEM0;
//        unsigned int resultY = ADC12MEM1;
//
//        uart_write_string("( ");
//        uart_write_uint16(resultX);
//        uart_write_string(" , ");
//        _delay_cycles(150000);
//        uart_write_uint16(resultY);
//        uart_write_string(" )");
//
//        uart_write_char('\r');
//        uart_write_char('\n');
//        _delay_cycles(150000);
//
//        P1OUT ^= red;
//        _delay_cycles(150000);
//    }
//}
//
//
//void Initialize_ADC() {
//     // Divert the pins to analog functionality
//     // X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
//     P9SEL1 |= BIT2;
//     P9SEL0 |= BIT2;
//
//     // Divert the pins to analog functionality
//     // Y-axis: A4/P8.7, for A4 (P8DIR=y, P8SEL1=1, P8SEL0=1)
//     P8SEL1 |= BIT7;
//     P8SEL0 |= BIT7;
//
//     // Turn on the ADC module
//     ADC12CTL0 |= ADC12ON;
//     // Turn off ENC (Enable Conversion) bit while modifying the configuration
//     ADC12CTL0 &= ~ADC12ENC;
//
//     //*************** ADC12CTL0 ***************
//      // Set the bit ADC12MSC (Multiple Sample and Conversion)
//      ADC12CTL0 |= ADC12SHT0_2 | ADC12MSC;
//
//
//      //*************** ADC12CTL1 ***************
//      // Set ADC12CONSEQ (select sequence-of-channels)
//      ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0;
//      ADC12CTL1 |= ADC12CONSEQ_1;
//
//
//     //*************** ADC12CTL2 ***************
//     // Set ADC12RES (select 12-bit resolution)
//     // Set ADC12DF (select unsigned binary format)
//      ADC12CTL2 |= ADC12RES_2;
//
//     //*************** ADC12CTL3 ***************
//     // Leave all fields at default values
//     // Set ADC12CSTARTADD to 0 (first conversion in ADC12MEM0)
//      ADC12CTL3 = ADC12CSTARTADD_0;
//
//     //*************** ADC12MCTL0 ***************
//     // Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
//     // Set ADC12INCH (select channel A10)
//      ADC12MCTL0 |= ADC12INCH_10 | ADC12VRSEL_0;
//      ADC12MCTL1 |= ADC12INCH_4 | ADC12VRSEL_0|ADC12EOS;
//
//     // Turn on ENC (Enable Conversion) bit at the end of the configuration
//     ADC12CTL0 |= ADC12ENC;
//
//     return;
//}
//
//
//
//void Initialize_UART() {
//     // Configure pins to UART functionality
//     P3SEL1 &= ~(BIT4 | BIT5);
//     P3SEL0 |= (BIT4 | BIT5);
//     // Main configuration register
//     UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
//     // Most fields in this register, when set to zero, correspond to the
//     // popular configuration
//     UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK
//     // Configure the clock dividers and modulators (and enable oversampling)
//     UCA1BRW = 6; // divider
//     // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
//     // UCBRS = 0x22 = 0010 0010
//     UCA1MCTLW = UCBRS5 | UCBRS1 | UCBRF3 | UCBRF2 | UCBRF0 | UCOS16 ;
//     // Exit the reset state
//     UCA1CTLW0 &= ~UCSWRST;
//}
//
//void uart_write_uint16(unsigned int n) {
//     if (n == 0) {
//         uart_write_char('0'); // If n is 0, transmit '0'
//         return;
//     }
//     // Create a temporary buffer to store the digits
//     char buffer[6]; // Max 5 digits + null-terminator
//     int count = 0;// Parse the integer into digits and store them in the buffer
//     while (n > 0) {buffer[count] = '0' + (n % 10); // Convert the digit to ASCII
//         n /= 10;
//         count++;
//     }
//     // Transmit the digits in reverse order (from the buffer)
//     while (count > 0) {
//         count--;
//         uart_write_char(buffer[count]);
//     }
//}
//void uart_write_string(char *str) {
//    while (*str) {
//        uart_write_char(*str++); // moves to the next character in the string
//    }
//}
//void uart_write_char(unsigned char ch){
//    // Wait for any ongoing transmission to complete
//    while ((FLAGS & TXFLAG) == 0) {}
//    // Copy the byte to the transmit buffer
//    TXBUFFER = ch; // Tx flag goes to 0 and Tx begins!
//}

//------------------------------------------------------------------------------------------------------------------------------

//// 8.3 - Platform Balancing Control
#include <msp430fr6989.h>
#define red BIT0            // Red @ P1.0
#define green BIT7          // Green @ P9.7
#define b1 BIT1             // button 1 at P1.1
#define b2 BIT2             // button 2 at P1.2

// flag definitions provided in the the lab manual
#define FLAGS UCA1IFG       // Contains the transmit & receive flags
#define RXFLAG UCRXIFG      // Receive flag
#define TXFLAG UCTXIFG      // Transmit flag
#define TXBUFFER UCA1TXBUF  // Transmit buffer
#define RXBUFFER UCA1RXBUF  // Receive buffer

// function headers
void Initialize_ADC();
void Initialize_UART();
void uart_write_uint16(unsigned int n);
void uart_write_string(char *str);
void uart_write_char(unsigned char ch);


void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // enable gen purpose input output (GPIO) pins

    P1DIR |= red;
    P1OUT &= ~red;

    Initialize_UART();
    Initialize_ADC();

    unsigned int rightTop = 127;
    unsigned int leftTop = 127;
    unsigned int rightBottom = 127;
    unsigned int leftBottom = 127;

    // acting as true/false with 1/0 to detect if remote has moved
    unsigned int movedX = 0;
    unsigned int movedY = 0;

    printBox(leftTop, rightTop, leftBottom, rightBottom);

    unsigned int prevX = 0;
    unsigned int prevY = 0;

    while(1){
        ADC12CTL0 |= ADC12SC;

        while ((ADC12CTL0 & ADC12BUSY)!=0);   // awaiting conversion completion

        unsigned int resultX = ADC12MEM0;
        unsigned int resultY = ADC12MEM1;

        // need to add button usage and options on the screen

        if((prevX != 0) && (prevY != 0)) {
            unsigned int diffX = resultX - prevX;
            if((diffX > 10) || (diffX < -10)) {     // checks if moved intentionally or natural drifting value
                movedX = 1;
            }
            unsigned int diffY = resultY - prevY;
            if((diffY > 10) || (diffY < -10)) {
                movedY = 1;
            }
        }

        // here we would would check if X moved right or left in order to establish which
        // coordinate of the box we are at
        // additionally, we would check if y moved up or down in order to see if we change
        // any of the 4 numeric values
        // then, we would change the value accordingly
        // new call to printbox with the changed value

        prevX = resultX;
        prevY = resultY;

    }
}


void Initialize_ADC() {
     // Divert the pins to analog functionality
     // X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
     P9SEL1 |= BIT2;
     P9SEL0 |= BIT2;

     // Divert the pins to analog functionality
     // Y-axis: A4/P8.7, for A4 (P8DIR=y, P8SEL1=1, P8SEL0=1)
     P8SEL1 |= BIT7;
     P8SEL0 |= BIT7;

     // Turn on the ADC module
     ADC12CTL0 |= ADC12ON;
     // Turn off ENC (Enable Conversion) bit while modifying the configuration
     ADC12CTL0 &= ~ADC12ENC;

     //*************** ADC12CTL0 ***************
      // Set the bit ADC12MSC (Multiple Sample and Conversion)
      ADC12CTL0 |= ADC12SHT0_2 | ADC12MSC;


      //*************** ADC12CTL1 ***************
      // Set ADC12CONSEQ (select sequence-of-channels)
      ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0;
      ADC12CTL1 |= ADC12CONSEQ_1;


     //*************** ADC12CTL2 ***************
     // Set ADC12RES (select 12-bit resolution)
     // Set ADC12DF (select unsigned binary format)
      ADC12CTL2 |= ADC12RES_2;

     //*************** ADC12CTL3 ***************
     // Leave all fields at default values
     // Set ADC12CSTARTADD to 0 (first conversion in ADC12MEM0)
      ADC12CTL3 = ADC12CSTARTADD_0;

     //*************** ADC12MCTL0 ***************
     // Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
     // Set ADC12INCH (select channel A10)
      ADC12MCTL0 |= ADC12INCH_10 | ADC12VRSEL_0;
      ADC12MCTL1 |= ADC12INCH_4 | ADC12VRSEL_0|ADC12EOS;

     // Turn on ENC (Enable Conversion) bit at the end of the configuration
     ADC12CTL0 |= ADC12ENC;

     return;
}



void Initialize_UART() {
     // Configure pins to UART functionality
     P3SEL1 &= ~(BIT4 | BIT5);
     P3SEL0 |= (BIT4 | BIT5);
     // Main configuration register
     UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
     // Most fields in this register, when set to zero, correspond to the
     // popular configuration
     UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK
     // Configure the clock dividers and modulators (and enable oversampling)
     UCA1BRW = 6; // divider
     // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
     // UCBRS = 0x22 = 0010 0010
     UCA1MCTLW = UCBRS5 | UCBRS1 | UCBRF3 | UCBRF2 | UCBRF0 | UCOS16 ;
     // Exit the reset state
     UCA1CTLW0 &= ~UCSWRST;
}

void uart_write_uint16(unsigned int n) {
     if (n == 0) {
         uart_write_char('0'); // If n is 0, transmit '0'
         return;
     }
     // Create a temporary buffer to store the digits
     char buffer[6]; // Max 5 digits + null-terminator
     int count = 0;// Parse the integer into digits and store them in the buffer
     while (n > 0) {buffer[count] = '0' + (n % 10); // Convert the digit to ASCII
         n /= 10;
         count++;
     }
     // Transmit the digits in reverse order (from the buffer)
     while (count > 0) {
         count--;
         uart_write_char(buffer[count]);
     }
}
void uart_write_string(char *str) {
    while (*str) {
        uart_write_char(*str++); // moves to the next character in the string
    }
}
void uart_write_char(unsigned char ch){
    // Wait for any ongoing transmission to complete
    while ((FLAGS & TXFLAG) == 0) {}
    // Copy the byte to the transmit buffer
    TXBUFFER = ch; // Tx flag goes to 0 and Tx begins!
}

void printBox(unsigned int tL, unsigned int tR, unsigned int bL, unsigned int bR) {

    // add a balance tracking function which compares all 4 of the numbers to make
    // sure they are within range

    uart_write_string("PLATFROM BALANCING CONTROL");
        uart_write_char('\r');
        uart_write_char('\n');
        uart_write_char('\n');

        uart_write_string("\t   TOP");
        uart_write_char('\r');
        uart_write_char('\n');
        uart_write_string("   ---------------------");
        uart_write_char('\r');
        uart_write_char('\n');

        //row 1
        uart_write_string("  |");
        uart_write_uint16(tL);
        uart_write_string("             ");
        uart_write_uint16(tR);
        uart_write_string("  |");
        uart_write_char('\r');
        uart_write_char('\n');


        //row 2
        uart_write_string(" L|");
        uart_write_string("                   ");
        uart_write_string("  | R");
        uart_write_char('\r');
        uart_write_char('\n');

        //row 3
        uart_write_string(" E|");
        uart_write_string("                   ");
        uart_write_string("  | I");
        uart_write_char('\r');
        uart_write_char('\n');

        //row 4
        uart_write_string(" F|");
        uart_write_string("                   ");
        uart_write_string("  | G");
        uart_write_char('\r');
        uart_write_char('\n');

        //row 5
        uart_write_string(" T|");
        uart_write_string("                   ");
        uart_write_string("  | H");
        uart_write_char('\r');
        uart_write_char('\n');

        //row 6
        uart_write_string("  |");
        uart_write_string("                   ");
        uart_write_string("  | T");
        uart_write_char('\r');
        uart_write_char('\n');


        uart_write_string("  |");
        uart_write_uint16(bL);
        uart_write_string("             ");
        uart_write_uint16(bR);
        uart_write_string("  |");
        uart_write_char('\r');
        uart_write_char('\n');


        uart_write_string("   ---------------------");
        uart_write_char('\r');
        uart_write_char('\n');
        uart_write_string("\t   BOTTOM");
        uart_write_char('\r');
        uart_write_char('\n');

}







