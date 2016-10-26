/*
  Analog input, analog output, serial output
 
 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 by Tom Igoe
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = 0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  
}

#define PINRC 40

void loop() {
  // read the analog in value:
  pinMode (PINRC, OUTPUT);
  digitalWrite (PINRC, 0);
  delayMicroseconds (10);
  
  unsigned int contador = 1;
  pinMode (PINRC, INPUT);
  while ( contador && ( digitalRead(PINRC) == 0) ) { ++contador ;} 
   
          

  // print the results to the serial monitor:
  if (contador) {
    Serial.print("\t output = ");      
    Serial.println(contador, DEC);   
  } else {
    Serial.println("\t output = NC");
  } 
  
  
  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(500);                     
}
