/**
 * blink and auto reset
 */

#include <msp430.h>


int main(void) {
  volatile int i;
  volatile int j;

  // stop watchdog timer
  WDTCTL = WDTPW | WDTHOLD;
  // set up bit 0 of P1 as output
  P1DIR = 0x01;
  // intialize bit 0 of P1 to 0
  P1OUT = 0x00;

  // loop of blinking
  for (j=0; j<10;j++) {
    // toggle bit 0 of P1
    P1OUT ^= 0x01;

    // delay for a while
    for (i = 0; i < 0x6000; i++);
  }

  for (i = 0; i < 30000 ; i++);     // Delay to reset

  WDTCTL = 0;                   // Forces system reset
}
