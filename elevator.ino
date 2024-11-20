// #include <Keypad.h>
// #include <Servo.h>

// const byte ROWS = 2;
// const byte COLS = 3;

// char hexaKeys[ROWS][COLS] = {
//     {'1', '2', '3'},
//     {'4', '5', '6'}};

// byte rowPins[ROWS] = {2, 3};
// byte colPins[COLS] = {4, 5, 6};

// Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Servo servo;
// int currentPos = 0;
// unsigned long lastServoUpdate = 0;
// int targetPos = 0;

// const int QUEUE_SIZE = 6;
// int queue[QUEUE_SIZE];
// int front = 0;
// int rear = -1;
// int itemCount = 0;

// int currentFloor = 1;
// const int FLOOR_HEIGHT = 30;

// bool delayAtDestination = false;
// unsigned long destinationDelayStart = 0;
// const unsigned long destinationDelay = 2000;

// bool isQueueFull()
// {
//     return itemCount == QUEUE_SIZE;
// }

// bool isQueueEmpty()
// {
//     return itemCount == 0;
// }

// bool isInQueue(int data)
// {
//     for (int i = 0; i < itemCount; i++)
//     {
//         if (queue[(front + i) % QUEUE_SIZE] == data)
//         {
//             return true;
//         }
//     }
//     return false;
// }

// void enqueue(int data)
// {
//     if (!isQueueFull())
//     {
//         if (rear == QUEUE_SIZE - 1)
//         {
//             rear = -1;
//         }
//         queue[++rear] = data;
//         itemCount++;
//     }
// }

// int dequeue()
// {
//     int data = queue[front++];
//     if (front == QUEUE_SIZE)
//     {
//         front = 0;
//     }
//     itemCount--;
//     return data;
// }

// void printQueue()
// {
//     for (int i = 0; i < itemCount; i++)
//     {
//         Serial.print(queue[(front + i) % QUEUE_SIZE]);
//         if (i < itemCount - 1)
//         {
//             Serial.print(", ");
//         }
//     }
//     Serial.println();
// }

// void moveServoNonBlocking()
// {
//     unsigned long currentMillis = millis();
//     if (currentMillis - lastServoUpdate >= 15)
//     {
//         if (currentPos < targetPos)
//         {
//             currentPos++;
//             servo.write(currentPos);
//         }
//         else if (currentPos > targetPos)
//         {
//             currentPos--;
//             servo.write(currentPos);
//         }
//         else
//         {
//             if (!delayAtDestination)
//             {
//                 delayAtDestination = true;
//                 destinationDelayStart = currentMillis;
//             }
//         }
//         lastServoUpdate = currentMillis;
//     }
// }

// void setup()
// {
//     Serial.begin(9600);
//     servo.attach(7);
//     servo.write(0);
// }

// void loop()
// {
//     char customKey = customKeypad.getKey();
//     if (customKey)
//     {
//         int customKeyInt = customKey - '0';
//         if (!isInQueue(customKeyInt) && !isQueueFull() && customKeyInt != currentFloor)
//         {
//             enqueue(customKeyInt);
//             printQueue();
//         }
//         delay(200);
//     }

//     if (delayAtDestination)
//     {
//         unsigned long currentMillis = millis();
//         if (currentMillis - destinationDelayStart >= destinationDelay)
//         {
//             delayAtDestination = false;
//             if (!isQueueEmpty())
//             {
//                 int nextFloor = dequeue();
//                 targetPos = (nextFloor - 1) * FLOOR_HEIGHT;
//                 currentFloor = nextFloor;
//             }
//         }
//         return;
//     }

//     moveServoNonBlocking();
// }
