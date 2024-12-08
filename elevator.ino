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

// Floor heights (index 0 is unused)
const float HEIGHTS[] = {0, 8000, 6000, 3000, 3000, 3000};

float currentFloor = 1;

Queue<int> floorQueue(6);

// Non-blocking motor control variables
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
    myStepper.setSpeed(15); // Set motor speed
}

void loop()
{
    // Check for keypad input and add floor requests to the queue
    char customKey = customKeypad.getKey();
    if (customKey)
    {
        lcd.clear();
        int requestedFloor = customKey - '0'; // Convert character to integer
        if (requestedFloor >= 1 && requestedFloor <= 6 && requestedFloor != currentFloor)
        {
            lcd.print(customKey);
            Serial.print("Requested: ");
            Serial.println(requestedFloor);

            // Add the floor to the queue if it is not already in the queue
            if (!floorQueue.contains(requestedFloor))
            {
                floorQueue.enqueue(requestedFloor);
            }
        }
    }

    // Process the next floor in the queue
    if (!isMoving && !floorQueue.isEmpty())
    {
        int targetFloor = floorQueue.dequeue(); // Get the next floor from the queue
        if (targetFloor != currentFloor)
        {
            Serial.print("Moving: ");
            Serial.println(targetFloor);

            totalSteps = calculateSteps(currentFloor, targetFloor);
            stepsMoved = 0; // Reset step counter
            isMoving = true; // Begin movement
            currentFloor = targetFloor; // Update current floor
        }
    }

    // Non-blocking motor movement
    if (isMoving)
    {
        if (stepsMoved < abs(totalSteps))
        {
            int stepDirection = totalSteps > 0 ? 1 : -1;
            myStepper.step(stepDirection); // Move one step in the required direction
            stepsMoved++;
        }
        else
        {
            // Movement complete
            isMoving = false;
            Serial.println("Reached floor.");
            delay(500); // Simulate stopping at the floor
        }
    }
}

// Helper function to calculate steps needed between floors
float calculateSteps(int startFloor, int endFloor)
{
    float stepsToMove = 0;

    if (endFloor > startFloor)
    {
        // Moving up
        for (int i = startFloor; i < endFloor; i++)
        {
            stepsToMove += HEIGHTS[i];
        }
    }
    else
    {
        // Moving down
        for (int i = startFloor - 1; i >= endFloor; i--)
        {
            stepsToMove -= HEIGHTS[i];
        }
    }
    return stepsToMove;
}
