// SPI Communication

// MASTER :

#include <msp430g2553.h>

long i=0;
unsigned char MST_Data, SLV_Data;
int received_ch;

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer

    P1DIR |= BIT0;
    P1DIR |= BIT5;
    P1SEL &= ~0x08;                     // Select Port 1 P1.3 (push button)
    P1DIR &= ~0x08;                     // Port 1 P1.3 (push button) as input, 0 is input
    P1REN |= 0x08;                      // Enable Port P1.3 (push button) pull-up resistor

    P1IE |= 0x08;                       // Port 1 Interrupt Enable P1.3 (push button)

    P1IFG &= ~0x08;                     // Clear interrupt flag

    P1SEL  =   BIT1    |   BIT2    |   BIT4;
    P1SEL2 =   BIT1    |   BIT2    |   BIT4;
    MST_Data = 0x01;                          // Initialize data values
    SLV_Data = 0x00;


    UCA0CTL1   =   UCSWRST;
    UCA0CTL0   |=  UCCKPH  +   UCMSB   +   UCMST   +   UCSYNC;          //  3-pin,  8-bit   SPI master
    UCA0CTL1   |=  UCSSEL_2;            //  SMCLK
    UCA0BR0    |=  0x02;   
    UCA0BR1    =   0; 
    UCA0MCTL   =   0;                   //  No  modulation
    UCA0CTL1   &=  ~UCSWRST;            //  **Initialize    USCI    state   machine**


    _BIS_SR(GIE);                       // Enable interrupts




    MST_Data++;                               // Increment master value
    SLV_Data++;                               // Increment expected slave value

    P1IFG &= ~0x08;                           // P1.3 Interrupt Flag cleared
    P1OUT ^= BIT0;

//  P1OUT  &=  (~BIT5);     //  Select  Device
//  P1OUT  &=  (~BIT5);     //  Select  Device
    P1OUT  |=  (BIT5);      //  Unselect    Device


    while  (!(IFG2 &   UCA0TXIFG));     //  USCI_A0 TX  buffer  ready?
    UCA0TXBUF  =   MST_Data;            //  Send    0xAA    over    SPI to  Slave

//  P1OUT  |=  (BIT5);      //  Unselect    Device
    P1OUT  &=  (~BIT5);     //  Select  Device
//  P1OUT  |=  (BIT5);      //  Unselect    Device


    i = 0;                          // Reset count

}



// SLAVE :


#include <msp430g2553.h>

long i=0;
unsigned char MST_Data, SLV_Data;
int received_ch;

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;           // Stop watchdog timer

    P1DIR |= BIT0;
    P1SEL &= ~BIT5;                     // Select Port 1 P1.3 (push button)
    P1DIR &= ~BIT5;                    // Port 1 P1.3 (push button) as input, 0 is input
    P1REN |= BIT5;                      // Enable Port P1.3 (push button) pull-up resistor

    P1IE |= BIT5;                       // Port 1 Interrupt Enable P1.3 (push button)

    P1IFG &= ~BIT5;                     // Clear interrupt flag

    P1SEL  =   BIT1    |   BIT2    |   BIT4;
    P1SEL2 =   BIT1    |   BIT2    |   BIT4;

    MST_Data = 0x01;                          // Initialize data values
    SLV_Data = 0x00;


    UCA0CTL1   =   UCSWRST;
    UCA0CTL0   |=  UCCKPH  +   UCMSB   +   UCSYNC;  //  3-pin,  8-bit   SPI master
    UCA0CTL1   |=  UCSSEL_2;                        //  SMCLK
    UCA0BR0    |=  0x02;
    UCA0BR1    =   0;
    UCA0MCTL   =   0;                               //  No  modulation
    UCA0CTL1   &=  ~UCSWRST;                        //  **Initialize    USCI    state   machine**


    _BIS_SR(GIE);                               // Enable interrupts

    MST_Data++;                                 // Increment master value
    SLV_Data++;                                 // Increment expected slave value

    P1IFG &= ~BIT5;                             // P1.3 Interrupt Flag cleared
    P1OUT ^= BIT0;

    //  P1OUT  &=  (~BIT5);    //  Select  Device
    //  P1OUT  |=  (BIT5); //  Unselect    Device
    //__delay_cycles(500);

    while  (!(IFG2 &   UCA0RXIFG));     //  USCI_A0 RX  Received?
    received_ch    =   UCA0RXBUF;       //  Store   received    data
    while  (!(IFG2 &   UCA0TXIFG));     //  USCI_A0 TX  buffer  ready?
    UCA0TXBUF  =   0x14;                //  Send    0xAA    over    SPI to  Slave

    //      P1OUT  |=  (BIT5);      //  Unselect    Device
    //      P1OUT  &=  (~BIT5);     //  Select  Device


    i = 0;                              // Reset count


}
