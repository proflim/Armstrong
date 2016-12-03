/* FSR testing sketch. 
 
Connect one end of FSR to 5V, the other end to Analog 0.
Then connect one end of a 10K resistor from Analog 0 to ground
Connect LED from pin 11 through a resistor to ground 
 
For more information see www.ladyada.net/learn/sensors/fsr.html */

#include <math.h>
#include <SoftwareSerial.h>

SoftwareSerial bluetooth(8,9);
const int trig_pin = 12;
const int echo_pin = 13;

int LEDpin = 11;      // connect Red LED to pin 11 (PWM pin)

int left_Reading0;      // the analog reading from the FSR resistor divider
int left_Reading1;

int right_Reading0;
int right_Reading1;

int foot_Reading0;
int foot_Reading1;

int LEDbrightness;

float distcache = 0;
bool pushupflag = false;
int pressurecount = 0;
float lefthand [10] = {0,0,0,0,0,0,0,0,0,0};
float righthand [10] = {0,0,0,0,0,0,0,0,0,0};
float presssuretol = 200.00;
float preesuremin = 600;

bool exerciseflag = false;

void setup() {
  Serial.begin(4800);   // We'll send debugging information via the Serial monitor
  bluetooth.begin(9600);
  pinMode(LEDpin, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(trig_pin,OUTPUT);
  pinMode(echo_pin,INPUT);
}
 
void loop(void) {
  char c = 'Z';
  measurePressure();
  if (bluetooth.available()){
    
        c = bluetooth.read();
        bluetooth.println(c);
  }

    if (c=='A'&&!exerciseflag){
      exerciseflag = true;
//      Serial.println("start exercise");
       pushupflag = false;
       pressurecount = 0;
       resetArray(lefthand,10);  
       resetArray(righthand,10);
    }
    else if (c=='B' && exerciseflag){
//      Serial.println("end exercise");
      exerciseflag = false;
       pushupflag = false;
       pressurecount = 0;
       resetArray(lefthand,10);  
       resetArray(righthand,10);
    }

  if (exerciseflag){
    supersonichandler();
  }
  else {
    delay(20);
  }
  
  
  // we'll need to change the range from the analog reading (0-1023) down to the range
  // used by analogWrite (0-255) with map!
//  LEDbrightness = map(fsrReading0, 0, 1023, 0, 255);
//  // LED gets brighter the harder you press
//  analogWrite(LEDpin, LEDbrightness);
// Serial.println("");

}




float convert(int analog){ 
  return analog;
//  if (analog>=1023) analog=1022;
//  
//  
//  float V = (1023-analog)/1023.0*5;
////  Serial.print("V: ");
////  Serial.print(V);
////  Serial.print(", Analog: ");
////  Serial.println(analog);
//  if(V<1) return 0;
//  else if (V==5.0) return 0;
//  else if (V>3) return 2*(V-2.9);
//  else return (V-1.8846)/0.002/1000;
//
////  
////  float R = 10*analog/(1023-analog);
//////  return pow(6.25*0.69/R,1.412)/9.8;
////  return analog;
}

void measurePressure(){

    left_Reading0 = analogRead(A0); 
  delay(1);
  
  left_Reading1 = analogRead(A1);
  delay(3);
  //----------------Right
  right_Reading0 = analogRead(A2);
  delay(1);
  
  right_Reading1 = analogRead(A3);
  delay(3);

  //----------------Foot
  foot_Reading0 = analogRead(A4);
  delay(1);
  foot_Reading1 = analogRead(A5);
  
//  Serial.print("Foot reading 1 = ");
//  Serial.println(foot_Reading1);
  
  Serial.print(left_Reading0, DEC);
  Serial.print(",");
  Serial.print(left_Reading1, DEC);
  Serial.print(",");
  
  Serial.print(right_Reading0, DEC);
  Serial.print(",");
  Serial.print(right_Reading1, DEC);
  Serial.print(",");

  Serial.print(foot_Reading0, DEC);
  Serial.print(",");
  Serial.print(foot_Reading1, DEC);
  
  Serial.print("|");

}
void pressurehandler(float left[],float right[], int n){
  measurePressure();
  //----------------Left

//  
//  Serial.print(foot_Reading0, DEC);
//  Serial.print(",");
//  Serial.print(foot_Reading1, DEC);

//  Serial.print((left_Reading0+left_Reading1)/2.0, DEC);
//  Serial.print(",");
//  Serial.println((right_Reading0+right_Reading1)/2.0, DEC);
 
  delay(1);

  if (pressurecount<n){
    left[pressurecount] = (left_Reading0+left_Reading1)/2.0;
    right[pressurecount] = (right_Reading0+right_Reading1)/2.0;
    pressurecount += 1;
  }
  
}

void resetArray(float thearray[], int n){
  for (int i=0; i<n; i++){
    thearray[i] = 0;
  }
}

//Supersonic
float supersonichandler(){

  digitalWrite(trig_pin,LOW);
  delay(2);
  digitalWrite(trig_pin,HIGH);
  delay(10);
  digitalWrite(trig_pin,LOW);
  float dist= pulseIn(echo_pin,HIGH)/58.00;
//  Serial.println(dist);

  delay(20);
  if (dist < 100){
    distcache = dist;
    if (dist<15){
      if (!pushupflag){
         pushupflag = true;
         pressurecount = 0;
         resetArray(lefthand,10);  
         resetArray(righthand,10);
      }
  
      pressurehandler(lefthand,righthand,10);
    }
    else if (dist>=15 && pushupflag){
      pushupflag = false;

      float sumL = 0, sumR = 0;
      for (int i = 0; i<pressurecount; i++){
        sumL += lefthand[i];
        sumR += righthand[i];
      }
      float aveL = sumL/(float)pressurecount;
      float aveR = sumR/(float)pressurecount;
      Serial.print("lefthand: ");
      Serial.print(aveL);
      Serial.print(" righthand: ");
      Serial.println(aveR);
      
      pressurecount = 0;
       
       resetArray(lefthand,10);  
       resetArray(righthand,10);

      
      if (aveL < aveR - presssuretol){
        bluetooth.println("R");
      }
      else if (aveL > aveR + presssuretol){
        bluetooth.println("L");
      }
      else if (aveL < preesuremin || aveR < preesuremin){
        bluetooth.println("U");
      }
      else{
        bluetooth.println("S");
      }
      
    }
    else{
      measurePressure();
    }
    return dist;
  }
  else{
    return distcache;
  }
}
