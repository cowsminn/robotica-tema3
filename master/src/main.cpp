#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Wire.h>

LiquidCrystal lcd(9, 8, 5, 4, 3, 2);
Servo cronometruServo;

const int pinButonStart = 7;
const int pinServo = 6;
const int servoUnghiMaxim = 180;
const unsigned long timpTotalJoc = 150000;  // 30 secunde

char culori[] = {'r', 'g', 'b'};
unsigned long timpStartJoc = 0;
unsigned long timpUltimaRunda = 0;
unsigned long timpRunda = 5000; 
bool jocInceput = false;
bool asteaptaRaspuns = false;
char raspuns;

String jucator1, jucator2;
int jucator1scor = 0;
int jucator2scor = 0;
int cronometruRunda = 0;
bool randJucator1 = true;
// Sends a random color command ('r', 'g', 'b') to the SPI slave
void sendColorCommand();

// Updates the player's score based on the response received from the SPI slave
void scorNou(char scor);

// Sends a command via SPI and receives a response
char sendCommand(char command);

// Displays the final scores and announces the winner on the LCD
void displayWinner();

// Resets the game state and prepares for a new game session
void resetGame();

void setup() {
  Serial.begin(28800);
  SPI.begin();
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  pinMode(pinButonStart, INPUT_PULLUP);

  lcd.begin(16, 2);
  cronometruServo.attach(pinServo);

  lcd.print("Incepe jocul!");
}

void loop() {
  if (!jocInceput && digitalRead(pinButonStart) == LOW) {
    delay(200);
    jocInceput = true;
    lcd.clear();
    lcd.print("Nume jucator 1:");
    while (Serial.available() == 0) {}
    jucator1 = Serial.readStringUntil('\n'); 

    lcd.clear();
    lcd.print("Nume jucator 2:");
    while (Serial.available() == 0) {}
    jucator2 = Serial.readStringUntil('\n');

    timpStartJoc = millis();
  }

  if (jocInceput) {
    unsigned long millisCurent = millis();

    // Daca au trecut 30 de secunde jocul s-a terminat
    if (millisCurent - timpStartJoc >= timpTotalJoc) {
      jocInceput = false;
      displayWinner();
      resetGame();
      return;
    }

    // Roteste servo-ul
    int angle = map(millisCurent - timpStartJoc, 0, timpTotalJoc, 0, servoUnghiMaxim);
    cronometruServo.write(angle);

    // Randul celuilalt jucator dupa cate 1 secunda
    if (millisCurent - timpUltimaRunda >= timpRunda) {
      if (cronometruRunda < 30) {  // 15 runde fiecare jucator
        if (randJucator1) {
          lcd.clear();
          lcd.print("Runda " + jucator1);
          sendColorCommand();
        } else {
          lcd.clear();
          lcd.print("Runda " + jucator2);
          sendColorCommand();
        }
        delay(1000);
        asteaptaRaspuns = true;
        timpUltimaRunda = millisCurent;
        randJucator1 = !randJucator1;
        cronometruRunda++;
      }
    }

    // Primeste si proceseaza raspunsul de la slave
    if (asteaptaRaspuns) {
      raspuns = sendCommand('#');
      Serial.println((randJucator1 ? jucator2 : jucator1) + " scor: ");
      Serial.println(raspuns);

      scorNou(raspuns);
      asteaptaRaspuns = false;
    }
  }
  Serial.println();
}

// Functie care trimite culoarea prin SPI
void sendColorCommand() {
  char colorCommand = culori[random(0, 3)];
  sendCommand(colorCommand);
  Serial.print("Sent color: ");
  Serial.println(colorCommand);
}

// Noul scor in functie de raspunsul jucatorului
void scorNou(char scor) {
  int puncte = 0;
  switch (scor) {
    case 'e': puncte = 50; break;
    case 'g': puncte = 25; break;
    case 'm': puncte = 10; break;
    case 'i': puncte = 0; break;
  }

  if (!randJucator1) {
    jucator1scor += puncte;
  } else {
    jucator2scor += puncte;
  }
}

char sendCommand(char command) {
  digitalWrite(SS, LOW);
  char response;
  do {
    SPI.transfer(command); // Send the command
    delay(10);             // Give the slave some time to process
    response = SPI.transfer("#"); // Check for the response
  } while (response == '$'); // Wait until a valid response is received
  digitalWrite(SS, HIGH);
  return response;
}

// Afiseaza scorul final si castigatorul jocului
void displayWinner() {
  lcd.clear();
  lcd.print(jucator1 + " scor:");
  lcd.setCursor(0, 1);
  lcd.print(jucator1scor);
  delay(2000);

  lcd.clear();
  lcd.print(jucator2 + " scor:");
  lcd.setCursor(0, 1);
  lcd.print(jucator2scor);
  delay(2000);

  lcd.clear();
  if (jucator1scor > jucator2scor) {
    lcd.print("Castigator: " + jucator1);
  } else if (jucator2scor > jucator1scor) {
    lcd.print("Castigator: " + jucator2);
  } else {
    lcd.print("Egalitate!");
  }
  delay(2000);
}

// Reseteaza jocul la starea initiala
void resetGame() {
  jucator1scor = 0;
  jucator2scor = 0;
  cronometruRunda = 0;
  randJucator1 = true;
  lcd.clear();
  lcd.print("Incepe jocul!");
}
