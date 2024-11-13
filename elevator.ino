#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', '4'},
    {'5', '6', 'A', '.'},
    {'.', '.', '.', '.'},
    {'.', '.', '.', '.'}};

byte rowPins[ROWS] = {2, 3, 0, 0};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

const int QUEUE_SIZE = 6;
char queue[QUEUE_SIZE];
int front = 0;
int rear = -1;
int itemCount = 0;

bool isQueueFull()
{
  return itemCount == QUEUE_SIZE;
}

bool isInQueue(char data)
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
      if (!isInQueue(customKey))
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
      else
      {
        Serial.print("Key ");
        Serial.print(customKey);
        Serial.println(" is already in the queue");
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