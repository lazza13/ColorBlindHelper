#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "bitmap.h"


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

//Definizione delle funzioni
void drawBitmapWithText(const unsigned char* bitmap, int bmp_width, int bmp_height, const char* message);
ColorClass bestMatchRGB(uint8_t r, uint8_t g, uint8_t b);

// Creo istanza del display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

void setup() {

  //Inizializzazione display
  if (!display.begin( SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true); //rimango fermo se non lo trovo 
  }
  // Buffer clear
  display.clearDisplay();
  // Disegno del display
  display.display();
}

void loop() {

  ColorClass col = bestMatchRGB(101, 67, 33);
  switch(col) {
    case COL_NERO: 
      drawBitmapWithText(nullptr,40,40, "NERO");
      break;
    case COL_BIANCO:
      drawBitmapWithText(nullptr,40,40, "BIANCO");
      break;
    case COL_GRIGIO:
      drawBitmapWithText(nullptr,40,40, "GRIGIO");
      break;
    case COL_ROSSO:    // ...
      drawBitmapWithText(epd_bitmap_cuore,40,40, "ROSSO");
      break;
    case COL_GIALLO:
      drawBitmapWithText(epd_bitmap_sole, 40, 40, "GIALLO");
      break;
    case COL_BLU:
      drawBitmapWithText(nullptr,40,40, "BLU");
      break;
    case COL_MARRONE:
      drawBitmapWithText(epd_bitmap_marrone, 40, 40, "MARRONE");
      break;
    case COL_ARANCIONE:
      drawBitmapWithText(nullptr,40,40, "ARANCIONE");
      break;
    case COL_VIOLA:
      drawBitmapWithText(nullptr,40,40, "VIOLA");
      break;
    case COL_ROSA:
      drawBitmapWithText(epd_bitmap_rosa, 40, 40, "ROSA");
      break;
    case COL_AZZURRO:
      drawBitmapWithText(nullptr,40,40, "AZZURRO");
      break;
    case COL_VERDE:
      drawBitmapWithText(epd_bitmap_foglia, 40, 40, "VERDE");
      break;
    default:
      drawBitmapWithText(epd_bitmap_marrone, 40, 40, "MARRONE");
  }
  
  return; //non voglio che sia un loop ma deve fare le cose solo una volta
}


//Disegno della bitmap piu testo
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

// calcoliamo il coore come la distanza minima (tralasciando la radice quadrata che non cambia ai fini del trovare il piu vicino)
ColorClass bestMatchRGB(uint8_t r, uint8_t g, uint8_t b) 
{
  uint32_t minDist = 0xFFFFFFFF;
  ColorClass best = COL_NERO;
  for (int i = 0; i < sizeof(color_reference)/sizeof(color_reference[0]); i++) {
    int dr = (int)r - color_reference[i].r;
    int dg = (int)g - color_reference[i].g;
    int db = (int)b - color_reference[i].b;
    uint32_t dist = dr*dr + dg*dg + db*db;
    if (dist < minDist) {
      minDist = dist;
      best = (ColorClass)i;
    }
  }
  return best;
}