#include <Keypad.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Queue.h"

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'1', '2', '3'},
  {'4', '5', '6'},
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8,};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const float HEIGHTS[] = {0, 2038, 2038, 2038, 2038, 2038};

AccelStepper myStepper(AccelStepper::FULL4WIRE, 9, 11, 10, 12);

LiquidCrystal_I2C lcd(0x27, 16, 2);

Queue<int> floorQueue(6);

int currentFloor = 1;
int targetFloor = -1;
bool isMoving = false;
bool isGoingUp = false;

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  myStepper.setMaxSpeed(750);
  myStepper.setAcceleration(500);
  lcd.setCursor(0, 0);
  lcd.print("Esperando");
  lcd.setCursor(0, 1);
  lcd.print("Piso ");
  lcd.print(currentFloor);
}

void loop()
{
  char customKey = customKeypad.getKey();
  if (customKey)
  {
    int requestedFloor = customKey - '0';
    if (requestedFloor >= 1 && requestedFloor <= 6 && requestedFloor != currentFloor)
    {
      floorQueue.enqueue(requestedFloor);
    }
  }

  if (!isMoving && !floorQueue.isEmpty())
  {
    targetFloor = floorQueue.dequeue();
    if (targetFloor != currentFloor)
    {
      isMoving = true;
      isGoingUp = targetFloor > currentFloor;
      lcd.setCursor(0, 0);
      lcd.print(isGoingUp ? "Subiendo     " : "Bajando      ");
    }
  }

  if (isMoving)
  {
    if (currentFloor < targetFloor)
    {
      myStepper.moveTo(myStepper.currentPosition() + HEIGHTS[(int)currentFloor]);
      myStepper.runToPosition();
      currentFloor++;
    }
    else if (currentFloor > targetFloor)
    {
      myStepper.moveTo(myStepper.currentPosition() - HEIGHTS[(int)currentFloor - 1]);
      myStepper.runToPosition();
      currentFloor--;
    }

    lcd.setCursor(0, 1);
    lcd.print("Piso ");
    lcd.print(currentFloor);
    lcd.print("   ");

    if (currentFloor == targetFloor)
    {
      isMoving = false;
      lcd.setCursor(0, 0);
      lcd.print("Esperando     ");
      lcd.setCursor(0, 1);
      lcd.print("Piso ");
      lcd.print(currentFloor);
      delay(500);
    }
  }
}
