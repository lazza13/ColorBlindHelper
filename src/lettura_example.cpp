#ifdef TEST
#define S2_PIN  4
#define S3_PIN  5
#define OUT_PIN 6

void setup() {
  pinMode(S2_PIN, OUTPUT);
  pinMode(S3_PIN, OUTPUT);
  pinMode(OUT_PIN, INPUT);
  Serial.begin(9600);
}

// Funzione generica per leggere la frequenza del colore selezionato
unsigned int readColor() {
  // Leggevo pulse width in uscita: minore è il periodo, maggiore la concentrazione di quel colore
  // On alcuni sensori, meglio pulseIn(OUT_PIN, LOW)
  unsigned int t = pulseIn(OUT_PIN, LOW);
  return t == 0 ? 1 : t; // evitare divisione per zero dopo
}

void readRGB(uint8_t *red, uint8_t *green, uint8_t *blue) {
  // Rosso: S2 LOW, S3 LOW
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, LOW);
  delay(2);
  unsigned int r = readColor();

  // Verde: S2 HIGH, S3 HIGH
  digitalWrite(S2_PIN, HIGH);
  digitalWrite(S3_PIN, HIGH);
  delay(2);
  unsigned int g = readColor();

  // Blu: S2 LOW, S3 HIGH
  digitalWrite(S2_PIN, LOW);
  digitalWrite(S3_PIN, HIGH);
  delay(2);
  unsigned int b = readColor();

  // Converto in "componente luminosa": minore pulseIn = maggiore colore, inverti
  unsigned int maxValue = max(r, max(g, b));
  *red   = map(maxValue - r, 0, maxValue, 0, 255);
  *green = map(maxValue - g, 0, maxValue, 0, 255);
  *blue  = map(maxValue - b, 0, maxValue, 0, 255);
}

void loop() {
  uint8_t r, g, b;
  readRGB(&r, &g, &b);
  Serial.print("R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.println(b);
  delay(500);
}
#endif