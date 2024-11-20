#include <Keypad.h>
#include <Servo.h>

// Keypad Setup
const byte ROWS = 2;
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'}};

byte rowPins[ROWS] = {2, 3};
byte colPins[COLS] = {4, 5, 6};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

Servo servo;
int pos = 0;
unsigned long lastServoUpdate = 0;
int servoDirection = 1;

const int QUEUE_SIZE = 6;
int queue[QUEUE_SIZE];
int front = 0;
int rear = -1;
int itemCount = 0;

bool isQueueFull()
{
    return itemCount == QUEUE_SIZE;
}

bool isInQueue(int data)
{
    for (int i = 0; i < itemCount; i++)
    {
        if (queue[(front + i) % QUEUE_SIZE] == data)
        {
            return true;
        }
    }
    return false;
}

void enqueue(int data)
{
    if (!isQueueFull())
    {
        if (rear == QUEUE_SIZE - 1)
        {
            rear = -1;
        }
        queue[++rear] = data;
        itemCount++;
    }
}

int dequeue()
{
    int data = queue[front++];
    if (front == QUEUE_SIZE)
    {
        front = 0;
    }
    itemCount--;
    return data;
}

void printQueue()
{
    for (int i = 0; i < itemCount; i++)
    {
        Serial.print(queue[(front + i) % QUEUE_SIZE]);
        if (i < itemCount - 1)
        {
            Serial.print(", ");
        }
    }
    Serial.println();
}

void moveServoNonBlocking()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastServoUpdate >= 15)
    {
        pos += servoDirection;
        if (pos >= 180 || pos <= 0)
        {
            servoDirection *= -1;
        }
        servo.write(pos);
        lastServoUpdate = currentMillis;
    }
}

void setup()
{
    Serial.begin(9600);
    servo.attach(7);
}

void loop()
{
    char customKey = customKeypad.getKey();
    if (customKey)
    {
        int customKeyInt = customKey - '0';
        if (!isInQueue(customKeyInt) && !isQueueFull())
        {
            enqueue(customKeyInt);
            printQueue();
        }
    }

    moveServoNonBlocking();
}
