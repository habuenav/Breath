# Breath

`Breath` es una librería que facilita el uso del sensor de presión MPS20N0040D para la detección de soplo en proyectos como instrumentos electrónicos de viento (EWI).
<img src='https://static.wixstatic.com/media/99ba69_f4d46c2102c24d46a3c3661d1a71ab0c~mv2.jpg/v1/fill/w_668,h_581,al_c,q_85/99ba69_f4d46c2102c24d46a3c3661d1a71ab0c~mv2.jpg' title=''/>

## Características

- Lectura de valores de soplo con el sensor MPS20N0040D.
- Calibración automática del valor inicial del sensor.
- Ajuste de sensibilidad mediante un factor de resistencia.
- Configuración de límites de lectura y de salida máxima para adaptarse a las necesidades del proyecto.

## Requisitos

- **Placa**: Probado en placas Esp32, Esp32-S2, Esp32-C3 y Esp32-S3
- **Sensor**: MPS20N0040D
- **IDE de Arduino**: Recomendado 1.8.10 o superior

## Instalación

1. Descarga el archivo ZIP de la librería desde [GitHub](https://github.com/habuenav/Breath).
2. En el IDE de Arduino, ve a **Sketch** > **Include Library** > **Add .ZIP Library...**.
3. Selecciona el archivo ZIP descargado para instalar la librería.

O tambien puedes Extraer el archivo ZIP del paso 1 en tu directorio de librerías de Arduino.

### Codigo de ejemplo
```
#include <Breath.h>
#define OUTPUT_PIN 13
#define CLOCK_PIN 12
Breath breath(OUTPUT_PIN, CLOCK_PIN);

void setup() {
  Serial.begin(115200);
  breath.setResistance(3);  // Ajustar la resistencia de soplo (1 = mínima, 5 = máxima).
  breath.setMaxOut(127);    // Establecer el valor máximo de salida.
}

void loop() {
  int16_t breathValue = breath.read();
  Serial.println(breathValue);
  delay(100);
}
```

## API
### Constructor

<b> Breath(byte output_pin, byte clock_pin): </b> Inicializa la clase Breath con el pin de salida de datos del sensor y el pin de reloj.

### Métodos

* <b> int16_t read():</b> Lee el valor del sensor, aplica la calibración y devuelve un valor ajustado que varía entre 0 y el valor máximo configurado.
* <b> void setResistance(uint8_t level):</b> Ajusta la sensibilidad del sensor, donde level puede ser un valor de 1 (mínima resistencia) a 5 (máxima resistencia).
* <b> void setMaxOut(uint8_t maxOut):</b> Define el valor máximo de salida que se obtendrá al leer el sensor.
* <b> void setMaxRead(uint16_t mRead):</b> Establece el límite superior de la lectura antes de aplicar el ajuste de salida.

### Notas

* El sensor de presión MPS20N0040D necesita una alimentación estable y debe ser conectado adecuadamente al ESP32.
* La librería incluye una función para ajustar la resistencia de soplo, permitiendo adaptarla a diferentes niveles de sensibilidad según la necesidad del proyecto.
* Los valores que establescas por medio de las funciones setMaxOut y setMaxRead afectan la sensibilidad y resistencia
* El valor recomendado para setMaxOut es 127 ya que es el maximo que se envia como parametro de velocidad segun el estandar MIDI
* El valor recomendaddo para setMaxRead es 255 pero puede ajustarse en un rango de 200 a 500, puedes probar con cual funciona mejor.

## Ejemplos

En la carpeta de examples del repositorio, encontrarás ejemplos adicionales que muestran cómo utilizar la librería Breath con el ESP32.

## Contribuciones

Las contribuciones son bienvenidas. Si encuentras un problema o tienes una sugerencia, por favor abre un issue o envía un pull request.
