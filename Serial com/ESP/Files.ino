void listDir(const char *dirname)
{
  Serial.printf("Listing directory: %s\n", dirname);
  Dir root = LittleFS.openDir(dirname);
  while (root.next())
  {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.print(file.size());
    file.close();
  }
}

void readFile()
{
  char c = '\0';
  File myFile = LittleFS.open("/record.txt", "r");
  if (myFile)
  {
    while (myFile.available())
    {
      c = myFile.read();
      Serial.print(c);
    }
    myFile.close();
  }
}

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

void modify(int id)
{
  int i = 0, flag = 0;
  char c = '\0';
  String str_old = "", str_new = "", str_num1 = "", str_num = String(id);
  File myFile = LittleFS.open("/record.txt", "r");
  if (myFile)
  {
    while (myFile.available())
    {
      c = myFile.read();
      str_old = str_old + c;
    }
    myFile.close();
  }
  while (str_old[i] != '\0')
  {
    if (flag == 0 && str_old[i] != '+')
      str_new = str_new + str_old[i];
    str_num1 = "";
    if (str_old[i] == '+')
    {
      flag = 0;
      while (str_old[i] != '\0')
      {
        i++;
        if (str_old[i] == '-')
        {
          break;
        }
        str_num1 = str_num1 + str_old[i];
      }
      if (str_num1 != str_num)
        str_new = str_new + '+' + str_num1 + '-';
      else
      {
        flag = 1;
      }
    }
    i++;
  }
  File myFile1 = LittleFS.open("/record.txt", "w");
  i = 0;
  if (myFile1)
  {
    while (str_new[i] != '\0')
    {
      myFile1.print(str_new[i]);
      i++;
    }
    myFile1.close();
  }
}
void writeFile(String s)
{
  File myFile = LittleFS.open("/record.txt", "a");
  if (myFile)
  {
    myFile.print(s);
    myFile.close();
  }
}
