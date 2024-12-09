#include <Keypad.h>
#include <Stepper.h>
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

const float stepsPerRevolution = 2038;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

const float HEIGHTS[] = {0, 8000, 6000, 3000, 3000, 3000};

float currentFloor = 1;

Queue<int> floorQueue(6);

int totalSteps = 0;
int stepsMoved = 0;
bool isMoving = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  Serial.begin(9600);
  myStepper.setSpeed(15);
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
      stepsMoved = 0;
      isMoving = true;
      currentFloor = targetFloor;
    }
  }

  if (isMoving)
  {
    if (stepsMoved < abs(totalSteps))
    {
      int stepDirection = totalSteps > 0 ? 1 : -1;
      myStepper.step(stepDirection);
      stepsMoved++;
    }
    else
    {
      isMoving = false;
      Serial.println("Reached floor.");

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
