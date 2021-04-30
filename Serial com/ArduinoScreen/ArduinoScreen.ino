#include <SoftwareSerial.h>
#include "SPFD5408_Adafruit_GFX.h"    // Core graphics library
#include "SPFD5408_Adafruit_TFTLCD.h" // Hardware-specific library
#include "SPFD5408_TouchScreen.h"     // Touch screen library
#include <SPI.h>
#include "SdFat.h"
SdFat SD;

#define LCD_RESET A4 // Reset goes to Analog 4
#define LCD_CS A3    // Chip Select goes to Analog 3
#define LCD_CD A2    // Command/Data goes to Analog 2
#define LCD_WR A1    // LCD Write goes to Analog 1
#define LCD_RD A0    // LCD Read goes to Analog 0
#define YP A1
#define XM A2
#define YM 7
#define XP 6

//calibrate value
#define SENSIBILITY 400
#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

//Colors
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define AQUA 0x66FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xF2C9

// Buttons
#define BUTTONS 12
#define BUTTON_REGISTER 0
#define BUTTON_VERIFY 1
#define BUTTON_DELETE 2
#define BUTTON_SETTIME 3
#define BUTTON_CHANGEADMIN 4
#define BUTTON_ABSENT 5
#define BUTTON_LOGOUT 6
#define BUTTON 7
#define SD_CS_PIN 10

File myFile;

Adafruit_GFX_Button buttons[BUTTONS]; //Declareing graphic buttons

Adafruit_TFTLCD lcd(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //Lcd initialization

TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY); //Touchscreen initialization

SoftwareSerial ESPserial(0, 1); // RX | TX

void setup()
{
  lcd.reset();
  lcd.begin(0x9341);
  lcd.setRotation(2);
  lcd.fillScreen(BLACK);
  ESPserial.begin(115200);
}

TSPoint waitOneTouch()
{
  TSPoint p;
  do
  {
    p = ts.getPoint();
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
  } while ((p.z < MINPRESSURE) || (p.z > MAXPRESSURE));

  return p;
}
void waitOneTouch1()
{
  lcd.setCursor(70, lcd.height() - 20);
  lcd.setTextSize(1);
  lcd.println(F("Touch to go back"));
  lcd.setCursor(0, 30);
  TSPoint p;
  do
  {
    if (ESPserial.available() > 0)
      write_lcd();
    p = ts.getPoint();
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
  } while ((p.z < MINPRESSURE) || (p.z > MAXPRESSURE));
  write_esp('1');
}

String sendzero(int num)
{
  if (num < 10)
    return "0";
  num = num / 10;
  return String(num);
}
int admin()
{
  int yy = 0, mm = 0, dd = 0, hh = 0, mi = 0, ss = 0;
  String r;
  uint32_t reg = 0;
  lcd.fillScreen(BLACK);
  initializeButtons(1);
  for (uint8_t i = 0; i < BUTTON; i++)
  {
    buttons[i].drawButton();
  }

  TSPoint p;
  p = waitOneTouch();

  p.x = map(p.x, TS_MINX, TS_MAXX, 0, lcd.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, lcd.height());
  for (uint8_t b = 0; b < BUTTON; b++)
  {
    if (buttons[b].contains(p.x, p.y))
    {
      switch (b)
      {
      case BUTTON_REGISTER:
        lcd.setCursor(0, 20);
        lcd.fillScreen(BLACK);
        reg = Time_Date_Reg(10000, 999999, "Id");
        lcd.fillScreen(BLACK);
        lcd.setTextColor(WHITE);
        lcd.setTextSize(1);
        r = "e" + String(reg);
        //lcd.println(reg);
        ESPserial.print(r);
        delay(1000);
        waitOneTouch1();
        break;
      case BUTTON_VERIFY:
        lcd.fillScreen(BLACK);
        lcd.setCursor(0, 20);
        lcd.setTextColor(WHITE);
        lcd.setTextSize(1);
        lcd.println(F("Put your finger on sensor"));
        write_esp('v');
        delay(1000);
        waitOneTouch1();
        break;
      case BUTTON_DELETE:
        lcd.fillScreen(BLACK);
        reg = Time_Date_Reg(10000, 999999, "Id");
        lcd.fillScreen(BLACK);
        lcd.setTextColor(WHITE);
        lcd.setCursor(0, 20);
        lcd.setTextSize(1);
        r = "d" + String(reg);
        ESPserial.print(r);
        delay(1000);
        absent_delete();
        break;
      case BUTTON_SETTIME:
        lcd.fillScreen(BLACK);
        hh = Time_Date_Reg(0, 23, "Hour");
        mi = Time_Date_Reg(0, 59, "Minute");
        yy = Time_Date_Reg(2019, 9999, "Year");
        mm = Time_Date_Reg(1, 12, "Month");
        dd = Time_Date_Reg(1, 31, "Day");
        if (yy < 2019)
          yy = 2020;
        r = 't';
        r = r + sendzero(hh) + String(hh % 10) + sendzero(mi) + String(mi % 10) + sendzero(dd) + String(dd % 10) + sendzero(mm % 10) + String(mm % 10) + String(yy);
        ESPserial.print(r);
        lcd.fillScreen(BLACK);
        lcd.setCursor(0, 20);
        lcd.setTextColor(WHITE);
        delay(1000);
        waitOneTouch1();
        break;
      case BUTTON_CHANGEADMIN:
        lcd.fillScreen(BLACK);
        lcd.setCursor(0, 20);
        lcd.setTextColor(WHITE);
        lcd.setTextSize(1);
        lcd.println(F("Admin Access required"));
        write_esp('c');
        delay(1000);
        waitOneTouch1();
        break;
      case BUTTON_ABSENT:
        lcd.fillScreen(BLACK);
        lcd.setCursor(0, 10);
        lcd.setTextColor(WHITE);
        lcd.setTextSize(1);
        write_esp('a');
        delay(1000);
        absent_delete();
        break;
      case BUTTON_LOGOUT:
        lcd.fillScreen(BLACK);
        return 0;
        break;
      }
      return 1;
    }
  }
  return 1;
}

void printScreen(uint32_t num, String mode)
{
  lcd.fillRect(25, 10, lcd.width() - 50, 40, WHITE);
  lcd.setTextColor(BLACK);
  lcd.setCursor(30, 20);
  lcd.setTextSize(2);
  lcd.println(mode + ":" + num);
}

uint32_t Time_Date_Reg(uint32_t min_size, uint32_t max_size, String mode)
{
  lcd.fillRect(25, 10, lcd.width() - 50, 40, WHITE);
  int count = 0;
  initializeButtons(2);
  for (uint8_t i = 0; i < 12; i++)
  {
    buttons[i].drawButton();
  }
  uint32_t num = 0;
  TSPoint p;
  printScreen(num, mode);
  while (1)
  {
    count = 0;
    delay(1000);
    p = waitOneTouch();
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, lcd.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, lcd.height());

    for (uint8_t b = 0; b < BUTTONS; b++)
    {
      if (count == 1)
        break;
      if (buttons[b].contains(p.x, p.y))
      {
        count = 1;
        switch (b)
        {
        case 0:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 1:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 2:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 3:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 4:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 5:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 6:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 7:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 8:
          num = (num * 10) + (b + 1);
          printScreen(num, mode);
          break;
        case 9:
          if (num > max_size)
          {
            lcd.fillRect(25, 10, lcd.width() - 50, 40, WHITE);
            lcd.setTextColor(BLACK);
            lcd.setCursor(30, 20);
            lcd.print(F("MAX NUMBER:"));
            lcd.println(max_size);
            delay(1000);
            num = 0;
            printScreen(num, mode);
          }
          else if (num < min_size)
          {
            lcd.fillRect(25, 10, lcd.width() - 50, 40, WHITE);
            lcd.setTextColor(BLACK);
            lcd.setCursor(30, 20);
            lcd.print(F("MIN NUMBER:"));
            lcd.println(min_size);
            delay(1000);
            num = 0;
            printScreen(num, mode);
          }
          else
            return num;
          break;
        case 10:
          num = (num * 10);
          printScreen(num, mode);
          break;
        case 11:
          num = num / 10;
          printScreen(num, mode);
          break;
        }
      }
    }
  }
}

void initializeButtons(int i)
{
  if (i == 1)
  {
    uint16_t x = 65;
    uint16_t y = 40;
    uint16_t w = 90;
    uint16_t h = 40;

    uint8_t spacing_x = 15;
    uint8_t spacing_y = 15;
    uint8_t textSize = 1;

    char buttonlabels[7][9] = {"Register", "Verify", "Delete", "Time_Date", "Admin_chg", "Absentees", "LOGOUT"};
    uint16_t buttoncolors[15] = {RED, BLUE, BLUE, RED, RED, BLUE, GREEN};

    for (uint8_t b = 0; b < BUTTON - 1; b++)
    {
      buttons[b].initButton(&lcd,                                                         // TFT object
                            x + (b % 2) * (w + spacing_x), y + (b / 2) * (h + spacing_y), // x, y,
                            w, h, WHITE, buttoncolors[b], WHITE,                          // w, h, outline, fill,
                            buttonlabels[b], textSize);                                   // text
    }
    buttons[BUTTON - 1].initButton(&lcd, 120, 280, w, h, WHITE, buttoncolors[BUTTON - 1], WHITE, buttonlabels[BUTTON - 1], textSize); // TFT object,x, y,w, h, outline, fill,text,text,text
  }
  if (i == 2)
  {
    uint16_t x = 50;
    uint16_t y = 90;
    uint16_t w = 50;
    uint16_t h = 50;

    uint8_t spacing_x = 15;
    uint8_t spacing_y = 15;
    uint8_t textSize = 2;

    char buttonlabels[12][2] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "S", "0", "C"};
    uint16_t buttoncolors[15] = {RED, BLUE, RED, BLUE, RED, BLUE, RED, BLUE, RED, GREEN, RED, GREEN};

    for (uint8_t b = 0; b < BUTTONS; b++)
    {
      buttons[b].initButton(&lcd,                                                         // TFT object
                            x + (b % 3) * (w + spacing_x), y + (b / 3) * (h + spacing_y), // x, y,
                            w, h, WHITE, buttoncolors[b], WHITE,                          // w, h, outline, fill,
                            buttonlabels[b], textSize);                                   // text
    }
  }
}

void write_esp(char a)
{
  ESPserial.write(a);
}
void write_lcd()
{
  while (ESPserial.available())
  {
    char a = ESPserial.read();
    lcd.print(a);
  }
  delay(1000);
}
void loop()
{
  lcd.setCursor(75, 10);
  lcd.fillScreen(BLACK);
  lcd.setTextColor(ORANGE);
  lcd.setTextSize(1);
  lcd.print("Aum Sri Sai Ram");
  lcd.setCursor(40, 150);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(1);
  write_esp('p');
  int i = 0;
  while (1)
  {
    if (ESPserial.available())
    {
      char a = ESPserial.read();
      if (a != '+' && a != '#')
        lcd.print(a);
      write_lcd();
      if (a == '+')
      {
        flush_esp();
        while (1)
        {
          i = admin();
          if (i == 0)
            break;
        }
        if (i == 0)
          break;
      }
      else if (a == '#')
      {
        flush_esp();
        lcd.fillScreen(BLACK);
        lcd.setCursor(40, 150);
        lcd.setTextColor(WHITE);
        lcd.setTextSize(1);
        lcd.println(F("Saving in process..."));
        write_esp('s');
        writeFile();
        break;
      }
      else
        break;
    }
  }
}
