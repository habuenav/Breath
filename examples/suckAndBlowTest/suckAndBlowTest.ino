// FILE: suckAndBlow.ino
// AUTHOR: Holman Buenaventura
// PURPOSE: test bidirectional reads.

#include <Breath.h>

Breath myBreath(5, 18);

int minbreath;
int maxbreath;
int lastbreath;

void setup()
{
    Serial.begin(115200);
    Serial.println("Test of Breath (Suck and Blow)");
    // Configuramos el rango bidireccional (-127 a 127)
    myBreath.setMinOut(-127);
    myBreath.setMinRead(-255);
    myBreath.setMaxOut(127);
    myBreath.setMaxRead(255);
    minbreath = maxbreath = lastbreath = myBreath.read();    
}

void loop()
{
  int breath = myBreath.read();                          

  if(breath != lastbreath)
  {
    Serial.print("breath="); Serial.print(breath);
    if(breath < minbreath) minbreath = breath;
    if(breath > maxbreath) maxbreath = breath;
    Serial.print("\tmin="); Serial.print(minbreath);
    Serial.print("\tmax="); Serial.print(maxbreath);
    
    // Pestaña de separación para la gráfica
    Serial.print("\t\t");
    // Map current value (-127 a 127) to scale (ej. max 25 characters by side)
    int numChars = (int)((float)breath / 127.0f * 25);
     if (numChars < 0) {
      for (int i = 0; i < (25 + numChars); i++) Serial.print(" ");
      for (int i = 0; i < -numChars; i++) Serial.print("-"); //suck indicator
      Serial.print("|"); // (central point)
    } 
    else if (numChars > 0) {
      for (int i = 0; i < 25; i++) Serial.print(" ");
      Serial.print("|"); // (central point)
      for (int i = 0; i < numChars; i++) Serial.print("+"); //blow indicator
    } 
    else {
      // idle (0)
      for (int i = 0; i < 25; i++) Serial.print(" ");
      Serial.print("|");
    }
    Serial.println();
    lastbreath = breath;
  }
}
