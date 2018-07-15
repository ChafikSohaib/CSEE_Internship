// SPI Communication

#include <msp430.h>

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                       // Stop watchdog timer
    P3DIR |= BIT0;                                  // Set 3.0 as Output (Latch)

    P3SEL = BIT4 + BIT0;                            // Select P3.4 -> SIMO (Data)
    P3SEL2 = BIT4 + BIT0;                           // Select P3.0 -> CLK (Clock)

    UCA0CTL1 |= UCSWRST;                            // Hold USCI in reset state
    UCA0CTL0 |= UCCKPL + UCMST + UCSYNC;            // 3-pin, 8-bit, SPI Master
    UCA0CTL1 |= UCSSEL_2;                           // Clock -> SMCLK
    UCA0BR0 = 0x02;                                 // SPI CLK -> SMCLK/2
    UCA0CTL1 &= ~UCSWRST;                           // Initialise USCI module

    while(1)
    {
        P3OUT &= ~BIT0;                             // Set Latch to LOW
        while (!(IFG2 & UCA0TXIFG));                // Check if TX Buffer is empty
        UCA0TXBUF = 0xAA;                           // Transmit first pattern
        while ((UCA0STAT & UCBUSY));                // Wait till TX Completes
        P3OUT |= BIT3;                              // Set Latch to HIGH
        __delay_cycles(500000);                     // Delay 500 ms

        P3OUT &= ~BIT3;                             // Set Latch to LOW
        while (!(IFG2 & UCA0TXIFG));                // Check if TX Buffer is empty
        UCA0TXBUF = 0x55;                           // Transmit second pattern
        while ((UCA0STAT & UCBUSY));                // Wait till TX Completes
        P3OUT |= BIT3;                              // Set Latch to HIGH
        __delay_cycles(500000);                     // Delay 500 ms
    }
}
