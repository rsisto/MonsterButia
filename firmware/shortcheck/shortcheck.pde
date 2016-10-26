/*
  check PCB is ok
 */

#include <inttypes.h>

#define NUMBER_DIGITAL_PINS 54
#define NUMBER_ANALOG_PINS 16
#define NUMBER_PINS (NUMBER_DIGITAL_PINS + NUMBER_ANALOG_PINS)
// 0 y 1 son rx tx
#define INIT_PIN 2
#define BIGNUMBER 20000
#define FINISH -1

#define NOTCHECK 255

void setup() { 
  Serial.begin(115200);
  int i;
  for (i=INIT_PIN; i < NUMBER_PINS; ++i){
    pinMode(i, INPUT);     
    digitalWrite(i, LOW); //not pullup
  };
}




void dontcheck(uint8_t pin) {


}


void init(uint8_t * nocheck){
}


int check(){


  digitalWrite(13, HIGH);   // set the LED on
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(1000);              // wait for a second


} 

class Shortcheck {
  private:
  byte pins[NUMBER_PINS];

  public:
  Shortcheck(){
    for (int i=0; i < NUMBER_PINS ; ++i) pins[i] = i;
  };
  
  void notcheckpin(uint8_t pin){
    if (pin > NUMBER_PINS) return;
//    if pins[pin]=
    pins[pin] = NOTCHECK;
  };
  
  void checkpin(uint8_t pin){
    if (pin > NUMBER_PINS) return;
    pins[pin] = pin;
  };

  void addshort(uint8_t a, uint8_t b){
    uint8_t aux;
    if (b > NUMBER_PINS) return;
    if (a > NUMBER_PINS) return;
    if (a > NUMBER_PINS) return;
    if (a > NUMBER_PINS) return;
    if (a > b) { aux=a ; a=b ; b=aux ; };
    
 //   if pins[a] = 
    pins[a] = b;
  
  }
  
  uint16_t check(){
  
    return 0;
  };
};







void loop() {

  
  Serial.print("Waiting!!!, pres any key\n");
  while ( ! Serial.available() )  ; //wait
  while ( Serial.read() != -1) ; // flush

  int i,j,aux;

/* this check is not usefull
  Serial.print("### Checkfloating!!!\n");
  //floating check
  for (i=INIT_PIN; i < NUMBER_PINS; ++i){
    pinMode(i, INPUT);     
    digitalWrite(i, LOW); //not pullup

    aux = digitalRead(i);
    for (j=0 ; (j < BIGNUMBER) && (aux != FINISH) ; ++j) 
      if (digitalRead(i) != aux) aux = FINISH;
    if (aux != FINISH) {
      Serial.print("WARN: pin ");
      Serial.print(i, DEC);
      Serial.print(" not flouting\n"); 
    }
  }
*/

  Serial.print("### CheckPullup!!!\n");
  for (i=INIT_PIN; i < NUMBER_PINS; ++i) {
    pinMode(i, INPUT);     
    digitalWrite(i,1); //pullup
    if (digitalRead(i) == 0) { 
      Serial.print("ERR: pin ");
      Serial.print(i, DEC);
      Serial.print(" not work pullup\n");
    };
  };
  
  //shortcheck
  Serial.print("### CheckShortcheck!!!\n");
  for (i=INIT_PIN; i < NUMBER_PINS; ++i) {
    digitalWrite(i,0);  //not pullup
    pinMode(i, OUTPUT);
    digitalWrite(i,0);
    for (j=i+1 ; j < NUMBER_PINS ; ++j) {
      if (digitalRead(j) == 0){
        Serial.print("ERR: pin ");
        Serial.print(i, DEC);
        Serial.print(" is in shortcut whit ");
        Serial.print(j, DEC);
        Serial.print("\n");
      }
    }
    pinMode(i, INPUT);
    digitalWrite(i,1); //pullup
  }

  
//  Serial.print("### CheckSoulding!!!\n");
  // soulding check Already done in check pull-up
  // using conector that is a pull-down

  Serial.print("FINISH!!!!");
  
}


