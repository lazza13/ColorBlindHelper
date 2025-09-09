#ifdef TEST
#define S0 4  //D4
#define S1 5  //D5
#define S2 6  //D6
#define S3 7  //D7
#define OUT 8 //D8

// Valori da calibrare con superfici di riferimento bianca (MAX) e nera (MIN)
int redMin = 0/* INSERISCI IL TUO VALORE */;      
int redMax = 0/* INSERISCI IL TUO VALORE */;
int greenMin = 0/* INSERISCI IL TUO VALORE */;
int greenMax = 0/* INSERISCI IL TUO VALORE */;
int blueMin = 0/* INSERISCI IL TUO VALORE */;
int blueMax = 0/* INSERISCI IL TUO VALORE */;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  Serial.begin(9600);
}
void normalLoop();
void calibrationLoop();

void Loop() {
  calibrationLoop();
}

void normalLoop() {
  // Rosso
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int redRaw = pulseIn(OUT, LOW);

  // Verde
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int greenRaw = pulseIn(OUT, LOW);

  // Blu
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int blueRaw = pulseIn(OUT, LOW);

  // Conversione da raw a scala 0-255 (mappando l’intervallo tra i tuoi minimi e massimi)
  int red = map(redRaw, redMin, redMax, 255, 0);
  int green = map(greenRaw, greenMin, greenMax, 255, 0);
  int blue = map(blueRaw, blueMin, blueMax, 255, 0);

  // Limita i valori tra 0 e 255
  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);

  Serial.print("RGB: ");
  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.println(blue);

  delay(500);
}

void calibrationLoop() {
  // Rosso
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int red = pulseIn(OUT, LOW);

  // Verde
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int green = pulseIn(OUT, LOW);

  // Blu
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int blue = pulseIn(OUT, LOW);

  Serial.print("Red: ");
  Serial.print(red);
  Serial.print("  Green: ");
  Serial.print(green);
  Serial.print("  Blue: ");
  Serial.println(blue);

  delay(500);
}


#endif