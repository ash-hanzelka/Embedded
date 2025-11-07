//// 5.1 - printing on the LCD display
//#include <msp430fr6989.h>
//#define red BIT0                // red 1.0
//#define green BIT7              // green 9.7
// void Initialize_LCD();
// void lcd_write_uint16(unsigned int n);
//
//// The array holds shapes of digits 0-9
//// see notes/lab report to see how the values were calculated
//
//const unsigned char LCD_Shapes[10] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBF, 0xE0, 0xFE, 0xF6};
//
////const unsigned char felix[5] = {0x8E, 0x9E, 0x1C, 0x60, 0x6E};
//
////0110 1110
//
//int main(void)
//{
//
//    volatile unsigned int n;
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
//    PM5CTL0 &= ~ LOCKLPM5;      // enable gen purpose input output (GPIO) pins
//
//    P1DIR |= red;               // configure pins as output
//    P9DIR |= green;
//    P1OUT |= red;               // set red - on
//    P9OUT &= ~green;            // clear green - off
//
//    Initialize_LCD();
//
//    // LCDCMEMCTL = LCDCLRM;    // clears all the segments
//
//    // display 430 on the right most 3 digits:
////    LCDM19 = LCD_Shapes[3];
////    LCDM15 = LCD_Shapes[4];
////    LCDM8 = LCD_Shapes[5];
////
////    // figuring out the
////    LCDM4 = LCD_Shapes[2];
////    LCDM10 = LCD_Shapes[0];
////    LCDM6 = LCD_Shapes[1];
//
//    // my boyfriend's name (:
////    LCDM10 = felix[0];
////    LCDM6 = felix[1];
////    LCDM4 = felix[2];
////    LCDM19 = felix[3];
////    LCDM15 = felix[4];
//
//        lcd_write_uint16(65535);
////        delay(1000);
////        lcd_write_uint16(310);
//
//    // flash the red LED
//    for (;;) {
//        for(n=0; n <= 60000; n++) {
//            P1OUT ^= red;
//        }
//    }
//    // return 0;
//
//}
//
//void lcd_write_uint16 (unsigned int n) {        // ex. 65535
//    unsigned int tenThouPlace = n/10000;        // 65535 / 10000 = 6
//    unsigned int thouPlace = (n/1000)%10;       // 65535 / 1000 = 65 % 10 = 5
//    unsigned int hundPlace = (n/100)%10;        // 65535 / 100 = 655 % 10 = 5
//    unsigned int tensPlace = (n/10)%10;         // 65535 / 10 = 6553 % 10 = 3
//    unsigned int onesPlace = n%10;              // 65535 % 10 = 5
//
//
//
//    if (tenThouPlace != 0) {
//        LCDM6 = LCD_Shapes[tenThouPlace];
//    }
//    if(thouPlace != 0) {
//        LCDM4 = LCD_Shapes[thouPlace];
//    }
//    if(hundPlace != 0) {
//        LCDM19 = LCD_Shapes[hundPlace];
//    }
//    if(tensPlace != 0) {
//        LCDM15 = LCD_Shapes[tensPlace];
//    }
//    LCDM8 = LCD_Shapes[onesPlace];
//
//    // found in notes:
////    unsigned char* ptr &= LCDM0;
////    int digit;
////    int i = 0;
////
////    do {
////        digit = n%10;
////        ptr[i] = LCD_Shapes[digit];
////        i++;
////        n = (n/10);
////    } while (n > 0);
////
////    // clear remaining bits
////    while (i < 8) {
////        LCDptr[i] = 0;
////        i++;
////    }
////    return;
// }
//
////int isZero (unsigned int n) {
////    if (n == 0) {
////        return 1;
////    }
////    return 0;
////}
//
//void Initialize_LCD() {         // function given in lab manual, obtained from MSP sample code
//    PJSEL0 = BIT4 | BIT5;       // for LFXT (crystal)
//
//    //Initialize LCD segments 0-21; 26-43
//    LCDCPCTL0 = 0xFFFF;
//    LCDCPCTL1 = 0xFC3F;
//    LCDCPCTL2 = 0x0FFF;
//
//    // configure LFXT 32Khz crystal
//    CSCTL0_H = CSKEY >> 8;      // unlock CS registers
//    CSCTL4 &= ~LFXTOFF;         // enable LFXT
//    do {
//        CSCTL5 &= ~LFXTOFFG;    // clear LFXT fault flag
//        SFRIFG1 &= ~OFIFG;
//    } while (SFRIFG1 & OFIFG);  // test oscillator fault flag
//
//    CSCTL0_H = 0;
//
//    // Initialize LCD _C
//    // ACLK; divider = 1, pre divider = 16; 4 pin MUX
//    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;
//
//    // VLCD generated internally
//    // V2 - V4 generated internally, v5 to ground
//    // set VLCD voltage to 2.6 V
//    // enable charge pump and select internal reference for it
//    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;
//
//    LCDCCPCTL = LCDCPCLKSYNC;   // enable clock synchronization
//
//    LCDCMEMCTL = LCDCLRM;       // clear LCD memory
//
//    LCDCCTL0 |= LCDON;          //turn on LCD
//
//    return;
//}

// ----------------------------------------------------------------------------------------------------------

 // 5.2 - counter
 #include <msp430fr6989.h>
 #define red BIT0                // red 1.0
 #define green BIT7              // green 9.7
 #define b1 BIT1                 // button 1 by the red LED
 #define b2 BIT2                 // button 2 by the green led
 void Initialize_LCD();
 void lcd_write_uint16(unsigned int n);
 volatile unsigned int counter = 0;

// The array holds shapes of digits 0-9
// see notes/lab report to see how the values were calculated

const unsigned char LCD_Shapes[10] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBF, 0xE0, 0xFE, 0xF6};

int main(void)
{

    volatile unsigned int n;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~ LOCKLPM5;      // enable gen purpose input output (GPIO) pins

    P1DIR |= red;               // configure pins as output
    P9DIR |= green;
    P1OUT |= red;               // set red - on
    P9OUT &= ~green;            // clear green - off

    // configure buttons and initialize interrupts for them
    P1DIR &= ~(b1 | b2);
    P1REN |= (b1 | b2);
    P1OUT |= (b1 | b2);
    P1IES &= ~(b1 | b2);                // clear - 0 for rising edge
    P1IFG &= ~(b1 | b2);
    P1IE |= (b1 | b2);

    config_ACLK_to_32KHz_crystal();

    TA0CCR0 = 32767;                    // 1 second @32 KHz - 1 second would be half of the 2 seconds made by 65535 cycles
    TA0CCTL0 = CCIE;                    // enable channel 0 ccie bit
    TA0CCTL0 &= ~CCIFG;                 // clear channel 0 ccifg bit

    TA0CTL = TASSEL_1 |     // sets to ACLK
                 ID_0 |         // divides by 1
                 MC_1 |         // sets to up mode
                 TACLR;         // Timer_A clear, sets TAR to 0

    TA0CTL &= ~TAIFG;       // ensures flag is clear at start

    _enable_interrupts();


    Initialize_LCD();

        // lcd_write_uint16(65535);

    // flash the red LED
    for (;;) {
        for(n=0; n <= 60000; n++) {
            P1OUT ^= red;
            _delay_cycles(1000);    // blinking not noticeable, add delay
        }
    }
    // return 0;

}

#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
    // Interrupt Response       // check interrupt
        counter++;
        // handling the counter exceeding 16 bit, reset back to 0
        // no instructions given so I decided to implement this
        if (counter > 65535) {
            counter = 0;
        }
        lcd_write_uint16(counter);
}

#pragma vector = PORT1_VECTOR // Link the ISR to the vector
__interrupt void Port1_ISR() {

    if((P1IFG & b1) == b1 ){
        counter = 0;
        P1IFG &= ~b1;
    }

    if((P1IFG & b2) == b2 ) {
        counter += 1000;
        // handling the counter exceeding 16 bit, reset back to 0
        // no instructions given so I decided to implement this
        if (counter > 65535) {
            counter = 0;
        }
        P1IFG &= ~b2;
    }

    lcd_write_uint16(counter);

}


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

void lcd_write_uint16 (unsigned int n) {        // ex. 65535

    unsigned int tenThouPlace = n/10000;        // 65535 / 10000 = 6
    unsigned int thouPlace = (n/1000)%10;       // 65535 / 1000 = 65 % 10 = 5
    unsigned int hundPlace = (n/100)%10;        // 65535 / 100 = 655 % 10 = 5
    unsigned int tensPlace = (n/10)%10;         // 65535 / 10 = 6553 % 10 = 3
    unsigned int onesPlace = n%10;              // 65535 % 10 = 5

    // clear the previous segments
    LCDM6 = 0;
    LCDM4 = 0;
    LCDM19 = 0;
    LCDM15 = 0;
    LCDM8 = 0;


    // prevent display from showing 1    1 instead of 10001 or resetting to 1   0
    if (tenThouPlace != 0) {
        LCDM6 = LCD_Shapes[tenThouPlace];
    }
    if(tenThouPlace != 0 || thouPlace != 0) {
        LCDM4 = LCD_Shapes[thouPlace];
    }
    if(tenThouPlace != 0 || thouPlace != 0 || hundPlace != 0) {
        LCDM19 = LCD_Shapes[hundPlace];
    }
    if(tenThouPlace != 0 || thouPlace != 0 || hundPlace != 0 || tensPlace != 0) {
        LCDM15 = LCD_Shapes[tensPlace];
    }
    LCDM8 = LCD_Shapes[onesPlace];
}


void Initialize_LCD() {         // function given in lab manual, obtained from MSP sample code
    PJSEL0 = BIT4 | BIT5;       // for LFXT (crystal)

    //Initialize LCD segments 0-21; 26-43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // configure LFXT 32Khz crystal
    CSCTL0_H = CSKEY >> 8;      // unlock CS registers
    CSCTL4 &= ~LFXTOFF;         // enable LFXT
    do {
        CSCTL5 &= ~LFXTOFFG;    // clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);  // test oscillator fault flag

    CSCTL0_H = 0;

    // Initialize LCD _C
    // ACLK; divider = 1, pre divider = 16; 4 pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally
    // V2 - V4 generated internally, v5 to ground
    // set VLCD voltage to 2.6 V
    // enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC;   // enable clock synchronization

    LCDCMEMCTL = LCDCLRM;       // clear LCD memory

    LCDCCTL0 |= LCDON;          //turn on LCD

    return;
}

