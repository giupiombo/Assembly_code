// Author: Giulia Piombo
// Date: 4/1/2021
// Description: Count display using MSP430 board and 7-segment display.
//              Play/pause and reset functionalities.

#include "io430.h"

void init(void);
void delay(void);

unsigned char startPressed = 0;
unsigned char resetPressed = 0;

#define ALLD 0x3C
#define DIS2 0x20               // Display 2
#define DIS3 0x10               // Display 3
#define DIS4 0x08               // Display 4
#define DIS5 0x04               // Display 5

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{ 
  if(P1IFG&BIT0) {
      P1IFG&=~BIT0; // Clear Bit0 Interrupt Flag
  } else if(P1IFG&BIT1) {
      P1IFG&=~BIT1; // Clear Bit1 Interrupt Flag
  } else if(P1IFG&BIT2) {
      P1IFG&=~BIT2; // Clear Bit2 Interrupt Flag
  } else if(P1IFG&BIT3) {
      P1IFG&=~BIT3; // Clear Bit3 Interrupt Flag
  } else if(P1IFG&BIT4) {
      P1IFG&=~BIT4; // Clear Bit4 Interrupt Flag
  } else if(P1IFG&BIT5) {
      P1IFG&=~BIT5; // Clear Bit5 Interrupt Flag
  } else if(P1IFG&BIT6) {               
      if (startPressed == 0) {          // Decide if play/pause button was pressed
        startPressed = 1;
      } else {
        startPressed = 0;
      }
      P1IFG&=~BIT6; // Clear Bit6 Interrupt Flag
  } else if(P1IFG&BIT7) {
      if (startPressed != 1) {          // If it is paused, reset if button is pressed
        resetPressed = 1;
      }
      P1IFG&=~BIT7; // Clear Bit7 Interrupt Flag
  }
}

void main( void )
{ 
  // Array of numbers to be displayed on the 7-segment
  unsigned char numbers[11] = {~0x3F, ~0x06, ~0x5B, ~0x4F, ~0x66, ~0x6D,
                                ~0x7D, ~0x07, ~0x7F, ~0x67, ~0x3F}; 
  unsigned int count = 0;
  unsigned int i = 0;
  unsigned int tens = 0;
  unsigned int hundreds = 0;
  unsigned int thousands = 0;
  
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  init();
  __enable_interrupt();
  
  // Turning off all displays
  P1OUT &= ~(ALLD);
  
  P2OUT = ~0x3F;
  
  while (1) {
    // Reset functionality
    if (resetPressed == 1) {
       i = 0;
       tens = 0;
       hundreds = 0;
       thousands = 0;
       
       P1OUT &= ~(ALLD);
       P1OUT |= DIS5;
       P2OUT = numbers[i];
       
       P1OUT |= DIS4;
       P2OUT = numbers[tens];
       
       P1OUT |= DIS3;
       P2OUT = numbers[hundreds];

       P1OUT |= DIS2;
       P2OUT = numbers[thousands];
       
       resetPressed = 0;
    }
    
    // Start counting and displaying
    while (startPressed == 1) {
      
      // Reset if count reaches 9999
      if (count == 10000) {
        i = 0;
        tens = 0;
        hundreds = 0;
        thousands = 0;
        count = 0;
      }

      if (i == 10 && tens < 10) {
        i = 0;
        tens++;
      } else if (tens == 10 && hundreds < 10) {
        if (count == 10) {
            tens = 1;
        } else {
          tens = 0;
        }
        hundreds++;
      } else if (hundreds == 10 && thousands < 10) {
        if (count == 100) {
            hundreds = 1;
        } else {
          hundreds = 0;
        }
        thousands++;
      }
      
      P1OUT &= ~(ALLD);
      P1OUT |= DIS5;
      P2OUT = numbers[i];
      delay();
      
      P1OUT &= ~(ALLD);
      P1OUT |= DIS4;
      P2OUT = numbers[tens];
      delay();
      
      P1OUT &= ~(ALLD);
      P1OUT |= DIS3;
      P2OUT = numbers[hundreds];
      delay();
      
      P1OUT &= ~(ALLD);
      P1OUT |= DIS2;
      P2OUT = numbers[thousands];
      delay();
    
      count++;
      i++;
    }
  }
}

// Configuration of the MSP430 board
void init() {
  
  P2DIR = 0xFF;
  
  P2SEL = 0x00;
  P2SEL2 = 0x00;
    
  P1DIR = 60;
  
  P1SEL = 0x00;
  P1SEL2 = 0x00;
  
  // Clear any interrupt flags before enabling interrupts
  P1IFG &= (BIT7|BIT6);
        
  // Enable interrupts for bit 7 and bit 6 
  P1IE = (BIT7|BIT6);
  P1IES |= (BIT7|BIT6);
}

void delay(){
  int i,j;
  for(i=0;i<150;i++)
    for(j=0;j<5;j++);
}

