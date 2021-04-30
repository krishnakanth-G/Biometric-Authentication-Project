String Day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
String Month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

void timeDisplay()
{
  digitalClockDisplay();
  //delay(1000);
}
void digitalClockDisplay()
{
  int d = weekday();
  Serial.print("@ ");
  Serial.print(Day[d - 1]);
  Serial.print(",");
  printzero(day());
  Serial.print(" ");
  int m = month();
  Serial.print(Month[m - 1]);
  Serial.print(" ");
  Serial.print(year());
  Serial.print(" ");
  Serial.print("  ");
  printzero(hour());
  printDigits(minute());
  //printDigits(second());
  Serial.println();
}
void printDigits(int digits)
{
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
void printzero(int a)
{
  if (a < 10)
    Serial.print('0');
  Serial.print(a);
}
