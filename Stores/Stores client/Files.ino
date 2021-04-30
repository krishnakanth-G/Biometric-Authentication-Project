uint32_t readId(int i)
{
  int num = 0;
  uint32_t n = 0;
  char c = '\0';
  File myFile = LittleFS.open("/record.txt", "r");
  if (myFile)
  {
    while (myFile.available())
    {
      num = 0;
      c = myFile.read();
      if (c == '+')
      {
        while (myFile.available())
        {
          c = myFile.read();
          if (c != '-')
          {
            if (c == '\n')
              break;
            num = (num * 10) + (c - 48);
          }
          if (c == '-')
          {
            if (num == i)
            {
              n = 0;
              while (myFile.available())
              {
                c = myFile.read();
                //lcd.println(n);
                if (c == '\n' || c == '\r')
                  return n;
                n = (n * 10) + (c - 48);
              }
            }
          }
        }
      }
    }
    myFile.close();
  }
}
