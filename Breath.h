#ifndef Breath_h
#define Breath_h

// Definición de macros para simplificar el uso de los pines de reloj y espera.
// switchPin: genera un pulso de reloj bajando y subiendo el pin CLOCK_PIN.
#define switchPin digitalWrite(CLOCK_PIN, HIGH); digitalWrite(CLOCK_PIN, LOW)
// waitPin: espera a que el pin OUT_PIN se ponga en estado bajo (lectura lista).
#define waitPin while(digitalRead(OUT_PIN))

class Breath {
  private:
    byte CLOCK_PIN;         // Pin de reloj para leer datos del sensor.
    byte OUT_PIN;           // Pin de salida del sensor de presión.
    // Unión que facilita el manejo de los datos leídos como un valor de 16 bits
    union {
      uint16_t value;
      uint8_t bytes[2];
    } result;
    int16_t initRead;       // Valor inicial de calibración del sensor.
    int16_t finalRead;      // Valor final leído, ajustado por la resistencia.
    int16_t maxRead = 0;    // Valor máximo leído (para cálculo).
    int16_t maxLimitRead = 255; // Límite máximo permitido para la lectura.
    uint8_t maxOUT = 127;   // Valor máximo de salida permitido.
    float resistanceFactor; // Factor que ajusta la sensibilidad de la lectura al soplo.

  public:
    // Constructor de la clase, recibe los pines de salida y reloj.
    Breath(byte output_pin, byte clock_pin);
    // Función que lee el valor de presión del sensor, lo ajusta y devuelve.
    int16_t read();
    // Establece el nivel de resistencia del soplo (1-5).
    void setResistance(uint8_t level);
    // Establece el valor máximo de salida.
    void setMaxOut(uint8_t maxOut);
    // Establece el valor máximo de lectura antes de limitar.
    void setMaxRead(uint16_t mRead);
};

// Implementación del constructor.
Breath::Breath(byte output_pin, byte clock_pin) {
  CLOCK_PIN = clock_pin;
  OUT_PIN = output_pin;
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(OUT_PIN, INPUT);
  
  // Espera a que el pin de salida esté listo para leer.
  waitPin;
  // Lee los bytes de datos del sensor utilizando shiftIn.
  result.bytes[1] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  result.bytes[0] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza una lectura adicional para estabilizar el valor.
  shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza tres pulsos de reloj para finalizar la lectura.
  switchPin;
  switchPin;
  switchPin;
  // Ajusta la lectura inicial (calibración) y guarda el valor.
  initRead = (result.value >> 2);
  // Inicializa el factor de resistencia con un valor por defecto.
  resistanceFactor = 1.0f; // Sin modificar la sensibilidad de la lectura.
}

// Establece el nivel de resistencia del soplo, con un rango de 1 a 5.
void Breath::setResistance(uint8_t level) {
  // Asegura que el nivel esté entre 1 (mínima resistencia) y 5 (máxima resistencia).
  level = constrain(level, 1, 5);
  // Define el factor de resistencia para ajustar la sensibilidad de la lectura.
  resistanceFactor = 1 / (1.0f + (level - 1) * 0.5f); 
  // Esto ajusta el factor de 1.0 (sin cambio) hasta 0.2 (mayor resistencia).
}

// Establece el valor máximo de salida permitido.
void Breath::setMaxOut(uint8_t maxOut) {
  maxOUT = maxOut;
}

// Establece el valor máximo de lectura antes de limitar.
void Breath::setMaxRead(uint16_t mRead) {
  maxLimitRead = mRead;
}

// Función para leer la presión del sensor y ajustarla.
inline int16_t IRAM_ATTR Breath::read() {
  // Espera a que el sensor esté listo para proporcionar una nueva lectura.
  waitPin;
  // Lee los bytes de datos del sensor.
  result.bytes[1] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  result.bytes[0] = shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza una lectura adicional para estabilizar el valor.
  shiftIn(OUT_PIN, CLOCK_PIN, MSBFIRST);
  // Realiza tres pulsos de reloj para completar la lectura.
  switchPin;
  switchPin;
  switchPin;
  // Ajusta la lectura final restando el valor de calibración inicial y aplicando el factor de resistencia.
  finalRead = ((result.value >> 2) - initRead) * resistanceFactor;
  // Limita el valor de la lectura entre 0 y el máximo definido.
  // Luego mapea la lectura a un valor de 0 a maxOUT.
  return map((finalRead < 0 ? 0 : finalRead > maxLimitRead ? maxLimitRead : finalRead), 0, maxLimitRead, 0, maxOUT);
}
#endif
