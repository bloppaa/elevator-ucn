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
char requestedFloor = -1;

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

    Serial.print("Response: ");
    Serial.println(message);

    if (message.charAt(0) == 'M' || message.charAt(0) == 'I')
    {
      bool otherIsMoving = message.charAt(0) == 'M' ? true : false;
      int otherCurrentFloor = message.charAt(1) - '0';

      if (!isMoving && otherIsMoving)
      {
        floorQueue.enqueue(requestedFloor - '0');
        elevatorSerial.println("N");
      }
      else if (isMoving && !otherIsMoving)
      {
        elevatorSerial.println("Y");
      }
      else
      {
        int requested = requestedFloor - '0';
        int d1 = abs(requested - currentFloor);
        int d2 = abs(requested - otherCurrentFloor);
        if (d1 < d2)
        {
          elevatorSerial.println("N");
        }
        else
        {
          elevatorSerial.println("Y");
        }
      }
    }
    else if (message.charAt(0) == 'Y')
    {
      floorQueue.enqueue(requestedFloor - '0');
    }
    else if (message.charAt(0) == 'N')
    {
      ; // Do nothing
    }
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
      requestedFloor = (customKey - 'a' + 1) + '0';

      elevatorSerial.print("R");
      elevatorSerial.println(requestedFloor);
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
