#include <Arduino.h>
#include <SPI.h>

// Function prototypes
void initiateButtonChallenge(char selectedColor);
void evaluateButtonPress();
void lightUpLED(char selectedColor);
void turnOffAllLEDs();

// SPI communication variables
volatile char spiCommand = '\0';
volatile bool newSPICommand = false;

// Pin definitions
const int analogButtonPinPlayer1 = A0;
const int analogButtonPinPlayer2 = A1;
const int buzzerControlPin = 2;
const int commonSmallLEDPin = 3;

// Player button thresholds
const int buttonThresholdsPlayer1[3] = {280, 550, 770}; // Red, Green, Blue
const int buttonThresholdsPlayer2[3] = {320, 400, 540}; // Red, Green, Blue
const int thresholdTolerance = 10;

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

const unsigned long excellentTime = 1500; // Excellent response time
const unsigned long goodTime = 3000;     // Good response time
const unsigned long mediocreTime = 5000; // Mediocre response time
const unsigned long timeoutTime = 4500;  // Timeout for button press

void setup() {
  Serial.begin(28800);        // Debugging
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

ISR(SPI_STC_vect) {
  char receivedData = SPDR;
  if (receivedData != '#') { // Ignore '#' command
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
      SPDR = '$';
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
  int buttonReadValue = analogRead(isPlayerOneActive ? analogButtonPinPlayer1 : analogButtonPinPlayer2);

  // Get the correct thresholds for the current player
  const int* thresholds = isPlayerOneActive ? buttonThresholdsPlayer1 : buttonThresholdsPlayer2;

  // Determine the color index for the expected color
  int expectedIndex = (spiCommand == 'r') ? 0 : (spiCommand == 'g') ? 1 : 2;
  int expectedThreshold = thresholds[expectedIndex];

  // Check if the button press matches the correct threshold
  if (abs(buttonReadValue - expectedThreshold) <= thresholdTolerance) {
    // Correct button press
    isWaitingForButtonPress = false;
    char scoreGrade;

    // Determine score based on response time
    if (elapsedTime <= excellentTime) {
      Serial.println("Excellent");
      scoreGrade = 'e'; // Excellent
    } else if (elapsedTime <= goodTime) {
      Serial.println("Good");
      scoreGrade = 'g'; // Good
    } else if (elapsedTime <= mediocreTime) {
      Serial.println("Mediocre");
      scoreGrade = 'm'; // Mediocre
    } else {
      Serial.println("Too Late");
      scoreGrade = 'i'; // Invalid (too late)
    }

    SPDR = scoreGrade; // Send score through SPI
    if (scoreGrade != 'i') {
      tone(buzzerControlPin, 659, 200); // Success tone
    }
    isPlayerOneActive = !isPlayerOneActive; // Switch active player
  } else {
    // Check if the button press matches any other threshold
    for (int i = 0; i < 3; i++) {
      if (i != expectedIndex && abs(buttonReadValue - thresholds[i]) <= thresholdTolerance) {
        // Invalid button press
        Serial.println("Wrong Color");
        isWaitingForButtonPress = false;
        SPDR = 'i'; // Send "invalid" response
        tone(buzzerControlPin, 440, 500); // Error tone
        return;
      }
    }
  }

  // Timeout handling
  if (elapsedTime > timeoutTime) {
    Serial.println("Timeout");
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
  } else {
    targetLEDPin = (selectedColor == 'r') ? p2RedLEDPin :
                   (selectedColor == 'g') ? p2GreenLEDPin :
                   p2BlueLEDPin;
  }

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
