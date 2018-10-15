#include  <msp430.h>

void delay()
{
    P1OUT ^= BIT0;

    volatile unsigned long i;
    i = 49999;
    do (i--);
    while (i != 0);
}

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;

    P1DIR |= BIT0;                          // Internal LEDs P1.0 of Launchpad is output

    P1DIR |= BIT2;                          // P1.2/TA0.1 is used for PWM, thus also an output -> servo 1

    P1OUT = 0;                              // Clear all outputs P1

    P1SEL |= BIT2;                          // P1.2 select TA0.1 option


    // if SMCLK is about 1MHz (or 1000000Hz),
    // and 1000ms are the equivalent of 1 Hz,
    // then, by setting CCR0 to 20000 (1000000 / 1000 * 20)
    // we get a period of 20ms
    TA0CCR0 = 20000-1;                           // PWM Period TA0.1


    // setting 1500 is 1.5ms is 0deg. servo pos
    TA0CCR1 = 1500;                            // CCR1 PWM duty cycle


    TA0CCTL1 = OUTMOD_7;                       // CCR1 reset/set
    TA0CTL   = TASSEL_2 + MC_1;                // SMCLK, up mode


    // loop just blinks build in LEDs to show activity
    //for (;;)
    //{
        delay();
        TA0CCR1 = 1000;

        delay();
        TA0CCR1 = 1500;

        delay();
        TA0CCR1 = 2000;

        delay();
        TA0CCR1 = 2500;
    //}
}
