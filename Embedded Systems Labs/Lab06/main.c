// Lab 6 - UART

// 6.1 - Transmitting data over UART
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
void uart_write_char(unsigned char ch);
void delay();
unsigned char uart_read_char(void);
void Initialize_UART();


void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // enable gen purpose input output (GPIO) pins

    Initialize_UART();

    P1DIR |= red;           // configure red led
    P9DIR |= green;         // configure green led

    P1OUT &= ~red;          // red LED set to off
    P9OUT &= ~green;        // green LED set to off

    while(1){

        unsigned int i;
        unsigned int data;

        for(i='0'; i<='9'; i++){

            uart_write_char(i);     // write character, will iterate 0-9
            uart_write_char('\n');  // write new line
            uart_write_char('\r');  // carriage return

            P1OUT ^= red;           // toggle
            delay ();               // delay so the toggle is visible

            unsigned char data = uart_read_char();

            if(data == '1'){
                P9OUT |= green; // green on when 1 is read
            }

            if(data == '2'){
                P9OUT &= ~green; //green off when 2 read
            }
        }
    }

}

// Configure UART to the popular configuration
// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
// no flow control, over sampling reception
// Clock: SMCLK @ 1 MHz (1,000,000 Hz)
void Initialize_UART(void){
    // Configure pins to back channel UART
    P3SEL1 &=  ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    // Main configuration register
    UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
                         // Most fields in this register, when set to zero, correspond to the
                         // popular configuration

    UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK

                 // Configure the clock dividers and modulators (and enable oversampling)
    UCA1BRW = 6; // divider, given in lab -- can be found in family user's guide

    // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
    // UCBRS = 0x20 = 0010 0000 = UCBRS5 (bit #5)
    UCA1MCTLW = UCBRF3 | UCBRS5 | UCOS16;

    UCA1CTLW0 &= ~UCSWRST;  // Exit the reset state
}

unsigned char uart_read_char(void){ // returns byte or NULL

    unsigned char temp;
    if( (FLAGS & RXFLAG) == 0) { // return NULL if no byte
        return 0;
    }
                                // temp variable into receive buffer
    temp = RXBUFFER;            // else, copy byte which clears flag
    return temp;                // return value
}

void uart_write_char(unsigned char ch){
    while ( (FLAGS & TXFLAG)==0 ) {} // Wait for any transmission to finish
    TXBUFFER = ch; // copy byte to transmit buffer | Tx flag --> 0 and Tx starts
    return;
}

void delay(){
    // delay allows things like LED blinking to have enough time to be processed
    volatile unsigned int i;
    for(i=0; i<50000; i++){}
}

//// ------------------------------------------------------------------------------------
//
//// 6.2 - Transmitting integers and strings over UART
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
//void uart_write_char(unsigned char ch);
//void delay();
//unsigned char uart_read_char(void);
//void Initialize_UART();
//
//void uart_write_uint16(unsigned int n);
//void uart_write_string(char* str);
//
//
//void main(void) {
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;       // enable gen purpose input output (GPIO) pins
//
//    Initialize_UART();
//
//    P1DIR |= red;           // configure red led
//    P9DIR |= green;         // configure green led
//
//    P1OUT &= ~red;          // red LED set to off
//    P9OUT &= ~green;        // green LED set to off
//
//    while(1){
//
//        uart_write_string("Numbers for test: ");    // establish what is printing
//        uart_write_uint16(65535);                   // 16 bit #
//        uart_write_char('\r');                      // carriage return
//
//        P1OUT ^= red;                               // toggle red LED
//        delay();
//    }
//
//}
//
//// Configure UART to the popular configuration
//// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
//// no flow control, over sampling reception
//// Clock: SMCLK @ 1 MHz (1,000,000 Hz)
//void Initialize_UART(void){
//    // Configure pins to back channel UART
//    P3SEL1 &=  ~(BIT4|BIT5);
//    P3SEL0 |= (BIT4|BIT5);
//
//    // Main configuration register
//    UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
//                         // Most fields in this register, when set to zero, correspond to the
//                         // popular configuration
//
//    UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK
//
//                 // Configure the clock dividers and modulators (and enable oversampling)
//    UCA1BRW = 6; // divider, given in lab -- can be found in family user's guide
//
//    // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
//    // UCBRS = 0x20 = 0010 0000 = UCBRS5 (bit #5)
//    UCA1MCTLW = UCBRF3 | UCBRS5 | UCOS16;
//
//    UCA1CTLW0 &= ~UCSWRST;  // Exit the reset state
//}
//
//unsigned char uart_read_char(void){ // returns byte or NULL
//
//    unsigned char temp;
//    if( (FLAGS & RXFLAG) == 0) { // return NULL if no byte
//        return 0;
//    }
//                                // temp variable into receive buffer
//    temp = RXBUFFER;            // else, copy byte which clears flag
//    return temp;                // return value
//}
//
//void uart_write_char(unsigned char ch){
//    while ( (FLAGS & TXFLAG)==0 ) {} // Wait for any transmission to finish
//    TXBUFFER = ch; // copy byte to transmit buffer | Tx flag --> 0 and Tx starts
//    return;
//}
//
//void delay(){
//    // delay allows things like LED blinking to have enough time to be processed
//    volatile unsigned int i;
//    for(i=0; i<50000; i++){}
//}
//
//void uart_write_uint16(unsigned int n) {
//    char digits [6];            // 16 bit num can be up to 5 digits and need a null spot
//    unsigned char i = 0;        // digit index tracker
//
//    if (n == 0) {
//        uart_write_char('0');   // transmit 0 if n is 0; no other digits so break
//        return;
//    }
//    while(n != 0) {
//        i++;
//        digits[i]  = (n%10) + '0';  // converts digit to ASCII
//        n=n/10;
//    }
//    while (i > 0) {
//        uart_write_char(digits[i]);
//        i--;
//    }
//}
//
//void uart_write_string(char * str){
//    while(*str != '\0'){
//        uart_write_char(*str++);
//    }
//}

// ------------------------------------------------------------------------------------

//// 6.3 - modifying UART
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
//void uart_write_char(unsigned char ch);
//void delay();
//unsigned char uart_read_char(void);
//void Initialize_UART();
//
//void uart_write_uint16(unsigned int n);
//void uart_write_string(char* str);
//
//void config_ACLK_to_32KHz_crystal(void);
//void Initialize_UART_2();
//
//
//void main(void) {
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
//    PM5CTL0 &= ~LOCKLPM5;       // enable gen purpose input output (GPIO) pins
//
//    Initialize_UART_2();
//    config_ACLK_to_32KHz_crystal();
//
//    P1DIR |= red;           // configure red led
//    P9DIR |= green;         // configure green led
//
//    P1OUT &= ~red;          // red LED set to off
//    P9OUT &= ~green;        // green LED set to off
//
//    int data;
//    int i;
//
//    while(1){
//
//        for(i='0'; i<='9'; i++){
//
//            data = uart_read_char();
//            uart_write_uint16(4567);            // test with fewer digits
//            uart_write_string(" UART LAB!");    // Test uart string
//            uart_write_char(i);                 // write the index
//            uart_write_char('\n');              // transmit new line character
//            uart_write_char('\r');              // carriage return character
//            P1OUT ^= red;                       // toggle LED
//            delay();
//
//        }
//    }
//
//}
//
//// Configure UART to the popular configuration
//// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
//// no flow control, over sampling reception
//// Clock: SMCLK @ 1 MHz (1,000,000 Hz)
//void Initialize_UART(void){
//    // Configure pins to back channel UART
//    P3SEL1 &=  ~(BIT4|BIT5);
//    P3SEL0 |= (BIT4|BIT5);
//
//    // Main configuration register
//    UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
//                         // Most fields in this register, when set to zero, correspond to the
//                         // popular configuration
//
//    UCA1CTLW0 |= UCSSEL_2; // Set clock to SMCLK
//
//                 // Configure the clock dividers and modulators (and enable oversampling)
//    UCA1BRW = 6; // divider, given in lab -- can be found in family user's guide
//
//    // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
//    // UCBRS = 0x20 = 0010 0000 = UCBRS5 (bit #5)
//    UCA1MCTLW = UCBRF3 | UCBRS5 | UCOS16;
//
//    UCA1CTLW0 &= ~UCSWRST;  // Exit the reset state
//}
//
//void Initialize_UART_2(void){
//    // Configure pins to back channel UART
//    P3SEL1 &=  ~(BIT4|BIT5);
//    P3SEL0 |= (BIT4|BIT5);
//
//    // Main configuration register
//    UCA1CTLW0 = UCSWRST; // Engage reset; change all the fields to zero
//                         // Most fields in this register, when set to zero, correspond to the
//                         // popular configuration
//
//    UCA1CTLW0 |= UCSSEL_1; // Set clock to ACLK
//
//                 // Configure the clock dividers and modulators (and enable oversampling)
//    UCA1BRW = 6; // divider, given in lab -- can be found in family user's guide
//
//    // Modulators: UCBRF = 8 = 1000 --> UCBRF3 (bit #3)
//    // UCBRS = 0x20 = 0010 0000 = UCBRS5 (bit #5)
//    UCA1MCTLW = UCBRS7|UCBRS6|UCBRS5|UCBRS3|UCBRS2|UCBRS1;
//
//    UCA1CTLW0 &= ~UCSWRST;  // Exit the reset state
//}
//
//unsigned char uart_read_char(void){ // returns byte or NULL
//
//    unsigned char temp;
//    if( (FLAGS & RXFLAG) == 0) { // return NULL if no byte
//        return 0;
//    }
//                                // temp variable into receive buffer
//    temp = RXBUFFER;            // else, copy byte which clears flag
//    return temp;                // return value
//}
//
//void uart_write_char(unsigned char ch){
//    while ( (FLAGS & TXFLAG)==0 ) {} // Wait for any transmission to finish
//    TXBUFFER = ch; // copy byte to transmit buffer | Tx flag --> 0 and Tx starts
//    return;
//}
//
//void delay(){
//    // delay allows things like LED blinking to have enough time to be processed
//    volatile unsigned int i;
//    for(i=0; i<50000; i++){}
//}
//
//void uart_write_uint16(unsigned int n) {
//    char digits [6];            // 16 bit num can be up to 5 digits and need a null spot
//    unsigned char i = 0;        // digit index tracker
//
//    if (n == 0) {
//        uart_write_char('0');   // transmit 0 if n is 0; no other digits so break
//        return;
//    }
//    while(n != 0) {
//        i++;
//        digits[i]  = (n%10) + '0';  // converts digit to ASCII
//        n=n/10;
//    }
//    while (i > 0) {
//        uart_write_char(digits[i]);
//        i--;
//    }
//}
//
//void uart_write_string(char * str){
//    while(*str != '\0'){
//        uart_write_char(*str++);
//    }
//}
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
