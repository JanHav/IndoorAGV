/*****
Doel: De afstand van een punt tot een punt bepalen
Parameters:
float x1      float y1      float x2      floaty2
Return value:
float
*****/
float AfstandPuntPunt (float x1, float y1, float x2, float y2)
{
  float afstand;
  afstand = sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
  return afstand;
}

