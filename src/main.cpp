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
#include "bitmap.h"
#include "ita_string.h"

#define ENABLE_DISPLAY
//#define ENABLE_SENSOR
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

typedef struct {
  uint8_t r, g, b;
} RGBColor;

typedef enum {
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

const RGBColor color_reference[] = {
  {65, 98, 86},    // GRAY
  {120,  71,  64},    // RED
  {92, 108,   51},    // YELLOW
  { 63, 122,  72},    // GREEN (verde prato)
  {  48,   93, 116},    // BLU
  {100,  89,  66},    // BROWN (marrone scuro)
  {156, 60,   43},    // ORANGE (arancione scuro)
  {76,   81, 99},    // PURPLE (viola intenso)
  {108, 76, 72},    // PINK (rosa chiaro)
  {44, 105, 109}     // AZURE (azzurro cielo)
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

// Display object
#ifdef ENABLE_DISPLAY
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

// Sensor object
#if defined(ENABLE_SENSOR) && defined(TCS34725)
  Adafruit_TCS34725 tcs = Adafruit_TCS34725();
#endif

// Setup of the ARDUINO NANO with pin init
void setup() 
{
Serial.begin(9600);
#if defined(ENABLE_SENSOR) && defined(TCS3200)
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
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

#ifdef ENABLE_DISPLAY
  // Display init
  if (!display.begin( SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.print("No Display");
    while (true); // If no display found stop the program in a loop
  }
    // Buffer clear
  display.clearDisplay();
  // Apply to display
  display.display();
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
  curretColor.r = 101;
  curretColor.g = 67;
  curretColor.b = 33;
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
      ;
  }

  while(true); //After one read stop the program. If you want another read push the power button
}


// Bitmap and text draw function (2/3 Bitmap, 1/3 String) 
void drawBitmapWithText(const unsigned char* bitmap, int bmp_width, int bmp_height, const char* message) 
{
  #ifdef ENABLE_DISPLAY
  display.clearDisplay();

  // Place the bitmap centered at the top
  int x_bmp = (SCREEN_WIDTH - bmp_width) / 2;
  int y_bmp = 0;

  display.drawBitmap(x_bmp, y_bmp, bitmap, bmp_width, bmp_height, WHITE);

  // Set text size
  display.setTextSize(2);
  display.setTextColor(WHITE);

  // Calculate where to center the text (**lower third**)
  int y_text = bmp_height + ((SCREEN_HEIGHT - bmp_height - 16) / 2); // 16 = font size 2 approx.

  // Calculate text width to center it horizontally
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);

  int x_text = (SCREEN_WIDTH - w) / 2;

  display.setCursor(x_text, y_text);
  display.print(message);

  display.display();
#endif
  Serial.print(message);
}

// We calculate color as the minimum distance in 3 dimensions
// (ignoring the square root which does not change for the purposes of finding the closest)
ColorClass bestMatchRGB(RGBColor currentColor) 
{
  uint32_t minDist = 0xFFFFFFFF;
  ColorClass best = COL_GRAY;
  for (unsigned int i = 0; i < sizeof(color_reference)/sizeof(color_reference[0]); i++) {
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
  uint16_t rRaw, gRaw, bRaw, cRaw;
  // Reading channels from the sensor
  tcs.getRawData(&rRaw, &gRaw, &bRaw, &cRaw);
  float r,g,b;
  tcs.getRGB(&r,&g,&b);

  if (cRaw == 0) cRaw = 1;

  // We normalize each value to 8 bits (0-255) based on the "clear" channel
  color.r = (uint8_t)(min(255, (rRaw * 255) / cRaw));
  color.g = (uint8_t)(min(255, (gRaw * 255) / cRaw));
  color.b = (uint8_t)(min(255, (bRaw * 255) / cRaw));

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