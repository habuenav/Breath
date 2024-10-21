#include <Breath.h>

Breath myBreath(5,18);

int minbreath;
int maxbreath;
int lastbreath;

void setup()
{
  Serial.begin(115200);
  Serial.println("Test of Breath");
  minbreath = maxbreath = lastbreath =  myBreath.read();                          
}

void loop()
{

  int breath =  myBreath.read();                          

  if(breath != lastbreath)
  {
    Serial.print("breath="); Serial.print(breath);
  
    if(breath < minbreath) minbreath = breath;
    if(breath > maxbreath) maxbreath = breath;
  
    Serial.print("\tmin="); Serial.print(minbreath);
    Serial.print("\tmax="); Serial.print(maxbreath);

    // plot a little graph
    int reading = (int)((double)(breath - minbreath)/(maxbreath - minbreath + 1) * 50);
    Serial.print("\t\t");

    for(int i = 0; i < reading; i++) Serial.print("+");

    Serial.println();
    lastbreath = breath;
  }
    
}
