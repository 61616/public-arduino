/*
 * patternLockFinalBuild.ino
 * This is a locking mechanism for a bedroom door. There are buttons inside the room that open the 
 * door when pressed (constant *interiorButton* defines the arduino input pin which is configurable), 
 * and a single button outside the door which requires a specific press pattern conprising 5 presses
 * to open the door (constant *passIn* defines pin). The door used in this project is a wooden bedroom 
 * door with a latch, allowing a servo motor to lift the latch. If you use a different locking/opening 
 * mechanism on your door all you have to change is function *void open()*.
 *    SETUP
 *      To setup the system you need a button on the inside of the door hooked up to pin 4, a button 
 *    outside hooked up to pin 3, and the servo motor hooked up to pin 5. My servo has to be powered 
 *    externally because it takes a lot to lift my door latch. If you do this make sure servo ground
 *    is connected to arduino ground otherwise your servo wont work.
 *      I suggest running the program looking at the serial output during setup to calibrate your 
 *    passwords and tolerance.
 *      Also note that if a password check succeeds it will print "opened" at the end. This can be used
 *    for testing without a servo hooked up. If any password check fails debug data will be printed
 * by Nayan Sawyer
 * Final implementation of a project started years ago
 * version 1.0.1 Oct 16 2019
 * changes 1.0.1:
 *      added *void notify*
 */

#include <Servo.h>
Servo myServo;

/*
 *  VARIABLES
 */
int count = 0; // stores passIn button press count
unsigned long a[]={0,0,0,0,0}; // input storage
double del[]={0,0,0,0,0}; // delay storage
double rat[] = {0,0,0}; // ratio storage

// The following global variables can be set according to user preferance and configuration
const int numPass = 2; // Number of ratios that open door !MUST MATCH NUMBER OF ENTRIES IN VARIABLE pass!
double pass[numPass][3] = { // List of ratios that will open door
  {0.50,1.00,0.50},
  {2.00,2.00,2.00}
};
double tolerance = 0.10; // Inaccuracy tolerance
// Pin definition
const int passIn = 3; // This is the arduino pin number for exterior button (password input)
const int interiorButton = 4; // This is the arduino pin number for the interior buttons
const int servoPin = 5; // This is the arduino pin number for driving the servo

/*
 *  PROGRAM START
 */
void setup(){
  Serial.begin(115200);
  Serial.println("Ready...");
}
// After *void setup()* the program continues at *void loop()* at the very bottom

/*
 *  FUNCTIONS
 */

// Capture delays between button presses
void dataCapture(){
  a[count]=millis();
  count++;

  // Cancel button debounce
  while(digitalRead(passIn) == HIGH){
    delay(2);
  }
}

// Display delays between presses. For setup and debug
void displayData(){
  // Calcualate delays between keypresses
  del[0] = a[1] - a[0];
  del[1] = a[2] - a[1];
  del[2] = a[3] - a[2];
  del[3] = a[4] - a[3];
 
  // Display raw delay data for setup/debug
  for(int a = 0; a < 4; a++){
    Serial.print("Delay ");
    Serial.print(a);
    Serial.print(" ");
    Serial.print(del[a]);
    Serial.println(" ms");
  }

  // Calculate ratios of subsequent delays to initial delay
  rat[0] = del[1] / del[0];
  rat[1] = del[2] / del[0];
  rat[2] = del[3] / del[0];
  
  // Display ratios of key press delays for setup/debug
  Serial.println();
  Serial.println("During setup if you have entered your password correctly");
  Serial.println("the following three ratios should be put in array *pass*");
  for(int b = 0; b < 3; b++){
    Serial.println();
    Serial.print("Ratio ");
    Serial.print(b);
    Serial.print(" ");
    Serial.print(rat[b]);
  }

  Serial.println("");
  Serial.println("- - - - - - - - -");
  count=0;
  // Cancel button debounce
  while(digitalRead(passIn) == HIGH){
    delay(2); 
  }
}

// Check input combination against stored combinations. All Serial output is for setup/debug
bool checkCombo(int j){
  for(int i = 0; i < 3; i++){
    if(rat[i] < (pass[j][i] - tolerance)){
      Serial.print("below target password ");
      Serial.println(j);
      Serial.print("i = ");
      Serial.println(i);
      Serial.print("target: ");
      Serial.println(pass[j][i]);
      Serial.print("target - tolerance: ");
      Serial.println(pass[j][i] - tolerance);
      Serial.print("input: ");
      Serial.println(rat[i]);
      Serial.println();
      return false;
    }
    else if(rat[i] > (pass[j][i] + tolerance)){
      Serial.print("above target password ");
      Serial.println(j);
      Serial.print("i = ");
      Serial.println(i);
      Serial.print("target: ");
      Serial.println(pass[j][i]);
      Serial.print("target + tolerance: ");
      Serial.println(pass[j][i] + tolerance);
      Serial.print("input: ");
      Serial.println(rat[i]);
      Serial.println();
      return false;
    }
  }
  // Function only reaches here if input is within tolerance of target
  open();
  return true;
}

// Function for servo control. Current resting pos is 97, open pos is 30.
void open() {
  Serial.println("opened");
  myServo.attach(servoPin);
  delay(200);
  myServo.write(30); // Open position
  delay(500);
  myServo.write(97); // Closed position (rests here when not in use)
  delay(200);
  myServo.detach();
}

// Run servo so you know when the attempt has failed
void notify(){
  myServo.attach(servoPin);
  delay(200);
  myServo.write(110);
  delay(500);
  myServo.write(97);
  delay(200);
  myServo.detach();
}

/*
 *  PROGRAM CONTINUES
 */

// Main loop
void loop(){
  if(digitalRead(interiorButton) == HIGH){
    open();
    count = 0; // Reset for debugging
  }
  if (digitalRead(passIn)==HIGH){
    dataCapture();
  }
  if (count>4){
    displayData();
    for(int j = 0; j < numPass; j++){
      if(checkCombo(j)){
        break;
      }
      notify();
    }
  }
}
