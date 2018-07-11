// SPI master 

#include<SPI.h>

void setup()
{
  pinMode(SS, OUTPUT);        // pin of Slave Select in output
  digitalWrite(SS, HIGH);      
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);  // make the master slow down a bit

}


void loop()
{
  digitalWrite(SS, LOW);    // the master is going to send data
  SPI.transfer('f');        // sending one byte to the slave
  SPI.transfer('\n');
  digitalWrite(SS, HIGH);   // select the slave to read the data sent by the master
}
