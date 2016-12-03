
import processing.serial.*;

Serial port;  // Create object from Serial class
int val;      // Data received from the serial port
int[] left0_values;
int[] left1_values;

int[] right0_values;
int[] right1_values;


int[] left_avg_values;
int[] right_avg_values;

int left_avg;
int right_avg;

int left_0;
int left_1;
int right_0;
int right_1;

void setup() 
{
  size(1280, 660);
  // Open the port that the board is connected to and use the same speed (9600 bps)
  port = new Serial(this, Serial.list()[0], 4800);
  left0_values = new int[width];
  left1_values = new int[width];
  right0_values = new int[width];
  right1_values = new int[width];
  
  left_avg_values = new int[width];
  right_avg_values = new int[width];

  smooth();
}

//read the message in Processing
void serialEvent(Serial myPort){
 String myString = myPort.readStringUntil(124); //the ascii value of the "|" character
 if(myString != null ){
   myString = trim(myString); //remove whitespace around our values
   int inputs[] = int(split(myString, ','));
   //now assign your values in processing
   if(inputs.length == 6){
     
     left_0 = inputs[1];
     left_1 = inputs[0];
     
     right_0 = inputs[2];
     right_1 = inputs[3];

   }
 }

}

int getY(int val) {
  float R = 10000*val/(1023.0f - val);
  return (int)(val / 1023.0f * height) - 1;
}

void draw()
{
  serialEvent(port);
  colorMode(HSB, 100);
  
  
  //----------4 inputs
  for (int i=0; i<width-1; i++)
  {
    left0_values[i] = left0_values[i+1];
    left1_values[i] = left1_values[i+1];
    right0_values[i] = right0_values[i+1];
    right1_values[i] = right1_values[i+1];
    left_avg_values[i] = left_avg_values[i+1];
    right_avg_values[i] = right_avg_values[i+1];
  }
  left0_values[width-1] = left_0;
  left1_values[width-1] = left_1;
  right0_values[width-1] = right_0;
  right1_values[width-1] = right_1;
  left_avg_values[width-1] = (left_0+left_1)/2;
  right_avg_values[width-1] = (right_0+right_1)/2;
  
  int[] coords = {left_0,left_1, right_0, right_1};
  
  background(0);
  textSize(32);
  text("Left Hand", 70, 30); 
  textSize(32);
  text("Right Hand", 1030, 30);
  stroke(100,30,100);
  int buffer = 100;
  for (int x = 0; x < 4; x = x + 1) {
    //creates a counter for each of the values in the array
    //draws the rectangle for each of the values in the array to corresponding height
    if(x>1)
      buffer = 1000;
    rect (x*35+buffer, height-coords[x]/2,
    30, (coords[x])/2);
  }
  

  stroke(100,80,100);
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(left_avg_values[x-1]), 
         width-1-x, height-1-getY(left_avg_values[x]));
  }  
  

  stroke(50,80,100);
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(right_avg_values[x-1]), 
         width-1-x, height-1-getY(right_avg_values[x]));
  }  
  
}