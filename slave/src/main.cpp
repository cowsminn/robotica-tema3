#include <Arduino.h>
#include <SPI.h>

// Function prototypes
void initiateButtonChallenge(char selectedColor);
void evaluateButtonPress();
void lightUpLED(char selectedColor);
void turnOffAllLEDs();

volatile char spiCommand = '\0';
volatile bool newSPICommand = false;

// Pin definitions
const int analogButtonPin = A0;
const int buzzerControlPin = 2;
const int commonSmallLEDPin = 3;

// Player LED pins
const int p1RedLEDPin = 4;
const int p1GreenLEDPin = 5;
const int p1BlueLEDPin = 6;
const int p2RedLEDPin = 7;
const int p2GreenLEDPin = 8;
const int p2BlueLEDPin = 9;

// Game state variables
unsigned long buttonPressStartTime = 0;
bool isWaitingForButtonPress = false;
bool isPlayerOneActive = true;

void setup() {
  Serial.begin(28800);         // Debugging
  SPCR |= bit(SPE);            // Enable SPI in slave mode
  pinMode(MISO, OUTPUT);       // Set MISO as output for SPI
  SPI.attachInterrupt();       // Enable SPI interrupt

  // Configure pins
  pinMode(buzzerControlPin, OUTPUT);
  pinMode(commonSmallLEDPin, OUTPUT);
  pinMode(p1RedLEDPin, OUTPUT);
  pinMode(p1GreenLEDPin, OUTPUT);
  pinMode(p1BlueLEDPin, OUTPUT);
  pinMode(p2RedLEDPin, OUTPUT);
  pinMode(p2GreenLEDPin, OUTPUT);
  pinMode(p2BlueLEDPin, OUTPUT);

  digitalWrite(commonSmallLEDPin, HIGH); // Small LED active during the game
}

// SPI interrupt
ISR(SPI_STC_vect) {
  char receivedData = SPDR;
  if (receivedData != '#') { // Ignore the '#' command
    spiCommand = receivedData;
    newSPICommand = true;
  }
}

void loop() {
  if (newSPICommand) {
    newSPICommand = false;

    // Validate the received command
    if (spiCommand == 'r' || spiCommand == 'g' || spiCommand == 'b') {
      initiateButtonChallenge(spiCommand);
    } else {
      SPDR = '$'; // Send default response for unknown commands
    }
  }

  if (isWaitingForButtonPress) {
    evaluateButtonPress();
  }
}

void initiateButtonChallenge(char selectedColor) {
  Serial.print("Received command: ");
  Serial.print(spiCommand);
  Serial.println(isPlayerOneActive ? " for player 1" : " for player 2");

  buttonPressStartTime = millis();
  isWaitingForButtonPress = true;
  lightUpLED(selectedColor);
}

void evaluateButtonPress() {
  unsigned long elapsedTime = millis() - buttonPressStartTime;
  int buttonReadValue = analogRead(analogButtonPin);
  Serial.print("Button value: ");
  Serial.println(buttonReadValue);

  bool isButtonPressed = false;

  // Ignore noise around the "no button pressed" state
  if ( buttonReadValue <= 50) {
    return; // No button is pressed, exit the function
  }

  // Check button press for each player
  if (isPlayerOneActive) {
    if ((spiCommand == 'b' && buttonReadValue >= 120 && buttonReadValue <= 140) ||
        (spiCommand == 'g' && buttonReadValue >= 85 && buttonReadValue <= 100) ||
        (spiCommand == 'r' && buttonReadValue >= 955 && buttonReadValue <= 970)) {
      isButtonPressed = true;
    }
  } else {
    if ((spiCommand == 'b' && buttonReadValue >= 80 && buttonReadValue <= 90) ||
        (spiCommand == 'g' && buttonReadValue >= 935 && buttonReadValue <= 945) ||
        (spiCommand == 'r' && buttonReadValue >= 970 && buttonReadValue <= 980)) {
      isButtonPressed = true;
    }
  }

  if (isButtonPressed) {
    isWaitingForButtonPress = false;
    char scoreGrade;

    // Determine score based on response time
    if (elapsedTime <= 300) {
      scoreGrade = 'e'; // Excellent
    } else if (elapsedTime <= 600) {
      scoreGrade = 'g'; // Good
    } else if (elapsedTime <= 1000) {
      scoreGrade = 'm'; // Mediocre
    } else {
      scoreGrade = 'i'; // Invalid
    }

    SPDR = scoreGrade; // Send score through SPI
    if (scoreGrade != 'i') {
      tone(buzzerControlPin, 659, 200); // Emit sound for valid scores
    }
    isPlayerOneActive = !isPlayerOneActive; // Switch active player
  } else if (elapsedTime > 900) { // Timeout
    isWaitingForButtonPress = false;
    SPDR = 'i'; // Send timeout response
    isPlayerOneActive = !isPlayerOneActive; // Switch active player
  }
}

void lightUpLED(char selectedColor) {
  turnOffAllLEDs();
  int targetLEDPin;

  // Select LED based on the player and color
  if (isPlayerOneActive) {
    targetLEDPin = (selectedColor == 'r') ? p1RedLEDPin :
                   (selectedColor == 'g') ? p1GreenLEDPin :
                   p1BlueLEDPin;
    Serial.print("Lighting Player 1 LED: ");
  } else {
    targetLEDPin = (selectedColor == 'r') ? p2RedLEDPin :
                   (selectedColor == 'g') ? p2GreenLEDPin :
                   p2BlueLEDPin;
    Serial.print("Lighting Player 2 LED: ");
  }

  Serial.println(targetLEDPin);
  digitalWrite(targetLEDPin, HIGH);
}

void turnOffAllLEDs() {
  // Turn off all LEDs
  digitalWrite(p1RedLEDPin, LOW);
  digitalWrite(p1GreenLEDPin, LOW);
  digitalWrite(p1BlueLEDPin, LOW);
  digitalWrite(p2RedLEDPin, LOW);
  digitalWrite(p2GreenLEDPin, LOW);
  digitalWrite(p2BlueLEDPin, LOW);
}
