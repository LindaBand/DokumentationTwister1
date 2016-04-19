/*You see here the complete code of an electronical Twister game. See the documentation for 
further details!

Have fun, Linda and Karl*/


#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// ## light: Control of the LED Strip
#define PIN 6 //LED Pin
Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, 6, NEO_GRB + NEO_KHZ800);

// ## servo: Moves the hand and the foot in the play box

// servo is on pin 9 @see setup()
Servo myservo;  // create servo object to control a servo
int servoPos = 0;    // variable to store the servo position

// ## field: Control of the playing field
float Quellspannung = 5.0;
int R1 = 2700.0; // Reference resistor value (is higher than the resistors used on the body parts)

// MAP Field -> AnalogPin, every circle on the playing field is listed here. 
int Field00 = 0;
int Field01 = 1;
int Field02 = 2;
int Field03 = 3;
int Field04 = 4;
int Field05 = 5;
int Field06 = 6;
int Field07 = 7;
int Field08 = 8;
int Field09 = 9;
int Field10 = 10;
int Field11 = 11;
int Field12 = 12;
int Field13 = 13;
int Field14 = 14;
int Field15 = 15;

// ## Logic of the game
boolean SHOW_MODE = false; // Mode where it is not checked whether everyone is at the right place (due to calculating the game logic)
// player
// values of last limb scan 99 == not found on field
int Pos_P1_L_Hand = 99;
int Pos_P1_R_Hand = 99;
int Pos_P1_L_Foot = 99;
int Pos_P1_R_Foot = 99;
int P1_Cnt_Errors = 0;
int P1_LapErrorCount = 0;

int Pos_P2_L_Hand = 99;
int Pos_P2_R_Hand = 99;
int Pos_P2_L_Foot = 99;
int Pos_P2_R_Foot = 99;
int P2_Cnt_Errors = 0;
int P2_LapErrorCount = 0;

int Lap_ErrorCountPenalty = 5; //Displays when a player has done the wrong move too often. (punishment = increase Player_Cnt_Errors): 
int Lap_ErrorCheckCount = 10; // Displays how many times in a round the movements get checked.
int Lap_ErrorCheckDelay = 1000; // Defines the length of delay between these checks.
int LapColor; // 0=green 1=yellow 2=blue 3=red
int LapLimb;  // 0=hand 1=foot
int LapBodySide; // 0=left  1=right

int ERROR_PENALTY_GAMEOVER = 2;
boolean gameover = false;
boolean P1_HasLost = false;
boolean P2_HasLost = false;
boolean InStartPhase = true;
int TimesGeneratorCalled = 0;

// rules
// position of last valid postion, 77 = postion not set but no error e.g. at the beginning of the game
int LastValid_Pos_P1_L_Hand = 9;
int LastValid_Pos_P1_R_Hand = 8;
int LastValid_Pos_P1_L_Foot = 15;
int LastValid_Pos_P1_R_Foot = 14;

int LastValid_Pos_P2_L_Hand = 7;
int LastValid_Pos_P2_R_Hand = 6;
int LastValid_Pos_P2_L_Foot = 3;
int LastValid_Pos_P2_R_Foot = 2;

// ###### SERVO #########

void rotateFootUp() {
  Serial.print("footUp pos ");
  //Serial.print(servoPos);
  Serial.println("+");
  for (servoPos; servoPos <= 180; servoPos += 1) { // goes from 0 degrees to 180 degrees
    myservo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

}

void rotateParking() { //Nor the hand or the foot are visible. 

  Serial.print("parking pos ");
  //Serial.print(servoPos);
  if (servoPos < 90) {
    Serial.println("+");
    for (servoPos; servoPos <= 90; servoPos += 1) { // goes from 0 degrees to 180 degrees
      myservo.write(servoPos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  } else {
    Serial.println("-");

    for (servoPos; servoPos >= 90; servoPos -= 1) { // goes from 0 degrees to 180 degrees
      myservo.write(servoPos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  }
}
void rotateHandUp() {

  Serial.print("handUp pos ");
  //Serial.print(servoPos);
  Serial.println("+");
  for (servoPos; servoPos >= 0; servoPos -= 1) { // goes from 0 degrees to 180 degrees
    myservo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}


// ###### END SERVO #####

// ###### START GAMEFIELD CONNECTIVITY #####

void doConnectivityTest(boolean fast) { //Tests the connectivity 
  doConnectivityTest(Field00 , R1, fast);
  doConnectivityTest(Field01 , R1, fast);
  doConnectivityTest(Field02 , R1, fast);
  doConnectivityTest(Field03 , R1, fast);

  doConnectivityTest(Field04 , R1, fast);
  doConnectivityTest(Field05 , R1, fast);
  doConnectivityTest(Field06 , R1, fast);
  doConnectivityTest(Field07 , R1, fast);

  doConnectivityTest(Field08 , R1, fast);
  doConnectivityTest(Field09 , R1, fast);
  doConnectivityTest(Field10 , R1, fast);
  doConnectivityTest(Field11 , R1, fast);

  doConnectivityTest(Field12 , R1, fast);
  doConnectivityTest(Field13 , R1, fast);
  doConnectivityTest(Field14 , R1, fast);
  doConnectivityTest(Field15 , R1, fast);
}
void doConnectivityTest(int field, int resistance, boolean fast) {
  int currField = field;

  boolean doStage1 = true;
  boolean doStage2 = true;
  Serial.print("Field ");
  Serial.print(currField);
  Serial.print(": Bitte auflegen -470ohm - ");
  while (doStage1) { // stage 1 - 470ohm resistor
    float res = measureResistance(currField, resistance);
    if (res > 440 && res < 525) {
      Serial.print(res, 2);
      Serial.print("OK");
      doStage1 = false;
    }
    delay(1000);
  }
  if (fast) {
    Serial.println(" - OKOK");
    return;
  }
  Serial.print(" 1800ohm - ");
  while (doStage2) { // stage 2 - 1800ohm resistor
    float res = measureResistance(currField, resistance);
    if (res > 1650 && res < 2000) {
      Serial.print(res, 2);
      Serial.print("OK");
      doStage2 = false;
    }
    delay(1000);
  }
  Serial.println(" - OKOK");
}

void doMeasureAllFields() { //Measures all vircles and prints out values of the circles/fields
  Serial.println("#printing measured vals of fields");
  measureResistanceLog(Field00, R1);
  measureResistanceLog(Field01, R1);
  measureResistanceLog(Field02, R1);
  measureResistanceLog(Field03, R1);

  measureResistanceLog(Field04, R1);
  measureResistanceLog(Field05, R1);
  measureResistanceLog(Field06, R1);
  measureResistanceLog(Field07, R1);

  measureResistanceLog(Field08, R1);
  measureResistanceLog(Field09, R1);
  measureResistanceLog(Field10, R1);
  measureResistanceLog(Field11, R1);

  measureResistanceLog(Field12, R1);
  measureResistanceLog(Field13, R1);
  measureResistanceLog(Field14, R1);
  measureResistanceLog(Field15, R1);
  Serial.println("#printing measured vals of fields");

}
void measureResistanceLog(int pin, int rs1) {
  long Messwert;
  float SpannungR2; //Current
  float Widerstand;

  //5 Voltage over the measured resistor
  Messwert = 0;
  for (int i = 0; i < 5; i++) {
    Messwert += analogRead(pin);
  }
  Messwert = trunc(Messwert / 5);

  //Voltage measuring via the resistor rule of physics 
  SpannungR2 = (Quellspannung / 1023.0) * Messwert;
  //Serial.print(pin, 10);
  //Serial.print(":Spannung ueber R2 betraegt ");
  //Serial.print(SpannungR2, 2);
  //Serial.println(" Volt!");
  //Berechnung: (R2 = rs1 * (U2/U1))
  Serial.print(pin, 10);
  Widerstand = rs1 * (SpannungR2 / (Quellspannung - SpannungR2));
  Serial.print("Der Widerstand hat ");
  Serial.print(Widerstand, 2);
  Serial.println(" Ohm.");
  //Serial.println();
}

float measureResistance(int pin, int rs1) {
  long Messwert;
  float SpannungR2; //Voltage over the measured resistor
  float Widerstand;

  //5 measurements and then calculating the mean
  Messwert = 0;
  for (int i = 0; i < 5; i++) {
    Messwert += analogRead(pin);
  }
  Messwert = trunc(Messwert / 5);

  //measuring the voltage 
  SpannungR2 = (Quellspannung / 1023.0) * Messwert;
  //Calculation: (R2 = rs1 * (U2/U1))
  Widerstand = rs1 * (SpannungR2 / (Quellspannung - SpannungR2));
  return Widerstand;
}

// ###### END GAMEFIELD CONNECTIVITY #####
// ###### START LIGHT ##### 
void LeftOn() { 
  //strip.setPixelColor(n, red, green, blue);
  strip.setPixelColor(2,  127, 127, 127); //White
  strip.setPixelColor(3,  127, 127, 127);
  strip.show();
}
void LeftRed() {
  strip.setPixelColor(2,  255, 0, 0); //Red
  strip.setPixelColor(3,  255, 0, 0);
  strip.show();
}
void LeftGreen() {
  strip.setPixelColor(2,   0, 255, 0); //Green
  strip.setPixelColor(3,   0, 255, 0);
  strip.show();
}
void LeftOff() {
  strip.setPixelColor(2,  0, 0, 0); //Light off
  strip.setPixelColor(3,  0, 0, 0);
  strip.show();
}

void RightOn() {
  strip.setPixelColor(0, 127, 127, 127); //White 
  strip.setPixelColor(5, 127, 127, 127);
  strip.show();
}
void RightRed() {
  strip.setPixelColor(0, 255, 0, 0); //Red
  strip.setPixelColor(5, 255, 0, 0);
  strip.show();
}
void RightGreen() {
  strip.setPixelColor(0, 0, 255, 0);
  strip.setPixelColor(5, 0, 255, 0);
  strip.show();
}
void RightOff() {
  strip.setPixelColor(0, 0, 0, 0);
  strip.setPixelColor(5, 0, 0, 0);
  strip.show();
}

void CircleRed() {
  //Red
  strip.setPixelColor(1, 255, 0, 0);
  strip.setPixelColor(4, 255, 0, 0);
  strip.show();
}

void CircleGreen() {
  //Green
  strip.setPixelColor(1, 0, 255, 0);
  strip.setPixelColor(4, 0, 255, 0);
  strip.show();
}
void CircleYellow() {
  //Yellow
  strip.setPixelColor(1, 255, 255, 0); //Yellow  
  strip.setPixelColor(4, 255, 255, 0);
  strip.show();
}
void CircleBlue() {
  //Blue
  strip.setPixelColor(1, 0, 0, 255);
  strip.setPixelColor(4, 0, 0, 255);
  strip.show();
}
void CircleOn(int color) { //The colors have integer values assigned to them, this is practical for the game generator 
  if (LapColor == 0) {
    CircleGreen();
  }
  if (LapColor == 1) {
    CircleYellow();
  }
  if (LapColor == 2) {
    CircleBlue();
  }
  if (LapColor == 3) {
    CircleRed();
  }
}
void CircleOff() {
  strip.setPixelColor(1, 0, 0, 0);
  strip.setPixelColor(4, 0, 0, 0);
  strip.show();
}


// The flickering shows that the round will be over soon. 
void doCircleFlickering() {
  CircleOn(LapColor);
  delay(1200);
  CircleOff();
  delay(1200);

  CircleOn(LapColor);
  delay(1000);
  CircleOff();
  delay(1000);

  CircleOn(LapColor);
  delay(800);
  CircleOff();
  delay(800);

  CircleOn(LapColor);
  delay(600);
  CircleOff();
  delay(600);

  CircleOn(LapColor);
  delay(500);
  CircleOff();
  delay(500);

  CircleOn(LapColor);
  delay(500);
  CircleOff();
  delay(500);

  CircleOn(LapColor);
  delay(300);
  CircleOff();
  delay(300);

  CircleOn(LapColor);
  delay(300);
  CircleOff();
  delay(300);

  CircleOn(LapColor);
  delay(100);
  CircleOff();
  delay(100);

  CircleOn(LapColor);
  delay(100);
  CircleOff();
  delay(100);

  CircleOn(LapColor);
  delay(100);
  CircleOff();
  delay(100);

  CircleOn(LapColor);
  delay(100);
  CircleOff();
  delay(100);
}
// ###### END LIGHT ####
// ###### START LOGIC ####

// ! this method will overwrite any already found limbs on an other field
void recognizeLimb(int field, int rs1) {
  float res = measureResistance(field, rs1);


  // ! do not set a limb to not found as it will overwrite found limbs on other fields!
  //

//This describes the different resistor values. 
  //---400
  // 470    s1 left   hand
  if (res > 400 && res <= 570) {
    Serial.print("s1 lh on ");
    Serial.println( field);
    Pos_P1_L_Hand = field;
  }
  //---570
  // 680    s1 right  hand
  if (res > 570 && res <= 780) {
    Serial.print("s1 rh on ");
    Serial.println( field);
    Pos_P1_R_Hand = field;
  }
  //---780
  // 820    s2 left   hand
  if (res > 720 && res <= 920) {
    Serial.print("s2 lh on ");
    Serial.println( field);
    Pos_P2_L_Hand = field;
  }
  //---920
  // 1k     s2 right  hand
  if (res > 920 && res <= 1100) {
    Serial.print("s2 rh on ");
    Serial.println( field);
    Pos_P2_R_Hand = field;
  }
  //---1,1k
  // 1,2k   s1 left   foot
  if (res > 1100 && res <= 1350) {
    Serial.print("s1 lf on ");
    Serial.println( field);
    Pos_P1_L_Foot = field;
  }
  //---1,35k
  // 1,5k   s1 right  foot
  if (res > 1350 && res <= 1650) {
    Serial.print("s1 rf on ");
    Serial.println( field);
    Pos_P1_R_Foot = field;
  }
  //--- 1,65k
  // 1,8k   s2 left   foot
  if (res > 1650 && res <= 2000) {
    Serial.println("s2 lf on ");
    Serial.println( field);
    Pos_P2_L_Foot = field;
  }
  //---2k
  // 2,2k   s2 right  foot
  if (res > 2000 && res <= 2400) {
    Serial.println("s2 rf on ");
    Serial.println( field);
    Pos_P2_R_Foot = field;
  }
  //---2,4k

}

void scanFullFieldForLimbs(int rs1) {
  // reset last scan
  Pos_P1_L_Hand = 99;
  Pos_P1_R_Hand = 99;
  Pos_P1_L_Foot = 99;
  Pos_P1_R_Foot = 99;

  Pos_P2_L_Hand = 99;
  Pos_P2_R_Hand = 99;
  Pos_P2_L_Foot = 99;
  Pos_P2_R_Foot = 99;

  // make new scan
  recognizeLimb(Field00, rs1);
  recognizeLimb(Field01, rs1);
  recognizeLimb(Field02, rs1);
  recognizeLimb(Field03, rs1);

  recognizeLimb(Field04, rs1);
  recognizeLimb(Field05, rs1);
  recognizeLimb(Field06, rs1);
  recognizeLimb(Field07, rs1);

  recognizeLimb(Field08, rs1);
  recognizeLimb(Field09, rs1);
  recognizeLimb(Field10, rs1);
  recognizeLimb(Field11, rs1);

  recognizeLimb(Field12, rs1);
  recognizeLimb(Field13, rs1);
  recognizeLimb(Field14, rs1);
  recognizeLimb(Field15, rs1);

}

void printLimbScanValues() { // Prints every measurement
  Serial.println("# last LimbScan values");
  Serial.print("Player 1");
  Serial.print(" hand l: ");
  Serial.print(Pos_P1_L_Hand);
  Serial.print(" r: ");
  Serial.print(Pos_P1_R_Hand);
  Serial.print(" foot L: ");
  Serial.print(Pos_P1_L_Foot);
  Serial.print(" R: ");
  Serial.println(Pos_P1_R_Foot);

  Serial.print("Player 2");
  Serial.print(" hand l: ");
  Serial.print(Pos_P2_L_Hand);
  Serial.print(" r: ");
  Serial.print(Pos_P2_R_Hand);
  Serial.print(" foot l: ");
  Serial.print(Pos_P2_L_Foot);
  Serial.print(" r: ");
  Serial.println(Pos_P2_R_Foot);
  Serial.println("#");
}

void generateNextRound() { // Starts a next round
  TimesGeneratorCalled++;
  if (TimesGeneratorCalled <= 4 && InStartPhase == true) {
    switch (TimesGeneratorCalled) {
      case 1:
        LapBodySide = 1;
        LapLimb = 0;
        break;
      case 2:
        LapBodySide = 1;
        LapLimb = 1;
        break;
      case 3:
        LapBodySide = 0;
        LapLimb = 1;
        break;
      case 4:
        LapBodySide = 0;
        LapLimb = 0;
        break;
    }
  }
  else {
    InStartPhase = false; 
    LapBodySide = random(0, 2);
    LapLimb = random(0, 2);
  }
  LapColor = random(0, 4);
}

// ###### END LOGIC ####

// ###### START LOOP ########
// code
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Twister Game says hello :)");
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  // light
  // put your setup code here, to run once:
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //Random generator for testing purposes 
  randomSeed(analogRead(0));
  rotateParking();

  Serial.println("setup finish");

  delay(8000);
  rotateHandUp();
  delay(1000);
  rotateFootUp();
  delay(1000);
  rotateParking();
  delay(1000);
  CircleBlue();
  delay(1000);
  CircleOff();
}

void loop() {
  scanFullFieldForLimbs(R1);
  printLimbScanValues();
  delay(3000);
  /**
  // put your main code here, to run repeatedly:
  if (!gameover) {
    doRound();
  }
  if (gameover) {
    Serial.println("gameoverloop");
    // flash lights
    // flash light of winner green

    // flash light of looser red
    // p2 has won
    if(P1_HasLost && !P2_HasLost){
      Serial.println("p2won");
      LeftRed();
      RightGreen();
      CircleOn(random(0,4));
      delay(1000);
      LeftOff();
      CircleOff();
      RightOff();
    }
    // p1 has won
    if(!P1_HasLost && P2_HasLost){
      Serial.println("p1won");
      LeftGreen();
      RightRed();
      CircleOn(random(0,4));
      delay(1000);
      LeftOff();
      CircleOff();
      RightOff();
    }
    // patt
    if(P1_HasLost && P2_HasLost){
      Serial.println("patt");
      LeftRed();
      RightRed();
      CircleOn(random(0,4));
      delay(1000);
      LeftOff();
      CircleOff();
      RightOff();
    }
    delay(1000);
  }
  */
}

void doRound() {
  P1_LapErrorCount = 0;
  P2_LapErrorCount = 0;
  rotateParking();
  // ### step 1 - start a round
  generateNextRound();
  // ### step 2 - tell the user
  // light: tells the players the game started
  if (LapBodySide == 0) {
    LeftOn();
    RightOff();
  } else {
    LeftOff();
    RightOn();
  }
  CircleOn(LapColor);
  // servo
  if (LapLimb == 0) {
    rotateHandUp();
  } else {
    rotateFootUp();
  }

  // ### step 3 - give user time to reposition
  delay(7000);  //pause
  doCircleFlickering();

  // ### step 4 - update users position
  scanFullFieldForLimbs(R1);
  // ### step 5 - check if users are in correct position
  Serial.println("#### Do Lap Error Checks #### ");
  for (int i = 0 ; i < Lap_ErrorCheckCount; i++) {
    doCheckRules();
    delay(Lap_ErrorCheckDelay);
    Serial.println("doing next check");
  }
  Serial.println("end of checks");
  Serial.print("P1 errors: ");
  Serial.println(P1_LapErrorCount);
  Serial.print("P2 errors: ");
  Serial.print(P2_LapErrorCount);
  if (P1_LapErrorCount >= Lap_ErrorCountPenalty) {
    P1_Cnt_Errors++;
    Serial.println("Increasing Player 1 Error Count to: ");
    Serial.println(P1_Cnt_Errors);
  }
  if (P2_LapErrorCount >= Lap_ErrorCountPenalty) {
    P2_Cnt_Errors++;
    Serial.println("Increasing Player 2 Error Count to: ");
    Serial.println(P2_Cnt_Errors);
  }
  // ### step 6 - check if someone has lost
  // TODO
  if (P1_Cnt_Errors >= ERROR_PENALTY_GAMEOVER) {
    P1_HasLost = true;
    Serial.println("Player 1 has lost");
  }
  if (P2_Cnt_Errors >= ERROR_PENALTY_GAMEOVER) {
    P2_HasLost = true;
    Serial.println("Player 2 has lost");
  }
  if (P1_HasLost || P2_HasLost) {
    gameover = true;
    rotateParking();
    Serial.println("###### GAMEOVER ####### ");
  }
}

void doCheckRules() { //Check if every limb is at correct position
  boolean P1_Error_Occured = false;
  boolean P2_Error_Occured = false;
  // # check limbs
  // ## Player 1
  // left hand
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P1_L_Hand == Pos_P1_L_Hand);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 0 && LapLimb == 0);
    int colorOfCurrentField = getColorOfField(Pos_P1_L_Hand);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P1_L_Hand != 77) {
      // limb was not moved
      Serial.println("p1 left hand was not moved");
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P1_L_Hand != 77) {
      // wrong limb was moved
      Serial.println("p1 left hand wrong limb was moved");
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P1_L_Hand = Pos_P1_L_Hand;
      } else {
        Serial.println("p1 left hand wrong field ");
        //wrong field
        P1_Error_Occured = true;
      }
    }
  }
  // right hand
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P1_R_Hand == Pos_P1_R_Hand);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 1 && LapLimb == 0);
    int colorOfCurrentField = getColorOfField(Pos_P1_R_Hand);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P1_R_Hand != 77) {
      // limb was not moved
      Serial.println("p1 right hand was not moved");
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P1_R_Hand != 77) {
      // wrong limb was moved
      Serial.println("p1 right hand was moved wrong");
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P1_R_Hand = Pos_P1_R_Hand;
      } else {
        //wrong field
        Serial.println("p1 right hand was not moved wrong");
        P1_Error_Occured = true;
      }
    }
  }
  // left Foot
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P1_L_Foot == Pos_P1_L_Foot);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 0 && LapLimb == 1);
    int colorOfCurrentField = getColorOfField(Pos_P1_L_Foot);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P1_L_Foot != 77) {
      // limb was not moved
      Serial.println("p1 left foot was not moved");
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P1_L_Foot != 77) {
      // wrong limb was moved
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P1_L_Foot = Pos_P1_L_Foot;
      } else {
        //wrong field
        P1_Error_Occured = true;
      }
    }
  }
  // right foot
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P1_R_Foot == Pos_P1_R_Foot);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 1 && LapLimb == 1);
    int colorOfCurrentField = getColorOfField(Pos_P1_R_Foot);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P1_R_Foot != 77) {
      // limb was not moved
      Serial.println("p1 left foot was not moved");
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P1_R_Foot != 77) {
      // wrong limb was moved
      P1_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P1_R_Foot = Pos_P1_R_Foot;
      } else {
        //wrong field
        P1_Error_Occured = true;
      }
    }
  }

  // ## Player 2
  // left hand
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P2_L_Hand == Pos_P2_L_Hand);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 0 && LapLimb == 0);
    int colorOfCurrentField = getColorOfField(Pos_P2_L_Hand);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P2_L_Hand != 77) {
      // limb was not moved
      Serial.println("p2 left hand was not moved");
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P2_L_Hand != 77) {
      // wrong limb was moved
      Serial.println("p2 wrong limb was moved");
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P2_L_Hand = Pos_P2_L_Hand;
      } else {
        //wrong field
        P2_Error_Occured = true;
      }
    }
  }
  // right hand
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P2_R_Hand == Pos_P2_R_Hand);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 1 && LapLimb == 0);
    int colorOfCurrentField = getColorOfField(Pos_P2_R_Hand);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P2_R_Hand != 77) {
      // limb was not moved
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P2_R_Hand != 77) {
      // wrong limb was moved
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P2_R_Hand = Pos_P2_R_Hand;
      } else {
        //wrong field
        P2_Error_Occured = true;
      }
    }
  }
  // left Foot
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P2_L_Foot == Pos_P2_L_Foot);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 0 && LapLimb == 1);
    int colorOfCurrentField = getColorOfField(Pos_P2_L_Foot);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P2_L_Foot != 77) {
      // limb was not moved
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P2_L_Foot != 77) {
      // wrong limb was moved
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P2_L_Foot = Pos_P2_L_Foot;
      } else {
        //wrong field
        P2_Error_Occured = true;
      }
    }
  }
  // right hand
  if (SHOW_MODE == false) {
    boolean isLimbOnSamePosition = (LastValid_Pos_P2_R_Foot == Pos_P2_R_Foot);
    boolean shouldLimbShouldBeMoved = (LapBodySide == 1 && LapLimb == 1);
    int colorOfCurrentField = getColorOfField(Pos_P2_R_Foot);
    if (shouldLimbShouldBeMoved == true && isLimbOnSamePosition == true && Pos_P2_R_Foot != 77) {
      // limb was not moved
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == false && Pos_P2_R_Foot != 77) {
      // wrong limb was moved
      P2_Error_Occured = true;
    }
    if (shouldLimbShouldBeMoved == false && isLimbOnSamePosition == true) {
      // is limb on right position?
      // has field the right color?
      if (colorOfCurrentField == LapColor) {
        LastValid_Pos_P2_R_Foot = Pos_P2_R_Foot;
      } else {
        //wrong field
        P2_Error_Occured = true;
      }
    }
  }
  // end of checks
  

  // ## display errors: Displays the errors on the display. 
  if (P1_Error_Occured) {
    P1_LapErrorCount++;
    LeftRed();
  } else {
    if (LapBodySide == 0) {
      LeftOn();
    } else {
      LeftOff();
    }
  }
  if (P2_Error_Occured) {
    P2_LapErrorCount++;
    RightRed();
  } else {
    if (LapBodySide == 1) {
      RightOn();
    } else {
      RightOff();
    }
  }
}
int getColorOfField(int field) {
  if (field == 0 || field == 4 || field == 8 || field == 12) {
    return 0; // green
  }
  if (field == 1 || field == 5 || field == 9 || field == 13) {
    return 1; // yellow
  }
  if (field == 2 || field == 6 || field == 10 || field == 14) {
    return 2; // blue
  }
  if (field == 3 || field == 7 || field == 11 || field == 15) {
    return 3; // rot
  }
  return 0;
}
// ###### END LOOP ########








