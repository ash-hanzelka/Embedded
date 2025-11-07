/*
EEL 4742C - UCF
Code that prints the UCF logo and tests the grahpics library
*/
#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h" // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h" // LCD driver
#include <stdio.h>
#define redLED BIT0
#define greenLED BIT7
#define BUT1 BIT1
#define BUT2 BIT2
Graphics_Context g_sContext; // Declare a graphic library context
void config_ACLK_to_32KHz_crystal(void);
void Initialize_Clock_System();
int state = 0; //variable to keep track of which screen we are on
//define image object
extern const tImage UCF_Logo;
// ****************************************************************************
void main(void) {
 WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
 PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins
 P1DIR |= redLED; // Configure pin as output
 P9DIR |= greenLED; // Configure pin as output
 // Configure the buttons for interrupts
 P1DIR &= ~(BUT1|BUT2); // 0: input
 P1REN |= (BUT1|BUT2); // 1: enable built-in resistors
 P1OUT |= (BUT1|BUT2); // 1: built-in resistor is pulled up to Vcc
 P1IES |= (BUT1|BUT2); // 1: interrupt on falling edge (0 for rising edge)
 P1IFG &= ~(BUT1|BUT2); // 0: clear the interrupt flags
 P1IE |= (BUT1|BUT2); // 1: enable the interrupts
 P1OUT &= ~redLED; // Turn LED Off
 P9OUT |= greenLED; // Turn LED ON FOR ALTERNATING
 // Set the LCD backlight to highest level
 //P2DIR |= BIT6;
 //P2OUT |= BIT6;
 // Configure clock system
 Initialize_Clock_System();
 // Graphics functions
 Crystalfontz128x128_Init(); // Initialize the display
 // Set the screen orientation
 Crystalfontz128x128_SetOrientation(0);
 // Initialize the context
 Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
 // Set background and foreground colors
 Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
 // Set the default font for strings
 GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
 //Clear the screen
 Graphics_clearDisplay(&g_sContext);
 // Configure ACLK to the 32 KHz crystal
 config_ACLK_to_32KHz_crystal();
 // Configure Channel 0 for up mode with interrupts
 TA0CCR0 = 32768; // 1 second @ 32 KHz
 TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
 TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit
 // Timer_A: ACLK, div by 1, up mode, clear TAR
 TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
 // Enable the global interrupt bit (call an intrinsic function)
 _enable_interrupts();

/////////////////////////////////////////////////////////////////////////////////////
///////
 // Print UCF logo
 Graphics_drawImage(&g_sContext, &UCF_Logo, 0, 0);
 // Infinite loop... the code waits here between interrupts
 for(;;) {}
}
//******* Writing the ISR *******
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A1_ISR() {
 // Toggle the red and green LED
 P1OUT ^= redLED;
 P9OUT ^= greenLED;
}
//******* Writing the ISR *******
#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {

/////////////////////////////////////////////////////////////////////////////////////
///////
 //defining our rectangles
 Graphics_Rectangle rect1;
 rect1.xMin = 70;
 rect1.xMax = 90;
 rect1.yMin = 10;
 rect1.yMax = 20;
 Graphics_Rectangle rect2;
 rect2.xMin = 70;
 rect2.xMax = 90;
 rect2.yMin = 80;
 rect2.yMax = 90;
 int x; //our waiting variable to draw shapes

/////////////////////////////////////////////////////////////////////////////////////
///////
 if (P1IFG & BUT1) { // Detect button 1 interrupt flag
 //if state is 1, print the shape screen
 if(state == 0){
 // Set the background to orange
 Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_ORANGE);
 // Set foreground color to black
 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
 //Clear the screen to remove UCF_Logo
 Graphics_clearDisplay(&g_sContext);
 // Drawing a Rectangle
 Graphics_drawRectangle(&g_sContext, &rect1);
 for (x = 0; x < 32000; x++){}
 // Fill the Rectangle
 Graphics_fillRectangle(&g_sContext, &rect2);
 for (x = 0; x < 32000; x++){}
 //change forground color for circle
 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
 // Drawing a Circle
 Graphics_drawCircle(&g_sContext, 20, 20, 10);
 for (x = 0; x < 32000; x++){}
 // Filling the Circle
 Graphics_fillCircle(&g_sContext, 70, 70, 10);
 for (x = 0; x < 32000; x++){}
 //Change color and draw line
 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
 Graphics_drawLineH(&g_sContext, 10, 60, 80);
 for (x = 0; x < 32000; x++){}
 //Change forground back to black for text
 Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
 //Draw a string with font 1
 GrContextFontSet(&g_sContext, &g_sFontFixed6x8); //FIXED FONT
 Graphics_drawStringCentered(&g_sContext, "GO KNIGHTS",AUTO_STRING_LENGTH,
64,40,OPAQUE_TEXT);
 //Draw a string with font 2
 GrContextFontSet(&g_sContext, &g_sFontCmss12); //Cmss Font
 Graphics_drawStringCentered(&g_sContext, "CHARGE ON!",AUTO_STRING_LENGTH,
64,49,OPAQUE_TEXT);
 state = 1;
 }
 //if state = 1, print UCF logo
 else{
 //Clear the screen
 //Graphics_clearDisplay(&g_sContext);
 // Print UCF logo
 Graphics_drawImage(&g_sContext, &UCF_Logo, 0, 0);
 //Clear the screen
 //Graphics_clearDisplay(&g_sContext);
 state = 0; //update state var
 }
 __delay_cycles(300000); // delay for debouncing
 P1IFG &= ~BUT1; // Clear button 1 interrupt flag
 }
}
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
 // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
 // Reroute pins to LFXIN/LFXOUT functionality
 PJSEL1 &= ~BIT4;
 PJSEL0 |= BIT4;
 // Wait until the oscillator fault flags remain cleared
 CSCTL0 = CSKEY; // Unlock CS registers
 do {
 CSCTL5 &= ~LFXTOFFG; // Local fault flag
 SFRIFG1 &= ~OFIFG; // Global fault flag
 } while((CSCTL5 & LFXTOFFG) != 0);
 CSCTL0_H = 0; // Lock CS registers
 return;
}
// *****************************
void Initialize_Clock_System() {
 // DCO frequency = 8 MHz (default value)
 // MCLK = fDCO/2 = 4 MHz
 // SMCLK = fDCO/1 = 8 MHz
 CSCTL0 = CSKEY; // Unlock clock module config registers
 CSCTL3 &= ~(BIT2|BIT1|BIT0); // DIVM = 000
 CSCTL3 |= BIT0; // DIVM = 001 = /2
 CSCTL3 &= ~(BIT6|BIT5|BIT4); // DIVS = 000 = /1
 CSCTL0_H = 0; // Relock clock module config registers
 return;
}
