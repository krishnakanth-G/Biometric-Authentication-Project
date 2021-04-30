#include "FPS_GT511C3.h" // Fingerprint library
#include <Time.h>
#include <LittleFS.h>

FPS_GT511C3 fps(4, 5); //initialization of RX to pin 4 and TX to pin 5

int Pre[199] = {};
int save = 0;
#define INTIMEH 5
#define INTIMEM 25
#define OUTTIMEM 45

void setup()
{
  Serial.begin(115200);
  fps.Open();
  fps.SetLED(true);
  setTime(5, 25, 30, 27, 3, 2020);
  //LittleFS.format();
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed");
    return;
  }
}

int breakloop()
{
  if (Serial.available())
  {
    char data;
    data = Serial.read();
    if (data == '1')
      return 1;
  }
  return 0;
}

void Verify()
{
  bool c = false;
  int id = 300;
  while (!fps.IsPressFinger())
  {
    if (breakloop())
      return;
  }
  c = fps.CaptureFinger(true);
  if (c)
  {
    id = fps.Identify1_N();
    if (id < 200)
    {
      if (id == 0 || id == 199)
      {
        Serial.print(F("Administrator"));
        return;
      }
      Serial.print(F("Finger print found\nid : "));
      uint32_t i = readId(id);
      Serial.println(i);
    }
    else
      Serial.println(F("No match found"));
  }
  else
  {
    Serial.println(F("Failed to capture"));
    Serial.println(F("try agian"));
  }
}

int Enroll(int i)
{
  int id = 1;
  bool is_exists = true;
  char reg[7] = {0, 0, 0, 0, 0, 0, 0};
  if (i == 0 || i == 199)
  {
    if (i == 0)
      id = 199;
    else
      id = 0;
  }
  else
  {
    Serial.readBytes(reg, 6);
    //Serial.println(reg);
    String reg1;
    for (int i = 1; i < 199; i++)
    {
      reg1 = readId(i);
      if (String(reg1) == reg)
      {
        Serial.println("Registration Number already exsits");
        return -1;
      }
    }
    while (is_exists)
    {
      is_exists = fps.CheckEnrolled(id);
      if (is_exists)
        id++;
    }
    if (id == 199)
    {
      Serial.println("Database Filled");
      return -1;
    }
  }

  fps.EnrollStart(id);
  Serial.println(F("Press Finger"));
  while (!fps.IsPressFinger())
  {
    if (breakloop())
      return -1;
  }
  bool capture = fps.CaptureFinger(true);
  int Status = 0;
  if (capture != false)
  {
    Serial.println(F("Remove finger"));
    Status = fps.Enroll1();
    while (fps.IsPressFinger() == true)
      delay(100);
    Serial.println(F("Press same finger again"));
    while (!fps.IsPressFinger())
    {
      if (breakloop())
        return -1;
    }
    capture = fps.CaptureFinger(true);
    if (capture != false && Status == 0)
    {
      Serial.println(F("Remove finger"));
      Status = fps.Enroll2();
      while (fps.IsPressFinger() == true)
        delay(100);
      Serial.println(F("Press same finger again"));
      while (!fps.IsPressFinger())
      {
        if (breakloop())
          return -1;
      }
      capture = fps.CaptureFinger(true);
      if (capture != false && Status == 0)
      {
        Serial.println(F("Remove finger"));
        Status = fps.Enroll3();
        if (Status == 0)
        {
          if (id != 0 && id != 199)
          {
            Serial.print(F("Enrolling Successful id :"));
            Serial.println(reg);
            //Serial.println(id);
            writeFile('+' + String(id) + '-' + reg + '\n');
            //readFile();
          }
          else
          {
            Serial.println(F("Welcome to new admin"));
          }
          return id;
        }
        else
        {
          Serial.print(F("Error code:"));
          //Serial.println(Status);
          if (Status == 1)
            Serial.println(F("Enroll Failed"));
          if (Status == 2)
            Serial.println(F("Bad Finger"));
          if (Status == 3)
            Serial.println(F("Finger already exsits"));
        }
      }
      else
      {
        Serial.println(F("Failed at 3rd press"));
      }
    }
    else
    {
      Serial.println(F("Failed at 2nd press"));
    }
  }
  else
  {
    Serial.println(F("Failed at 1st press"));
  }
  return -1;
}
int numbers(char a)
{
  switch (a)
  {
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  case '7':
    return 7;
  case '8':
    return 8;
  case '9':
    return 9;
  case '0':
    return 0;
  default:
    return -1;
  }
}
void settime()
{
  char str[18] = {};
  int hh = 0, mm = 0, ss = 0, dd = 0, m = 0, yy = 0;
  Serial.readBytes(str, 18);
  hh = (numbers(str[0]) * 10) + numbers(str[1]);
  mm = (numbers(str[2]) * 10) + numbers(str[3]);
  dd = (numbers(str[4]) * 10) + numbers(str[5]);
  m = (numbers(str[6]) * 10) + numbers(str[7]);
  yy = (numbers(str[8]) * 1000) + (numbers(str[9]) * 100) + (numbers(str[10]) * 10) + numbers(str[11]);
  setTime(hh, mm, ss, dd, m, yy);
  timeDisplay();
}
void Deleteid()
{
  int id = -1;
  bool c = false;
  uint32_t reg = 0;
  char reg1[7] = {0, 0, 0, 0, 0, 0, 0};
  Serial.readBytes(reg1, 6);
  //Serial.println(reg1);
  for (int i = 1; i < 199; i++)
  {
    reg = readId(i);
    if (String(reg) == reg1)
    {
      id = i;
      modify(i);
    }
  }
  bool is_exists = fps.CheckEnrolled(id);
  if (is_exists)
  {
    if (id == 0 || id == 199)
    {
      Serial.println(F("change admin before delete"));
      return;
    }
    fps.DeleteID(id);
    Serial.print(F("Finger print id : "));
    Serial.print(reg1);
    Serial.println(F(" is deleted"));
    //readFile();
  }
  else
  {
    Serial.print(F("No Finger Found with id :"));
    Serial.println(reg1);
  }
  Serial.println("*");
}

void present()
{
  bool c = false;
  int id = 300;
  if (hour() >= 23 && minute() >= 59)
    for (int i = 1; i < 199; i++)
      Pre[i] = 0;
  if (hour() >= 5 && minute() >= 45 && save == 0)
  {
    save = 1;
    Serial.print("#");
  }
  if (!fps.IsPressFinger())
    timeDisplay();
  if (fps.IsPressFinger())
  {
    c = fps.CaptureFinger(true);
    if (c)
    {
      id = fps.Identify1_N();
      if (id < 200)
      {
        if (id == 0 or id == 199)
        {
          Serial.println('+');
          return;
        }
        else if (hour() == INTIMEH)
          if (minute() > INTIMEM && minute() < OUTTIMEM)
          {
            if (Pre[id] == 1)
            {
              Serial.println(F("Only once"));
            }
            else
            {
              Serial.println(readId(id));
              Pre[id] = 1;
            }
          }
          else if (minute() <= INTIMEM && minute() < OUTTIMEM)
          {
            Serial.print(F("Not Yet Started"));
          }
          else
          {
            Serial.println(F("Time up"));
          }
        else if (hour() > INTIMEH)
        {
          Serial.println(F("Time up"));
        }
        else if (hour() < INTIMEH)
        {
          Serial.println(F("Not Yet Started"));
        }
      }
      else
      {
        Serial.println(F("No User"));
      }
    }
    else
    {
      Serial.println(F("Failed to capture"));
    }
  }
}

void attendance()
{
  int count = 0;
  uint32_t reg = 0;
  if(minute() > INTIMEM && hour() >= 5)
  for (int id = 1; id < 199; id++)
    if (fps.CheckEnrolled(id))
    {
      if (Pre[id] == 0)
      {
        count++;
        reg = readId(id);
        Serial.print(reg);
        Serial.print(F(", "));
        if (count % 2 == 0)
          Serial.println();
      }
    }
  if (count == 0)
  {
    Serial.println("No Absentees !!!*");
  }
  else
    Serial.println("*");
}
void sendLog()
{
  uint32_t reg = 0;
  int count = 0;
  timeDisplay();
  for (int id = 1; id < 199; id++)
    if (fps.CheckEnrolled(id))
    {
      if (Pre[id] == 0)
      {
        count++;
        reg = readId(id);
        Serial.println(reg);
      }
    }
  if (count == 0)
  {
    Serial.println("No Absentees !!!*");
  }
  else
    Serial.println("*");
}

void adminChange()
{
  bool c = false;
  int id = 300;
  int i = 0;
  while (!fps.IsPressFinger())
  {
    if (breakloop())
      return;
  }
  c = fps.CaptureFinger(true);
  if (c)
  {
    id = fps.Identify1_N();
    if (id == 0 || id == 199)
    {
      Serial.println(F("Access Granted"));

      i = Enroll(id);
      if (i != -1)
      {
        fps.DeleteID(id);
      }
    }
    else
    {
      Serial.println(F("Access denied"));
    }
  }
  else
  {
    Serial.println(F("Failed to capture"));
  }
}

void loop()
{
  while (Serial.available())
  {
    char data = Serial.read();
    switch (data)
    {
    case 'v':
      Verify();
      break;
    case 'e':
      Enroll(1);
      break;
    case 'a':
      attendance();
      break;
    case 'c':
      adminChange();
      break;
    case 'd':
      Deleteid();
      break;
    case 't':
      settime();
      break;
    case 'p':
      present();
      break;
    case 's':
      sendLog();
      break;
    }
  }
}
