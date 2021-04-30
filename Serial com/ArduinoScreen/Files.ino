void writeFile()
{
  if (!SD.begin(10))
  {
    lcd.println("SD intialization failed");
    return;
  }
  myFile = SD.open("absentees.txt", FILE_WRITE);
  if (myFile)
  {
    while (1)
    {
      if (ESPserial.available())
      {
        char a = ESPserial.read();
        if (a == '*')
        {
          myFile.print('\n');
          break;
        }
        //lcd.print(a);
        myFile.print(a);
      }
    }
    myFile.close();
  }
}

void absent_delete()
{
  while (1)
  {
    if (ESPserial.available())
    {
      char a = ESPserial.read();
      if (a == '*')
        break;
      lcd.print(a);
    }
  }
  lcd.setCursor(70, lcd.height() - 20);
  lcd.setTextSize(1);
  lcd.println(F("Touch to go back"));
  lcd.setCursor(0, 30);
  waitOneTouch();
}
void flush_esp()
{
  while (ESPserial.available())
    ESPserial.read();
}
