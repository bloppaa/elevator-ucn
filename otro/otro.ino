#include <SoftwareSerial.h>

SoftwareSerial elevatorSerial(A0, A1);

int currentFloor = 6;
bool isMoving = false;

void setup() {
  Serial.begin(115200);
  elevatorSerial.begin(9600);
}

void loop() {
  if (elevatorSerial.available() > 0) {
    String message = elevatorSerial.readStringUntil('\n');
    
    Serial.print("Received: ");
    Serial.println(message);

    if (message.charAt(0) == 'R') {
      elevatorSerial.print(isMoving ? 'M' : 'I');
      elevatorSerial.println(currentFloor);
    }
  }
}
