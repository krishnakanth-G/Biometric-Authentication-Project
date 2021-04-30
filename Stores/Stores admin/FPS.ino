uint32_t Verify()
{
  bool c = false;
  int id = 300;
  unsigned long stime,ttime;
  ttime=millis();
  while (!fps.IsPressFinger())
  {
    stime = millis();
    if(stime>ttime+10000)
      return -10;
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
        return id;
      }
      Serial.print(F("Finger print found\nid : "));
      uint32_t i = readId(id);
      Serial.println(i);
      return i;
    }
    else
      Serial.println(F("No match found"));
  }
  else
  {
    Serial.println(F("Failed to capture"));
    Serial.println(F("try agian"));
  }
  return -1;
}

int Enroll(String reg,int i)
{
  int id = 1;
  unsigned long stime,ttime;
  bool is_exists = true;
  if (i == 0 || i == 199)
  {
    id = i;
  }
  else
  {
    String reg1;
    for (int i = 1; i < 199; i++)
    {
      reg1 = readId(i);
      if (String(reg1) == reg)
      {
        Serial.println("Registration Number already exsits");
        return -4;
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
  ttime=millis();
  while (!fps.IsPressFinger())
  {
    stime = millis();
    if(stime>ttime+10000)
      return -10;
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
        stime = millis();
        if(stime>ttime+10000)
          return -10;
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
        stime = millis();
        if(stime>ttime+10000)
          return -10;
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
           if(Status == 2)
              return -2;
           if(Status == 3)
              return -3;
        }
      }
    }
  }
  return -1;
}

int Deleteid(String reg1)
{
  int id = -1;
  bool c = false;
  uint32_t reg = 0;
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
      return -1;
    }
    fps.DeleteID(id);
    Serial.print(F("Finger print id : "));
    Serial.print(reg1);
    Serial.println(F(" is deleted"));
    //readFile();
    return 1;
  }
  else
  {
    Serial.print(F("No Finger Found with id :"));
    Serial.println(reg1);
  }
  return -1;
}

int adminChange()
{
  int id = 300;
  int i = 0;
  bool is_exists = fps.CheckEnrolled(0);
  if (is_exists)
  {
      i = Enroll("",199);
      if (i!= -1 && i!= -10 && i != -2 && i !=-3)
      {
        fps.DeleteID(0);
        Serial.println("Success");
        return 1;
      }
  }
  else
  {
      i = Enroll("",0);
      if (i!= -1 && i!= -10 && i != -2 && i !=-3)
      {
        fps.DeleteID(199);
        Serial.println("Success");
        return 1;
      }
  }
  return i;
}
String getId()
{
  String record="";
  uint32_t reg=0;
  bool is_exists = false;
  for(int i=0 ;i<200;i++)
  {
    if(i== 0 || i == 199)
    {
      is_exists=fps.CheckEnrolled(i);
      if(is_exists)
      {
        record+="Admin, ";
      }
    }
    else
    {
      is_exists=fps.CheckEnrolled(i);
      if(is_exists)
      {
        reg = readId(i);
        record+=String(reg)+", ";
      }
    }
  }
  return record;
}
