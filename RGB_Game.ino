//www.elegoo.com
//2016.12.8

#include "pitches.h"
#include <LiquidCrystal.h>


// Joystick pin numbers
//Not Connected - const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

const int maxScore = 99; // maxScore for array indexing

// Pin Definition

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(5, 4, 3, 2, 1, 0);

#define BUTTON_A 7
#define BUTTON_B 8
#define RED 9
#define GREEN 10
#define BLUE 11
#define INDICATOR 13
#define BUZZER 12

// define variables
int redValue;
int greenValue;
int blueValue;
int colorSelect;
int firstTime; // Initializes for a first loop sequence
bool inGame;
int scoreCount;
int highScore;
int delayTime;
int gameSequence[maxScore];

void setup()
{
  //RGB Setup
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);

  //Button Setup
  pinMode(INDICATOR, OUTPUT);
  pinMode(BUTTON_A, INPUT_PULLUP);  
  pinMode(BUTTON_B, INPUT_PULLUP);  

  //Buzzer Setup
  pinMode(BUZZER, OUTPUT);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  firstTime = 0;
  highScore = 0;
  scoreCount = 0;
}

// main loop
void loop()
{
  delayTime = 1000;
  lcd.setCursor(0, 1);

  if (firstTime == 0){
      firstTime = 1;
      startUpSequence();  
    }
  
  if (digitalRead(BUTTON_A) == LOW)
  {
    newGame();
    while (inGame)
    {
      inGame = listenToSequence(gameSequence,scoreCount,delayTime);
      inGame = playSequence(gameSequence,scoreCount,delayTime);
      if(inGame) 
      {
        scoreCount++;
        delayTime = delayTime * 0.9;
      }
    }
    endGame();
  } else if (digitalRead(BUTTON_B) == LOW){
    turnRainbow();
    turnRainbow();
  } else {
    turnAllOff();
    if (scoreCount > highScore){
      highScore = scoreCount;
    }
    lcd.setCursor(0,1);
    lcd.print("High Score: ");
    lcd.print(highScore);    
  }

}

/*
* Title: New Game
*
* Function: Initializes in game flag, score, and delay timer. Turns on an in game LED indicator and clears the bottom line of the LCD.
*
* Returns: None
*
*/
void newGame()
{
  inGame = true;
  scoreCount = 0;
  delayTime = 1000;
  digitalWrite(INDICATOR, HIGH);
  clearLCDBottomLine();
}

/*
* Title: End Game
*
* Function: Turns off the in game LED indicator and displays the score on the bottom line of the LCD. Plays a failure tone.
*
* Returns: None
*
*/
void endGame()
{
  lcd.setCursor(0,1);
  lcd.print("Score: ");
  lcd.print(scoreCount);
  digitalWrite(INDICATOR, LOW);
  tone(BUZZER,NOTE_C3,75);
  delay(125);
  tone(BUZZER,NOTE_C3,75);
  delay(2000);
}

/*
* Title: Listen To Sequence
*
* Function: Adds a new color to the sequence. Plays the sequence to the user. Stops the game if the stop button is pressed. Passes in the sequence, score, and delay timer.
*
* Returns: False if stop button is pressed. Otherwise true.
*
*/
bool listenToSequence(int sequence[], int count, int delayTime)
{
  sequence[count] = random(4);
  for (int i = 0; i <= count; i ++)
  {
    switch (sequence[i])
    {
    case 0:
      turnRed();
      break;
    case 1:
      turnGreen();
      break;
    case 2:
      turnBlue();
      break;
    case 3:
      turnAllOn();
      break;
    }
    if (!watchTimer(delayTime,sequence[i]))
    {
      return false;
    }
  // 100 millisecond delay between colors in sequence
    turnAllOff();
    delay(100);
  }
  return true;
}

/*
* Title: Play Sequence
*
* Function: The user plays the sequence on the joystick.
*
* Returns: True if the user plays the sequence accurately. Otherwise returns false.
*
*/
bool playSequence(int sequence[], int count, int delayTime)
{
  for (int i = 0; i <= count; i ++)
  {
    if (!watchJoystickTimer(delayTime,sequence[i])){
      return false;
    }
    delay(50);
  }
  turnAllOff();
  delay(100);
  return true;
}

/*
* Title: Clear LCD Top Line
*
* Function: Sets the top line of the LCD to all spaces.
*
* Returns: None
*
*/
void clearLCDTopLine()
{
  lcd.setCursor(0,0);
    lcd.print("                ");
}

/*
* Title: Clear LCD Bottom Line
*
* Function: Sets the bottom line of the LCD to all spaces.
*
* Returns: None
*
*/
void clearLCDBottomLine()
{
  lcd.setCursor(0,1);
    lcd.print("                ");
}

/*
* Title: Start Up Sequence
*
* Function: Plays a sequence when the board resets.
*
* Returns: None
*
*/
void startUpSequence()
{
  lcd.setCursor(0,0);
  lcd.print("RGB Colors!");
  turnRainbow();
  turnRainbow();
  turnAllOff();
  delay(500);
  turnRed();
  tone(BUZZER,NOTE_C5,200);
  delay(200);
  turnAllOff();
  delay(200);
  turnGreen();
  tone(BUZZER,NOTE_E5,200);
  delay(200);
  turnAllOff();
  delay(200);
  turnBlue();
  tone(BUZZER,NOTE_G5,200);
  delay(200);
  turnAllOff();
  delay(200);
  turnAllOn();
  tone(BUZZER,NOTE_C6,200);
  delay(200);
  turnAllOff();
}

/*
* Title: Watch Timer
*
* Function: Watches for the stop button to be pressed. Passes in the duration of time and the note to play during the timer.
*
* Returns: Returns false if the stop button is pressed. Otherwise true.
*
*/
bool watchTimer(int watchTime, int noteSelect)
{
  int waitTime = 0;
  while ( waitTime < watchTime)
  {
    if (digitalRead(BUTTON_B) == LOW) 
    {
      turnAllOff();
      return false;
    }
    playNote(noteSelect);
    delay(5);
    waitTime += 5;
  }
  return true;
}

/*
* Title: Watch Joystick Timer
*
* Function: Watches for the joystick to be moved in the same direction as the note being played. Passes in the duration of time and the note to play during the timer.
*
* Returns: Returns false if the joystick is not moved in time or moved in the wrong direction. Returns true if it's moved in the right direction.
*
*/
bool watchJoystickTimer(int watchTime, int noteSelect)
{
  int waitTime = 0;
  bool failed = false;
  bool result = false;
  while ( waitTime < watchTime)
  {
    if (getJoystickQuadrant() == noteSelect)
    {
      playNote(noteSelect);
      result = true;
    } 
    else if (getJoystickQuadrant() != 4) //no note selected
    {
      if (failed){
        return false;
      }
      delay(50);
      failed = true;
    } else if (result == true)
    {
      if (getJoystickQuadrant() == 4){
        delay(50);
        return true;
      }
    }
    delay(5);
    waitTime += 5;
  }
  return false;
}

/* int digitalReadOutputPin(uint8_t pin)
{
 uint8_t bit = digitalPinToBitMask(pin);
 uint8_t port = digitalPinToPort(pin);
 if (port == NOT_A_PIN)
   return LOW;

 return (*portOutputRegister(port) & bit) ? HIGH : LOW;
} 

void toggleBuzzer()
{
  if (digitalReadOutputPin(BUZZER) == HIGH){
    digitalWrite(BUZZER,LOW);
  } else {
    digitalWrite(BUZZER,HIGH);
  }
} */

/*
* Title: Play Note
*
* Function: Plays one of four pre-selected buzzer tones for ten milliseconds.
*
* Returns: None
*
*/
void playNote(int noteSelect)
{
  switch (noteSelect)
  {
    case 0:
      tone(BUZZER,NOTE_C5,10);
      break;
    case 1:
      tone(BUZZER,NOTE_E5,10);
      break;
    case 2:
      tone(BUZZER,NOTE_G5,10);
      break;
    case 3:
      tone(BUZZER,NOTE_C6,10);
      break;
  }
}

/*
* Title: Turn All On
*
* Function: Turns the RGB LED to white and writes "White" on the bottom line of the LCD.
*
* Returns: None
*
*/
void turnAllOn()
{
  redValue = 255;
  greenValue = 255;
  blueValue = 255;

  analogWrite(RED, redValue);
  analogWrite(GREEN, greenValue);
  analogWrite(BLUE, blueValue);
  lcd.setCursor(0,1);
  lcd.print("White");
}

/*
* Title: Turn All Off
*
* Function: Turns the RGB LED off.
*
* Returns: None
*
*/
void turnAllOff()
{
  redValue = 0;
  greenValue = 0;
  blueValue = 0;

  analogWrite(RED, redValue);
  analogWrite(GREEN, greenValue);
  analogWrite(BLUE, blueValue);
}

/*
* Title: Turn Red
*
* Function: Turns the RGB LED to red and writes "Red  " on the bottom line of the LCD.
*
* Returns: None
*
*/
void turnRed()
{
  redValue = 255;
  greenValue = 0;
  blueValue = 0;

  analogWrite(RED, redValue);
  analogWrite(GREEN, greenValue);
  analogWrite(BLUE, blueValue);
  lcd.setCursor(0,1);
  lcd.print("Red  ");
}

/*
* Title: Turn Blue
*
* Function: Turns the RGB LED to blue and writes "Blue " on the bottom line of the LCD.
*
* Returns: None
*
*/
void turnBlue()
{
  redValue = 0;
  greenValue = 0;
  blueValue = 255;

  analogWrite(RED, redValue);
  analogWrite(GREEN, greenValue);
  analogWrite(BLUE, blueValue);
  lcd.setCursor(0,1);
  lcd.print("Blue ");
}

/*
* Title: Turn Green
*
* Function: Turns the RGB LED to green and writes "Green" on the bottom line of the LCD.
*
* Returns: None
*
*/
void turnGreen()
{
  redValue = 0;
  greenValue = 255;
  blueValue = 0;

  analogWrite(RED, redValue);
  analogWrite(GREEN, greenValue);
  analogWrite(BLUE, blueValue);
  lcd.setCursor(0,1);
  lcd.print("Green");
}


/*
* Title: Turn Rainbow
*
* Function: Cycles through a rainbow of colors on the RGB LED.
*
* Returns: None
*
*/
void turnRainbow()
{
  delayTime = 1; // fading time between colors

  redValue = 255; // choose a value between 1 and 255 to change the color.
  greenValue = 0;
  blueValue = 0;


  for (int i = 0; i < 255; i += 1) // fades out red into green full when i=255
  {
    redValue -= 1;
    greenValue += 1;
    analogWrite(RED, redValue);
    analogWrite(GREEN, greenValue);
    delay(delayTime);
  }

  redValue = 0;
  greenValue = 255;
  blueValue = 0;

  for (int i = 0; i < 255; i += 1) // fades out green into blue full when i=255
  {
    greenValue -= 1;
    blueValue += 1;
    analogWrite(GREEN, greenValue);
    analogWrite(BLUE, blueValue);
    delay(delayTime);
  }

  redValue = 0;
  greenValue = 0;
  blueValue = 255;

  for (int i = 0; i < 255; i += 1) // fades out blue into red full when i=255
  {
    blueValue -= 1;
    redValue += 1;
    analogWrite(BLUE, blueValue);
    analogWrite(RED, redValue);
    delay(delayTime);
  }
}

/*
* Title: Get Joystick Quadrant
*
* Function: Reads the joystick input. Must be beyond the threshhold in a given direction and between the thresholds in the perpendicular direction.
*
* Returns: 
* 
* 0 for North
* 1 for East
* 2 for South
* 3 for West
* 4 for none of the other quadrants.
*
*/
int getJoystickQuadrant()
{
  int x = analogRead(X_pin);
  int y = analogRead(Y_pin);

  const int x_mid = 509;
  const int y_mid = 496;
  const int x_max = 1023;
  int x_east = (x_mid + x_max) / 2;
  int x_west = x_mid / 2;
  int y_south = (y_mid + x_max) / 2;
  int y_north = y_mid / 2;

  //north
  if ((y < y_north)&&(x > x_west)&&(x < x_east)){
    return 0;
  }
  //east
  if ((x > x_east)&&(y < y_south)&&(y > y_north)){
    return 1;
  }
  //south
  if ((y > y_south)&&(x > x_west)&&(x < x_east)){
    return 2;
  }
  //west
  if ((x < x_west)&&(y < y_south)&&(y > y_north)){
    return 3;
  }
  //none
  return 4; 
}
