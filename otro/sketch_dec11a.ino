#include <SoftwareSerial.h>

SoftwareSerial elevatorSerial(A0, A1);

int currentFloor = 1;
bool isMoving = false;

void setup() {
  Serial.begin(115200);
  elevatorSerial.begin(9600);
}

void loop() {
  if (elevatorSerial.available()) {
    String message = elevatorSerial.readStringUntil('\n');
    
    Serial.print("Request: ");
    Serial.println(message);

    elevatorSerial.print(isMoving ? 'M' : 'I');
    elevatorSerial.println(currentFloor);
  }
}
