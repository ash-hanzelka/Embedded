// 5.1 - printing on the LCD display
#include <msp430fr6989.h>
#define red BIT0                // red 1.0
#define green BIT7              // green 9.7
void Initialize_LCD();

// The array holds shapes of digits 0-9
// see notes/lab report to see how the values were calculated

const unsigned char LCD_Shapes[10] = {0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBF, 0xE0, 0xFE, 0xF6};

int main(void)
{

    volatile unsigned int n;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~ LOCKLPM5;      // enable gen purpose input output (GPIO) pins

	P1DIR |= red;               // configure pins as output
	P9DIR |= green;
	P1OUT |= red;               // set red - on
	P9OUT &= ~green;            // clear green - off

	Intialize_LCD();

	// LCDCMEMCTL = LCDCLRM;    // clears all the segments
	
	// display 430 on the right most 3 digits:
	LCDM19 = LCD_Shapes[4];
	LCDM15 - LCD_Shapes[3];
	LCDM8 = LCD_Shapes[0];

	// flash the red LED
	for (;;) {
	    for(n=0; n <= 60000; n++) {
	        P1OUT ^= red;
	    }
	}
	return 0;

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
