// SPI Slave

#include<SPI.h>

char buf[5];
volatile byte pos = 0;
volatile boolean process_it = false; 


void setup()
{
  Serial.begin(9600);
  Serial.println("SPI test");
  pinMode(MISO, OUTPUT);
  digitalWrite(MISO, LOW);
  SPCR |= _BV(SPE);     //SPI Enable
  SPCR |= _BV(SPIE);    //SPI.attachInterrupt();
}


ISR(SPI_STC_vect)   // SPI interrupt routine
{
  byte c = SPDR;    // grab byte from SPI data register

  if(pos < sizeof buf)    // add the incoming byte c to the buffer in the next position
  {
    buf[pos++] = c;
    if(c == '\n')
     {
        process_it = true;
     }
  } 
}       // end of SPI interrupt routine

void loop()
{
    if(process_it)            // printing the buffer by the slave
      {
        Serial.println(buf);      
        pos = 0;
        process_it = false;
        delay (1000); 
}
}
