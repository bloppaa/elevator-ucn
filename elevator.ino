#include <Keypad.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Queue.h"
#include <SoftwareSerial.h>

SoftwareSerial elevatorSerial(A0, A1);

const byte ROWS = 4;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'a', 'b', 'c'},
    {'d', 'e', 'f'}};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8};

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
  Serial.begin(115200);
  elevatorSerial.begin(9600);

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
  if (elevatorSerial.available())
  {
    String message = elevatorSerial.readStringUntil('\n');
    Serial.println(message);
  }

  char customKey = customKeypad.getKey();
  if (customKey)
  {
    if (customKey >= '1' && customKey <= '6')
    {
      int requestedFloor = customKey - '0';
      if (requestedFloor >= 1 && requestedFloor <= 6 &&
          requestedFloor != currentFloor &&
          !floorQueue.isFull() &&
          !floorQueue.contains(requestedFloor))
      {
        floorQueue.enqueue(requestedFloor);
        // Serial.print("Floor added to queue: ");
        // Serial.println(requestedFloor);
      }
    }
    else if (customKey >= 'a' && customKey <= 'f')
    {
      char requestedFloor;
      switch (customKey)
      {
      case 'a':
        requestedFloor = '1';
        break;
      case 'b':
        requestedFloor = '2';
        break;
      case 'c':
        requestedFloor = '3';
        break;
      case 'd':
        requestedFloor = '4';
        break;
      case 'e':
        requestedFloor = '5';
        break;
      case 'f':
        requestedFloor = '6';
        break;
      }

      String message = "R" + String(requestedFloor);
      elevatorSerial.print(message);
    }
  }

  if (!isMoving && targetFloor == -1 && !floorQueue.isEmpty())
  {
    targetFloor = floorQueue.dequeue();
    // Serial.print("Dequeued target floor: ");
    // Serial.println(targetFloor);
    isGoingUp = targetFloor > currentFloor;
    isMoving = true;
    lcd.setCursor(0, 0);
    lcd.print(isGoingUp ? "Subiendo     " : "Bajando      ");
    myStepper.moveTo(myStepper.currentPosition() + (isGoingUp ? HEIGHTS[currentFloor] : -HEIGHTS[currentFloor - 1]));
  }

  if (isMoving)
  {
    myStepper.run();
    if (myStepper.distanceToGo() == 0)
    {
      currentFloor += isGoingUp ? 1 : -1;
      lcd.setCursor(0, 1);
      lcd.print("Piso ");
      lcd.print(currentFloor);
      lcd.print("   ");
      if (currentFloor == targetFloor)
      {
        isMoving = false;
        targetFloor = -1;
        lcd.setCursor(0, 0);
        lcd.print("Esperando     ");
        delay(500);
      }
      else
      {
        myStepper.moveTo(myStepper.currentPosition() + (isGoingUp ? HEIGHTS[currentFloor] : -HEIGHTS[currentFloor - 1]));
      }
    }
  }
}
