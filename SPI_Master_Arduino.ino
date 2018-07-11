#include<SPI.h>

void setup()
{
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);  //2^n = 2, 4, 8, 16, 32

}


void loop()
{
  digitalWrite(SS, LOW);
  SPI.transfer('f');
  SPI.transfer('\n');
  digitalWrite(SS, HIGH);
}
