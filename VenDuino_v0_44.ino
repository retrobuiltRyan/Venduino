/*--------------------VenDuino v0.44---------------
Vending machine using FULL ROTATION servos. Rotary encoder is used to select item (replaces four buttons).
I/O           PIN#
ServoA        ~11
ServoB        ~10
ServoC         ~9
ServoD         ~6
LEDready       13
coinInsert     12
Encoder CLK   2
Encoder DT    3 
Encoder Switch  4
A0-A4 Nokia5110 LCD
(84x48 pixel screen) (x,y)

*/
int CLK = 2;//
int DT = 3;//
int encoderSwitch = 4;//
const int interrupt0 = 0;// Interrupt 0 = pin 2 
int count = 0;//Define the count
int lastCLK = 0;//CLK initial value
#include <Servo.h> 
#include <LCD5110_Graph.h>

LCD5110 myGLCD(A0,A1,A2,A3,A4);

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t TinyFont[];

int coinCounter = 0;   // counter for the number of button presses
int coinState = 0;         // current state of the button
int lastcoinState = 0;     // previous state of the button

const int ServoA = 11;
const int ServoB = 10;
const int ServoC = 9;
const int ServoD = 6;

Servo doorServo;

const int clockwise = 1700;
const int counterclockwise = 1300;

const int Acost = 1; //cost in number of quarters for items
const int Bcost = 1;
const int Ccost = 2;
const int Dcost = 3;

const int LEDready = 13;
const int coinInsert = 12;

long previousMillis = 0;
long intervalIdle = 500;
int LEDreadyState = LOW;

int y1= 0; int y2= 10; //position shifters for LCD txt

void setup() { 
  doorServo.attach(5);
  pinMode(encoderSwitch, INPUT_PULLUP);
  digitalWrite(encoderSwitch, HIGH);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(interrupt0, ClockChanged, CHANGE);//Set the interrupt 0 handler, trigger level change
  
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.setContrast(70);
  

  pinMode(LEDready, OUTPUT);
  pinMode(coinInsert, INPUT_PULLUP);
 
  myGLCD.invert(true);
  myGLCD.print("VenDuino", CENTER, 1);
  myGLCD.print("by Ryan", CENTER, 10);
  myGLCD.print("(c) 2018", CENTER, 30);
  myGLCD.setFont(TinyFont);
   myGLCD.print("RETROBUILTGAMES.com", CENTER, 40);
  myGLCD.drawRoundRect(13, 0, 70, 8);
  myGLCD.drawRect(0, 20, 83, 47); 
  myGLCD.update(); delay(500); myGLCD.clrScr(); myGLCD.update();     
 
   myGLCD.clrScr();
  for (int i=0; i<48; i+=2)
  { myGLCD.drawLine(0, i, 83, 47-i);
    myGLCD.update(); delay(2); }
  for (int i=83; i>=0; i-=2)
  { myGLCD.drawLine(i, 0, 83-i, 47);
    myGLCD.update(); delay(2);}
  }
//_______________________MASTER LOOP__start____________________
void loop(){ 
  coinState=digitalRead(coinInsert);
  myGLCD.update();
    
  if (coinState != lastcoinState) { // if the state has changed, increment the counter
    if (coinState == LOW) {// if the current state is LOW then the button went from off to on:
       myGLCD.invert(false); myGLCD.update();
      coinCounter++; }
      delay(50);}        
lastcoinState = coinState; 


//--------------coin detected----------------------------
 if (coinCounter >=1) { //when a coin is inserted, stop showing "insert coin screen"
  LEDreadyState = HIGH;
  myGLCD.clrScr();
  gfx();
  rotatySelect();    } 
 
  else {
     standby();}

//myGLCD.printNumI(coinCounter,0,40) ; myGLCD.update(); //debug----- show coin count on screen
 
}

//_______________________MASTER LOOP_end_____________________
 void standby(){ 
//--Blink Ready LED---------------------------------------------------
 unsigned long currentMillis = millis(); 
  if(currentMillis - previousMillis >= intervalIdle) { // save the last time you blinked the LED 
   previousMillis = currentMillis; 
  
   if (LEDreadyState == LOW){  // if the LED is off turn it on and vice-versa:
      LEDreadyState = HIGH; 
      
     myGLCD.clrScr(); 
     myGLCD.setFont(SmallFont);
     myGLCD.print("Please insert", CENTER, y1);
     //myGLCD.print("$", CENTER, y2);
     myGLCD.invert(false);
     //myGLCD.setFont(MediumNumbers);
     myGLCD.print("$0.25", CENTER, y2);
     myGLCD.update();      }
    
   else
     LEDreadyState = LOW;
     digitalWrite(LEDready, LEDreadyState);// set the LED with the ledState of the variable:
     y1=y1 +5; y2=y2+5; //shift the text position down
     if ((y1==35) && (y2== 45)) //when the text almost goes off-screen, move it back up
        {y1=0; y2=10; }
  }
 }
//---Wait for Coin/Credit---------------------------------------------------------------

 //This Section constrains how far the encoder can count to, and displays the value on screen as selection "A1","B1","C2","D2".
 // for example, if the encoder is rotated and the tracked number is between 1 and 10, then the selection is item "A1".
void rotatySelect(){
 digitalWrite(LEDready,LOW);  //turn the LCD backlight on
count =constrain(count, 0, 41);
if (count >0   && count <=10) { myGLCD.print("A1",67,10); myGLCD.print("A1", 22, 24);}           
if (count >=11 && count <=20) { myGLCD.print("B1",67,10); myGLCD.print("B1", 38, 24);}
if (count >=21 && count <=30) { myGLCD.print("C2",67,10); myGLCD.print("C2", 22, 37);}
if (count >=31 && count <=41) { myGLCD.print("D2",67,10); myGLCD.print("D2", 37, 37);}
//if (count >=41 && count <=50) { myGLCD.print("E3",67,10); myGLCD.update();}
//if (count >=51 && count <=60) { myGLCD.print("F3",67,10); myGLCD.update();}
//if (count >= 41) {count = count - 40;}                  //roll over to zero
//myGLCD.printNumI(count,70, 40);myGLCD.update();                     //------debug show encoder count position

//This section handles the item selection and if there are enough credits to vend the item
if (digitalRead(encoderSwitch)== LOW && count >0 && count <=10 && coinCounter >=Acost) 
{servoA();}
if (digitalRead(encoderSwitch)== LOW && count >0 && count <=10 && coinCounter <Acost) 
{  insufficientCredits();}

if (digitalRead(encoderSwitch)== LOW && count >=11 && count <=20 && coinCounter >=Bcost) 
{servoB();}
if (digitalRead(encoderSwitch)== LOW && count >11 && count <=20 && coinCounter <Bcost) 
{  insufficientCredits();}

if (digitalRead(encoderSwitch)== LOW && count >=21 && count <=30 && coinCounter >=Ccost) 
{servoC();}
if (digitalRead(encoderSwitch)== LOW && count >21 && count <=30 && coinCounter <Ccost) 
{  insufficientCredits();}

if (digitalRead(encoderSwitch)== LOW && count >=31 && count <=40 && coinCounter >=Dcost) 
{servoD();}
if (digitalRead(encoderSwitch)== LOW && count >=31 && count <=40 && coinCounter <Dcost) 
{ insufficientCredits();}

  
}
void insufficientCredits(){
 myGLCD.clrScr(); myGLCD.print("Not enough",CENTER,0); myGLCD.print("Credits.",CENTER,10); myGLCD.update(); delay(700);  
}

void servoA(){
   doorServo.write(0);                  // open door for vending 
   delay(15);                           // waits for servo to get there 
     for(int i=0; i<2; i++) 
    {digitalWrite(LEDready, HIGH); delay(50); digitalWrite(LEDready, LOW); delay(50); }   
 myGLCD.clrScr(); myGLCD.print("Vending", CENTER, 0);
 myGLCD.drawRect(20, 20, 63, 47); myGLCD.drawRect(20, 20, 41, 47);
 myGLCD.drawRect(20, 20, 63, 34); myGLCD.print("A1", 25, 24); 
 myGLCD.update();
   for(int i=0; i<57; i++){                  // change this to adjust +- full revolution
    digitalWrite(ServoA,HIGH);
    delayMicroseconds(clockwise); 
    digitalWrite(ServoA,LOW);
    delay(18.5); // 18.5ms 
   } coinCounter=coinCounter -Acost; count = 0; myGLCD.clrScr();
  delay(1000);                       // some time for item to be redeemed 
  doorServo.write(90); delay(15);    //close door for vending 
 }


void servoB(){
  doorServo.write(0);
 for(int i=0; i<2; i++) 
    {digitalWrite(LEDready, HIGH); delay(50); digitalWrite(LEDready, LOW); delay(50); } 
 myGLCD.clrScr(); myGLCD.print("Vending", CENTER, 0);
 myGLCD.drawRect(20, 20, 63, 47); myGLCD.drawRect(20, 20, 41, 47);
 myGLCD.drawRect(20, 20, 63, 34); myGLCD.print("B1", 46, 24); 
 myGLCD.update();
   for(int i=0; i<57; i++){
    digitalWrite(ServoB,HIGH);
    delayMicroseconds(counterclockwise); 
    digitalWrite(ServoB,LOW);
    delay(18.5); // 18.5ms 
    } coinCounter=coinCounter -Bcost;   count = 0;  myGLCD.clrScr();
 delay(1000);                       // some time for item to be redeemed 
 doorServo.write(90); delay(15);    //close door for vending 
  }


void servoC(){
  doorServo.write(0);
 for(int i=0; i<2; i++) 
    {digitalWrite(LEDready, HIGH); delay(50); digitalWrite(LEDready, LOW); delay(50); } 
 myGLCD.clrScr(); myGLCD.print("Vending", CENTER, 0);
 myGLCD.drawRect(20, 20, 63, 47); myGLCD.drawRect(20, 20, 41, 47);
 myGLCD.drawRect(20, 20, 63, 34); myGLCD.print("C2", 25, 37); 
 myGLCD.update();
   for(int i=0; i<57; i++){
    digitalWrite(ServoC,HIGH);
    delayMicroseconds(counterclockwise); 
    digitalWrite(ServoC,LOW);
    delay(18.5); // 18.5ms 
    }coinCounter=coinCounter -Ccost;  count = 0;  
 delay(1000);                       // some time for item to be redeemed 
 doorServo.write(90); delay(15);    //close door for vending 
  }


void servoD(){
  doorServo.write(0);
 for(int i=0; i<2; i++) 
    {digitalWrite(LEDready, HIGH); delay(50); digitalWrite(LEDready, LOW); delay(50); } 
 myGLCD.clrScr(); myGLCD.print("Vending", CENTER, 0);
 myGLCD.drawRect(20, 20, 63, 47); myGLCD.drawRect(20, 20, 41, 47);
 myGLCD.drawRect(20, 20, 63, 34); myGLCD.print("D2", 46, 37); 
 myGLCD.update();
   for(int i=0; i<57; i++){
    digitalWrite(ServoD,HIGH);
    delayMicroseconds(counterclockwise); 
    digitalWrite(ServoD,LOW);
    delay(18.5); // 18.5ms 
   }coinCounter=coinCounter -Dcost;  count = 0;  
  delay(1000);                       // some time for item to be redeemed 
  doorServo.write(90); delay(15);    //close door for vending 
   }

void gfx(){
  myGLCD.setFont(SmallFont);
  myGLCD.print("Credit:", 0, 0); 
  myGLCD.print("Selection:", 0, 10);      
  if (coinCounter == 1) { myGLCD.print("$0.25",50,0);}           
  if (coinCounter == 2) { myGLCD.print("$0.50",50,0);}
  if (coinCounter == 3) { myGLCD.print("$0.75",50,0);}
  if (coinCounter == 4) { myGLCD.print("$1.00",50,0);}
  if (coinCounter >= 5) { myGLCD.print("$.25x ",40,0); myGLCD.printNumI(coinCounter,72,0); }
  myGLCD.drawRect(53, 24, 59, 28);
  myGLCD.drawRect(20, 20, 35, 47);   //(x1,y1, x2,y2)
  myGLCD.drawRect(35, 20, 50, 47);
  myGLCD.drawRect(50, 20, 62, 47); 
  myGLCD.drawLine(20, 33, 50, 33);  
  myGLCD.drawCircle(56, 39, 2); myGLCD.drawCircle(56, 39, 1); 
  myGLCD.update();
}


void ClockChanged(){
  int clkValue = digitalRead(CLK);//Read the CLK pin level
  int dtValue = digitalRead(DT);//Read the DT pin level
  if (lastCLK != clkValue)
  {
    lastCLK = clkValue;
    count += (clkValue != dtValue ? 1 : -1);//CLK and inconsistent DT + 1, otherwise - 1
     
  }}
