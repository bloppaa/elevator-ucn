#include <Keypad.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Queue.h"

const byte ROWS = 2;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'}};

byte rowPins[ROWS] = {2, 3};
byte colPins[COLS] = {4, 5, 6};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const float HEIGHTS[] = {0, 8000, 6000, 3000, 3000, 3000};

float currentFloor = 1;

Queue<int> floorQueue(6);

int totalSteps = 0;
int stepsMoved = 0;
bool isMoving = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int MOTOR_PIN1 = 8;
const int MOTOR_PIN2 = 10;
const int MOTOR_PIN3 = 9;
const int MOTOR_PIN4 = 11;

AccelStepper myStepper(AccelStepper::FULL4WIRE, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  Serial.begin(9600);
  
  myStepper.setMaxSpeed(750);
  myStepper.setAcceleration(400);
  myStepper.setCurrentPosition(0);
}

void loop()
{
  char customKey = customKeypad.getKey();
  if (customKey)
  {
    int requestedFloor = customKey - '0';
    if (requestedFloor >= 1 && requestedFloor <= 6 && requestedFloor != currentFloor)
    {
      Serial.print("Requested: ");
      Serial.println(requestedFloor);

      floorQueue.enqueue(requestedFloor);
      floorQueue.print();
    }
  }

  if (!isMoving && !floorQueue.isEmpty())
  {
    int targetFloor = floorQueue.dequeue();
    floorQueue.print();

    if (targetFloor != currentFloor)
    {
      Serial.print("Moving: ");
      Serial.println(targetFloor);

      if (targetFloor > currentFloor) {
        lcd.clear();
        lcd.print("Subiendo");
      } else {
        lcd.clear();
        lcd.print("Bajando");
      }

      totalSteps = calculateSteps(currentFloor, targetFloor);
      myStepper.moveTo(myStepper.currentPosition() + totalSteps);

      isMoving = true;
      currentFloor = targetFloor;
    }
  }

  if (isMoving)
  {
    myStepper.run();

    if (myStepper.distanceToGo() == 0)
    {
      isMoving = false;
      Serial.println("Reached floor");

      lcd.clear();
      delay(500);
    }
  }
}

float calculateSteps(int startFloor, int endFloor)
{
  float stepsToMove = 0;

  if (endFloor > startFloor)
  {
    for (int i = startFloor; i < endFloor; i++)
    {
      stepsToMove += HEIGHTS[i];
    }
  }
  else
  {
    for (int i = startFloor - 1; i >= endFloor; i--)
    {
      stepsToMove -= HEIGHTS[i];
    }
  }
  return stepsToMove;
}
