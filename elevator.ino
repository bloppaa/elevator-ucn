#include <Keypad.h>
#include <Stepper.h>

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

// Alturas entre pisos
const float HEIGHTS[] = {0, 8000, 6000, 3000, 3000, 3000}; // Índice 0 no se usa

float currentFloor = 5;

void setup()
{
    Serial.begin(9600);
    myStepper.setSpeed(15); // Configura la velocidad del motor
}

void loop()
{
    char customKey = customKeypad.getKey();
    if (customKey)
    {
        int customKeyInt = customKey - '0'; // Convierte el carácter en entero

        if (customKeyInt >= 1 && customKeyInt <= 6 && customKeyInt != currentFloor)
        {
            Serial.print("Moviendo al piso: ");
            Serial.println(customKeyInt);

            // Calcula los pasos necesarios para llegar al piso objetivo
            float stepsToMove = 0;

            if (customKeyInt > currentFloor)
            {
                // Moviéndose hacia arriba
                for (int i = currentFloor; i < customKeyInt; i++)
                {
                    stepsToMove += HEIGHTS[i];
                }
            }
            else
            {
                // Moviéndose hacia abajo
                for (int i = currentFloor - 1; i >= customKeyInt; i--)
                {
                    stepsToMove -= HEIGHTS[i];
                }
            }

            // Mueve el motor según los pasos calculados
            myStepper.step(-stepsToMove);

            // Actualiza el piso actual
            currentFloor = customKeyInt;

            // Detente un momento al llegar al piso
            delay(500);
        }
    }
}
