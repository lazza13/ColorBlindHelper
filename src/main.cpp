#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "bitmap.h"

//#define ENABLE_SENSOR
//#define CALIBRATION_MODE

// PIN per la gestione del sensore TCS3200
#define S0 4  //D4
#define S1 5  //D5
#define S2 6  //D6
#define S3 7  //D7
#define OUT 8 //D8

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

typedef struct {
  uint8_t r, g, b;
} RGBColor;

typedef enum {
    COL_NERO,
    COL_BIANCO,
    COL_GRIGIO,
    COL_ROSSO,
    COL_GIALLO,
    COL_VERDE,
    COL_BLU,
    COL_MARRONE,
    COL_ARANCIONE,
    COL_VIOLA,
    COL_ROSA,
    COL_AZZURRO
} ColorClass;

const RGBColor color_reference[] = {
  {  0,   0,   0 },   // NERO
  {255, 255, 255},    // BIANCO
  {127, 127, 127},    // GRIGIO
  {220,  20,  60},    // ROSSO
  {255, 255,   0},    // GIALLO
  { 34, 139,  34},    // VERDE (verde prato)
  {  0,   0, 255},    // BLU
  {139,  69,  19},    // MARRONE (marrone scuro)
  {255, 140,   0},    // ARANCIONE (arancione scuro)
  {148,   0, 211},    // VIOLA (viola intenso)
  {255, 182, 193},    // ROSA (rosa chiaro)
  {135, 206, 250}     // AZZURRO (azzurro cielo)
};


// Valori da calibrare con superfici di riferimento bianca (MAX) e nera (MIN)
int redMin = 0;   
int redMax = 0;
int greenMin = 0;
int greenMax = 0;
int blueMin = 0;
int blueMax = 0;

// Definizione delle funzioni
void drawBitmapWithText(const unsigned char* bitmap, int bmp_width, int bmp_height, const char* message);
ColorClass bestMatchRGB(RGBColor currentColor);
void rawSesnsorRead();
RGBColor rgbSensorRead();

// Creo istanza del display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

// Setup dell'arduino NANO e inizializzazione del display
void setup() {

#ifdef ENABLE_SENSOR
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
#endif

  //Inizializzazione display
  if (!display.begin( SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true); //rimango fermo se non lo trovo 
  }
  // Buffer clear
  display.clearDisplay();
  // Disegno del display
  display.display();

  Serial.begin(9600);
}

// Loop di esecuzione
void loop() 
{
  RGBColor curretColor;
#ifdef ENABLE_SENSOR
  //Leggo il colore dal sensore
  #ifdef CALIBRATION_MODE
    rawSesnsorRead(); //lettura dei dati di calibrazione necessaria solo la prima volta
    return; //// ritorno per riavviare nuovamente le letture e non finire nel while di stop
  #else
    curretColor = rgbSensorRead();
  #endif
#else
  curretColor.r = 101;
  curretColor.g = 67;
  curretColor.b = 33;
#endif
  //Decido il colore piu vicino e lo stampo a schermo 
  ColorClass col = bestMatchRGB(curretColor);
  switch(col) {
    case COL_NERO: 
      drawBitmapWithText(epd_bitmap_formica,40,40, "NERO");
      break;
    case COL_BIANCO:
      drawBitmapWithText(epd_bitmap_nuvola,40,40, "BIANCO");
      break;
    case COL_GRIGIO:
      drawBitmapWithText(epd_bitmap_spada,40,40, "GRIGIO");
      break;
    case COL_ROSSO:
      drawBitmapWithText(epd_bitmap_cuore,40,40, "ROSSO");
      break;
    case COL_GIALLO:
      drawBitmapWithText(epd_bitmap_sun, 40, 40, "GIALLO");
      break;
    case COL_BLU:
      drawBitmapWithText(epd_bitmap_onde,40,40, "BLU");
      break;
    case COL_MARRONE:
      drawBitmapWithText(epd_bitmap_marrone, 40, 40, "MARRONE");
      break;
    case COL_ARANCIONE:
      drawBitmapWithText(epd_bitmap_arancia,40,40, "ARANCIONE");
      break;
    case COL_VIOLA:
      drawBitmapWithText(epd_bitmap_principessa,40,40, "VIOLA");
      break;
    case COL_ROSA:
      drawBitmapWithText(epd_bitmap_rosa, 40, 40, "ROSA");
      break;
    case COL_AZZURRO:
      drawBitmapWithText(epd_bitmap_maglietta,40,40, "AZZURRO");
      break;
    case COL_VERDE:
      drawBitmapWithText(epd_bitmap_foglia, 40, 40, "VERDE");
      break;
    default:
      drawBitmapWithText(epd_bitmap_marrone, 40, 40, "MARRONE");
  }

  while(true); //leggo solo una volta cosi per leggere bisogna premere nuovamente il pulsante di accensione
}


// Disegno della bitmap piu testo nella parte bassa del display
void drawBitmapWithText(const unsigned char* bitmap, int bmp_width, int bmp_height, const char* message) 
{
  display.clearDisplay();

  // Posiziona la bitmap centrata nella parte superiore
  int x_bmp = (SCREEN_WIDTH - bmp_width) / 2;
  int y_bmp = 0;

  display.drawBitmap(x_bmp, y_bmp, bitmap, bmp_width, bmp_height, WHITE);

  // Imposta dimensione testo
  display.setTextSize(2);
  display.setTextColor(WHITE);

  // Calcola dove centrare il testo (**terzo inferiore**)
  int y_text = bmp_height + ((SCREEN_HEIGHT - bmp_height - 16) / 2); // 16 = font size 2 approx.

  // Calcola larghezza del testo per centrarlo orizzontalmente
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);

  int x_text = (SCREEN_WIDTH - w) / 2;

  display.setCursor(x_text, y_text);
  display.print(message);

  display.display();
}

// Calcoliamo il colore come la distanza minima in 3 dimensioni 
// (tralasciando la radice quadrata che non cambia ai fini del trovare il piu vicino)
ColorClass bestMatchRGB(RGBColor currentColor) 
{
  uint32_t minDist = 0xFFFFFFFF;
  ColorClass best = COL_NERO;
  for (int i = 0; i < sizeof(color_reference)/sizeof(color_reference[0]); i++) {
    int dr = (int)currentColor.r - color_reference[i].r;
    int dg = (int)currentColor.g - color_reference[i].g;
    int db = (int)currentColor.b - color_reference[i].b;
    uint32_t dist = dr*dr + dg*dg + db*db;
    if (dist < minDist) {
      minDist = dist;
      best = (ColorClass)i;
    }
  }
  return best;
}

//Funzione per la lettura RAW del sensore in modo da ottenere i dati di calibrazione
//per la lettura esatta del sensore
RGBColor rgbSensorRead() 
{
  RGBColor colorData;
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
  colorData.r = constrain(red, 0, 255);
  colorData.g = constrain(green, 0, 255);
  colorData.b = constrain(blue, 0, 255);

  return colorData;
}

// Funzione per la lettura del colore in formato RGB con dati di calibrazione
void rawSesnsorRead() 
{
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

   
  char buffer[200];
  sprintf(buffer,"r: %d, g: %d, b: %d",red,green,blue);
  drawBitmapWithText(nullptr,0,0,buffer);
  
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print("  Green: ");
  Serial.print(green);
  Serial.print("  Blue: ");
  Serial.println(blue);

  delay(500);
}