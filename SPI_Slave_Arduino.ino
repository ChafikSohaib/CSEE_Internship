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
  SPCR |= _BV(SPE); //SPI Enable
  SPCR |= _BV(SPIE);
  //SPI.attachInterrupt();
}


ISR(SPI_STC_vect)
{
  byte c = SPDR;

  if(pos < sizeof buf)
  {
    buf[pos++] = c;
    if(c == '\n')
     {
        process_it = true;
}
} 
}

void loop()
{
    if(process_it)
      {
        Serial.println(buf);
        pos = 0;
        process_it = false;
        delay (1000); 
}
}
