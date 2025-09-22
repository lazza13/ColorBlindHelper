/*

 * Color Blind Helper
 * Copyright (c) 2025 Giorgio Lazzaretti (lazza13@gmail.com)
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TCS34725.h>
#if defined(ESP32) && defined(COLORBLINDHELPER_OLED042)
  #include <U8g2lib.h>
#endif
#include "bitmap.h"
#include "ita_string.h"

#define ENABLE_DISPLAY
#define ENABLE_SENSOR
//#define CALIBRATION_MODE

//sensor type define
#ifdef ENABLE_SENSOR
  //#define TCS3200
  #define TCS34725
  #ifdef TCS3200
    //#define CALIBRATION_MODE
  #endif
#endif

//sanitiy check
#if defined(ENABLE_SENSOR) && defined(TCS3200) && defined(TCS34725)
  #error Choose TCS3200 or TCS34725. It cannot reads both sensor at the same time.
#endif


// PIN for TCS3200 sensor
#define S0 4  //D4
#define S1 5  //D5
#define S2 6  //D6
#define S3 7  //D7
#define OUT 8 //D8

#define OLED_ADDR 0x3C

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// esp32 c3 oled def
#if defined(ESP32) && defined(COLORBLINDHELPER_OLED042)
  const int OLED_WIDTH = 72, OLED_HEIGHT = 40, X_OFFSET = 28, Y_OFFSET = 32;
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);
#else 
    // Display object
  #ifdef ENABLE_DISPLAY
    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
  #endif
#endif

typedef struct {
  uint8_t r, g, b;
} RGBColor;

typedef enum {
    COL_UNDEFINED = -1,
    COL_GRAY,
    COL_RED,
    COL_YELLOW,
    COL_GREEN,
    COL_BLUE,
    COL_BROWN,
    COL_ORANGE,
    COL_PURPLE,
    COL_PINK,
    COL_AZURE
} ColorClass;

typedef struct {
  RGBColor reference_color;
  ColorClass color_class;
} ColoReference;

//Calibrate this value with your specific sensor
const ColoReference color_reference[] = {
  {{50,   50,   50},  COL_GRAY},    // GRAY
  {{112,  79,   71},  COL_RED},     // RED
  {{108,  78,   73},  COL_RED},     // RED Dark
  {{120,  76,   66},  COL_RED},     // RED Light
  {{92,   106,  50},  COL_YELLOW},  // YELLOW
  {{104,  100,  51},  COL_YELLOW},  // YELLOW Dark
  {{57,   115,  85},  COL_GREEN},   // GREEN 
  {{53,   109,  95},  COL_GREEN},   // GREEN Dark
  {{77,   117,  71},  COL_GREEN},   // GREEN Light
  {{51,   96,   107}, COL_BLUE},    // BLU dark
  {{43,   95,   119}, COL_BLUE},    // BLU light
  {{95,   92,   70},  COL_BROWN},   // BROWN
  {{91,   93,   70},  COL_BROWN},   // BROWN dark
  {{95,   92,   66},  COL_BROWN},   // BROWN light
  {{113,  87,   60},  COL_ORANGE},  // ORANGE 
  {{117,  82,   62},  COL_ORANGE},  // ORANGE dark
  {{111,  90,   56},  COL_ORANGE},  // ORANGE light
  {{53,   85,   117}, COL_PURPLE},  // PURPLE 
  {{95,   76,   90}, COL_PURPLE},  // PURPLE light
  {{85,   93,   80},  COL_PINK},    // PINK
  {{90,   84,   85},  COL_PINK},    // PINK dark
  {{76,   95,   86},  COL_PINK},    // PINK light
  {{41,   98,   117}, COL_AZURE},    // AZURE 
  {{38,   100,  119}, COL_AZURE}    // AZURE scuro
};


// Value for calibration of the TCS3200. White surface and black surface
int redMin = 0;   
int redMax = 0;
int greenMin = 0;
int greenMax = 0;
int blueMin = 0;
int blueMax = 0;

// Function definition
void drawBitmapWithText(const unsigned char* bitmap, int bmp_width, int bmp_height, const char* message);
ColorClass bestMatchRGB(RGBColor currentColor);
void rawSesnsorRead();
RGBColor rgbSensorReadTCS3200();
RGBColor readRGBColorTCS34725();

// Sensor object
#if defined(ENABLE_SENSOR) && defined(TCS34725)
  Adafruit_TCS34725 tcs = Adafruit_TCS34725();
#endif

// Setup of the ARDUINO NANO with pin init
void setup() 
{
  Serial.begin(9600);
  Serial.println("Running");
#if defined(ENABLE_SENSOR) && defined(TCS3200)
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
#endif

#ifdef ENABLE_DISPLAY
  #if defined(ESP32) && defined(COLORBLINDHELPER_OLED042)
    delay(1000);              // raccomanded from some exaple
    u8g2.begin();
    u8g2.setContrast(255);    // best visibility
    u8g2.setBusClock(400000); 
  #else
    // Display init
    if (!display.begin( SSD1306_SWITCHCAPVCC, 0x3C, true)) {
      Serial.print("No Display");
      while (true); // If no display found stop the program in a loop
    }
      // Buffer clear
    display.clearDisplay();
    // Apply to display
    display.display();
  #endif
#endif

#if defined(ENABLE_SENSOR) && defined(TCS34725)
  // Sensor init for TCS34725 (GY-33) with I2C
  if (!tcs.begin()) {
    Serial.print("No sensor");
    // If no sensor found stop the program in a loop
    while (true);
  }
  tcs.setGain(TCS34725_GAIN_16X);
#endif
}

// Exec Loop
void loop() 
{
  RGBColor curretColor;
#ifdef ENABLE_SENSOR
  #ifdef TCS3200
    #ifdef CALIBRATION_MODE
      rawSesnsorRead(); //read of the calibration parameters (only first time)
      return; //// exit from loop to avoid while at the end and read again
    #else
      curretColor = rgbSensorReadTCS3200();
    #endif
  #elif defined(TCS34725)
    curretColor = readRGBColorTCS34725();
  #endif
#else
  curretColor.r = 112;
  curretColor.g = 79;
  curretColor.b = 71;
#endif
  //Find nearest colo meatch
  ColorClass col = bestMatchRGB(curretColor);
  switch(col) {
    case COL_GRAY:
      drawBitmapWithText(epd_bitmap_gray,40,40, GRAY_STR);
      break;
    case COL_RED:
      drawBitmapWithText(epd_bitmap_red,40,40, RED_STR);
      break;
    case COL_YELLOW:
      drawBitmapWithText(epd_bitmap_yellow, 40, 40, YELLOW_STR);
      break;
    case COL_BLUE:
      drawBitmapWithText(epd_bitmap_blue,40,40, BLUE_STR);
      break;
    case COL_BROWN:
      drawBitmapWithText(epd_bitmap_brown, 40, 40, BROWN_STR);
      break;
    case COL_ORANGE:
      drawBitmapWithText(epd_bitmap_orange,40,40, ORANGESTR_STR);
      break;
    case COL_PURPLE:
      drawBitmapWithText(epd_bitmap_purple,40,40, PURPLE_STR);
      break;
    case COL_PINK:
      drawBitmapWithText(epd_bitmap_pink, 40, 40, PINK_STR);
      break;
    case COL_AZURE:
      drawBitmapWithText(epd_bitmap_azure,40,40, AZURE_STR);
      break;
    case COL_GREEN:
      drawBitmapWithText(epd_bitmap_green, 40, 40, GREEN_STR);
      break;
    default:
      drawBitmapWithText(nullptr, 0, 0, "?????");
      ;
  }
  delay(500);
}


// Bitmap and text draw function (2/3 Bitmap, 1/3 String) 
void drawBitmapWithText(const unsigned char* bitmap, int bmp_width, int bmp_height, const char* message) 
{
  #ifdef ENABLE_DISPLAY
    #ifdef COLORBLINDHELPER_OLED042
      // --- PER OLED 0.42" su ESP32-C3/U8G2 ---
      u8g2.clearBuffer();

      // // Bitmap centrata nell'area visibile con offset
      // int x_bmp = X_OFFSET/2 + (OLED_WIDTH  - bmp_width) / 2;
      // int y_bmp = Y_OFFSET/2 + 2;
      // u8g2.drawXBMP(x_bmp, y_bmp, bmp_width, bmp_height, bitmap);

      // Testo centrato sotto la bitmap
      u8g2.setFont(u8g2_font_ncenB10_tr);
      int textWidth = u8g2.getStrWidth(message);
      int x_text = X_OFFSET + (OLED_WIDTH - textWidth) / 2;
      int y_text = Y_OFFSET + OLED_HEIGHT - 8 -1;

      u8g2.drawStr(x_text, y_text, message);

      u8g2.sendBuffer();

  #else
      // --- PER DISPLAY CLASSICO Adafruit SSD1306 ---
      display.clearDisplay();
      int x_bmp = (display.width() - bmp_width) / 2;
      int y_bmp = 0;
      display.drawBitmap(x_bmp, y_bmp, bitmap, bmp_width, bmp_height, WHITE);

      display.setTextSize(2); // o regola secondo font desiderato
      display.setTextColor(WHITE);
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
      int x_text = (display.width() - w) / 2;
      int y_text = bmp_height + ((display.height() - bmp_height - 16) / 2);

      display.setCursor(x_text, y_text);
      display.print(message);
      display.display();
  #endif
#endif
  Serial.println(message);
}

#define THRESHOLD 700 
// We calculate color as the minimum distance in 3 dimensions
// (ignoring the square root which does not change for the purposes of finding the closest)
ColorClass bestMatchRGB(RGBColor currentColor) 
{
  uint32_t minDist = 0xFFFFFFFF;
  ColorClass best = COL_UNDEFINED;
  for (unsigned int i = 0; i < sizeof(color_reference)/sizeof(color_reference[0]); i++) {
    int dr = (int)currentColor.r - color_reference[i].reference_color.r;
    int dg = (int)currentColor.g - color_reference[i].reference_color.g;
    int db = (int)currentColor.b - color_reference[i].reference_color.b;
    uint32_t dist = dr*dr + dg*dg + db*db;
    if (dist < minDist && dist <= THRESHOLD) {
      minDist = dist;
      best = color_reference[i].color_class;
    }
  }
  Serial.println(minDist);
  return best;
}

// Function for RAW sensor reading to obtain calibration data
// for exact sensor readings
RGBColor rgbSensorReadTCS3200() 
{
  RGBColor colorData;
  // Red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int redRaw = pulseIn(OUT, LOW);

  // Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int greenRaw = pulseIn(OUT, LOW);

  // Blu
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int blueRaw = pulseIn(OUT, LOW);

  // Converting from raw to 0-255 scale (mapping the range between your minimums and maximums)
  int red = map(redRaw, redMin, redMax, 255, 0);
  int green = map(greenRaw, greenMin, greenMax, 255, 0);
  int blue = map(blueRaw, blueMin, blueMax, 255, 0);

  // Limit values ​​between 0 and 255
  colorData.r = constrain(red, 0, 255);
  colorData.g = constrain(green, 0, 255);
  colorData.b = constrain(blue, 0, 255);

  return colorData;
}

// Function for reading color in RGB format with calibration data
void rawSesnsorRead() 
{
  // Read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int red = pulseIn(OUT, LOW);

  // Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int green = pulseIn(OUT, LOW);

  // Blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int blue = pulseIn(OUT, LOW);

   
  char buffer[200];
  sprintf(buffer,"r: %d, g: %d, b: %d",red,green,blue);
#ifdef ENABLE_DISPLAY
  drawBitmapWithText(nullptr,0,0,buffer);
#endif
  
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print("  Green: ");
  Serial.print(green);
  Serial.print("  Blue: ");
  Serial.println(blue);

  delay(500);
}

RGBColor readRGBColorTCS34725() 
{
  RGBColor color;
#if !defined(TCS34725) || !defined(ENABLE_SENSOR)
  color.r = 255;
  color.g = 255;
  color.b = 255;
  return color;
#else
  float r,g,b;
  tcs.getRGB(&r,&g,&b);

  color.r = (uint8_t)r;
  color.g = (uint8_t)g;
  color.b = (uint8_t)b;

  Serial.print("Red: ");
  Serial.print(r);
  Serial.print("  Green: ");
  Serial.print(g);
  Serial.print("  Blue: ");
  Serial.println(b);
  // Serial.print("  Clear: ");
  // Serial.println(cRaw);

  return color;
#endif
}