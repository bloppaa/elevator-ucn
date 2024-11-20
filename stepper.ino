// Includes the Arduino Stepper Library
#include <Stepper.h>

// Defines the number of steps per rotation
const float stepsPerRevolution = 2038;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 2, 4, 3, 5);

void setup()
{
    // Nothing to do (Stepper Library sets pins as outputs)
}

void loop()
{
  float degrees = stepsPerRevolution / 360 * 180;

  // Rotate CW slowly at 5 RPM
  myStepper.setSpeed(10);
  myStepper.step(degrees);
  delay(1000);

  // Rotate CCW quickly at 10 RPM
  myStepper.setSpeed(10);
  myStepper.step(-degrees);
  delay(1000);
}