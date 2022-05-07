#include <debug.hpp>

byte printBuffer64(byte *buffer)
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      int count = (i * 8) + j;
      Serial.print(buffer[count]);
      Serial.print(' ');
    }
    Serial.println();
  }
  return 0;
}
