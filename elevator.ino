#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', '4'},
    {'5', '6', '7', '8'},
    {'9', '0', 'A', 'B'},
    {'C', 'D', 'E', 'F'}};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const int QUEUE_SIZE = 6;
char queue[QUEUE_SIZE];
int front = 0;
int rear = -1;
int itemCount = 0;

void enqueue(char data)
{
  if (itemCount != QUEUE_SIZE)
  {
    if (rear == QUEUE_SIZE - 1)
    {
      rear = -1;
    }
    queue[++rear] = data;
    itemCount++;
  }
}

char dequeue()
{
  char data = queue[front++];
  if (front == QUEUE_SIZE)
  {
    front = 0;
  }
  itemCount--;
  return data;
}

bool isQueueFull()
{
  return itemCount == QUEUE_SIZE;
}

void printQueue()
{
  Serial.print("Queue: ");
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

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  char customKey = customKeypad.getKey();

  if (customKey)
  {
    if (customKey >= '1' && customKey <= '6')
    {
      if (!isQueueFull())
      {
        enqueue(customKey);
        Serial.print("Enqueued: ");
        Serial.println(customKey);
        printQueue();
      }
      else
      {
        Serial.println("Queue is full");
      }
    }
    else if (customKey == 'A')
    {
      if (itemCount > 0)
      {
        char dequeuedKey = dequeue();
        Serial.print("Dequeued: ");
        Serial.println(dequeuedKey);
        printQueue();
      }
      else
      {
        Serial.println("Queue is empty");
      }
    }
    else
    {
      Serial.print("Ignored key: ");
      Serial.println(customKey);
    }
  }
}