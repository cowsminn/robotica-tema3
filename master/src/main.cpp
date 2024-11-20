#include <Arduino.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Wire.h>

// initializare lcd cu pinii rs, e, d4, d5, d6, d7
const int lcdRS = 9;
const int lcdE = 8;
const int lcdD4 = 5;
const int lcdD5 = 4;
const int lcdD6 = 3;
const int lcdD7 = 2;

LiquidCrystal lcd(lcdRS, lcdE, lcdD4, lcdD5, lcdD6, lcdD7);
Servo gameServo;

// definire pini
const int startButtonPin = 7;
const int servoControlPin = 6;
const int servoMaxPosition = 180;
const unsigned long totalGameTime = 30000; // durata totala a jocului (30 secunde)

// vector cu culorile disponibile
char availableColors[] = {'r', 'g', 'b'};

// variabile de stare ale jocului
unsigned long startGameTime = 0;
unsigned long lastTurnTime = 0;
unsigned long turnInterval = 3000; // interval intre runde
bool gameInProgress = false;
bool waitingForResponse = false;
char receivedResponse;

// nume jucatori si scoruri
String playerOneName, playerTwoName;
int playerOneScore = 0;
int playerTwoScore = 0;
int roundNumber = 0;
bool isPlayerOneTurn = true;

// prototipurile functiilor
void startGame();
void checkGameStatus();
void processTurn();
void displayFinalScores();
void resetGameSettings();
void transmitColorCommand();
char exchangeSPICommand(char command);
void updatePlayerScore(char scoreResponse);
String readPlayerName();

void setup() {
  Serial.begin(28800);
  Serial.println("testttt");
  SPI.begin();
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  pinMode(startButtonPin, INPUT_PULLUP);
  Serial.println("testttt");

  // configurare lcd si servo
  lcd.begin(16, 2);
  gameServo.attach(servoControlPin);
  lcd.print("press to start!");
  Serial.println("testttt");
}

void loop() {
  // porneste jocul cand butonul este apasat
  if (!gameInProgress && digitalRead(startButtonPin) == LOW) {
    delay(200);
    startGame();
  }

  // actualizeaza starea jocului daca acesta este in curs
  if (gameInProgress) {
    checkGameStatus();
  }
}

void startGame() {
  gameInProgress = true;
  lcd.clear();
  lcd.print("enter player 1:");
  Serial.println("enter player 1 name:");

  // citeste numele primului jucator de la serial
  playerOneName = readPlayerName();

  lcd.clear();
  lcd.print("enter player 2:");
  Serial.println("enter player 2 name:");

  // citeste numele celui de-al doilea jucator de la serial
  playerTwoName = readPlayerName();

  // porneste cronometrul pentru joc
  startGameTime = millis();
  lcd.clear();
  lcd.print("game starting!");
  delay(1000);
  lcd.clear();
}

// functie pentru a citi numele unui jucator de la serial
String readPlayerName() {
  String playerName = "";
  while (true) {
    if (Serial.available() > 0) {
      char incomingChar = Serial.read();
      if (incomingChar == '\n') {
        break; // opreste citirea cand se apasa enter
      }
      // verifica daca caracterul este printabil
      if (isPrintable(incomingChar)) {
        playerName += incomingChar;
        lcd.setCursor(playerName.length() - 1, 1); // seteaza cursorul pe lcd
        lcd.print(incomingChar); // afiseaza caracterul pe lcd
      }
    }
  }
  return playerName;
}

void checkGameStatus() {
  unsigned long currentMillis = millis();

  // verifica daca jocul s-a terminat dupa 30 de secunde
  if (currentMillis - startGameTime >= totalGameTime) {
    gameInProgress = false; 
    displayFinalScores();
    resetGameSettings();
    return;
  }

  // actualizeaza pozitia servo-ului pe baza timpului scurs
  int servoAngle = map(currentMillis - startGameTime, 0, totalGameTime, 0, servoMaxPosition);
  gameServo.write(servoAngle);

  // proceseaza o noua runda daca a trecut intervalul de timp
  if (currentMillis - lastTurnTime >= turnInterval) {
    processTurn();
    lastTurnTime = currentMillis;
  }

  // verifica daca s-a primit un raspuns de la dispozitivul slave
  if (waitingForResponse) {
    Serial.println((isPlayerOneTurn ? playerTwoName : playerOneName) + " received score:");
    Serial.println(receivedResponse);
    updatePlayerScore(receivedResponse);
    waitingForResponse = false;
  }
}

void processTurn() {
  // proceseaza runda daca numarul de runde este mai mic de 30
  if (roundNumber < 30) {
    lcd.clear();
    lcd.print((isPlayerOneTurn ? playerOneName : playerTwoName) + " turn");
    transmitColorCommand();
    delay(1000);
    waitingForResponse = true;
    isPlayerOneTurn = !isPlayerOneTurn;
    roundNumber++;
  }
}

void transmitColorCommand() {
  // selecteaza si trimite o comanda cu o culoare aleatorie
  char colorToSend = availableColors[random(0, 3)];
  exchangeSPICommand(colorToSend);
  delay(50); // scurta intarziere pentru stabilitate
  Serial.print("sent color: ");
  Serial.println(colorToSend);
}

char exchangeSPICommand(char command) {
  // trimite comanda si primeste raspunsul prin spi
  digitalWrite(SS, LOW);
  char response = SPI.transfer(command);
  digitalWrite(SS, HIGH);
  delay(50); // intarziere pentru sincronizare
  return response;
}

void updatePlayerScore(char scoreResponse) {
  int scorePoints = 0;

  // actualizeaza scorul pe baza raspunsului primit
  switch (scoreResponse) {
    case 'e': scorePoints = 100; break; // excellent
    case 'g': scorePoints = 50; break;  // good
    case 'm': scorePoints = 25; break;  // mediocre
    case 'i': scorePoints = 0; break;   // invalid
  }

  // actualizeaza scorul pentru jucatorul curent
  if (isPlayerOneTurn) {
    playerTwoScore += scorePoints;
  } else {
    playerOneScore += scorePoints;
  }
}

void displayFinalScores() {
  // afiseaza scorurile finale pentru ambii jucatori
  lcd.clear();
  lcd.print(playerOneName + " score:");
  lcd.setCursor(0, 1);
  lcd.print(playerOneScore);
  delay(2000);

  lcd.clear();
  lcd.print(playerTwoName + " score:");
  lcd.setCursor(0, 1);
  lcd.print(playerTwoScore);
  delay(2000);

  // determina si afiseaza castigatorul
  lcd.clear();
  if (playerOneScore > playerTwoScore) {
    lcd.print("winner: " + playerOneName);
  } else if (playerTwoScore > playerOneScore) {
    lcd.print("winner: " + playerTwoName);
  } else {
    lcd.print("it's a draw!");
  }
  delay(2000);
}

void resetGameSettings() {
  // reseteaza variabilele de scor si de stare
  playerOneScore = 0;
  playerTwoScore = 0;
  roundNumber = 0;
  isPlayerOneTurn = true;
  lcd.clear();
  lcd.print("press to start!");
}