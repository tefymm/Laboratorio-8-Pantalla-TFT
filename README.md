
# Laboratorio 8 - Electrónica Digital 2

**Universidad del Valle de Guatemala**  
**Estudiante:** Stephany Mejía  
**Curso:** Electrónica Digital 2  
**Año:** 2026

## Descripción del proyecto

Se desarrolló **VitaCheck**, una interfaz gráfica para la
visualización de signos vitales simulados, utilizando una
tarjeta STM32 NUCLEO-F446RE y una pantalla TFT ILI9341
de 2.4 pulgadas.

La interfaz se diseñó para representar tres parámetros
fisiológicos:

- **Frecuencia cardíaca:** 72 BPM.
- **Frecuencia respiratoria:** 16 rpm.
- **Temperatura corporal:** 36.8 °C.

También se incluyeron una ilustración de una doctora,
íconos personalizados, ondas fisiológicas ilustrativas
y un indicador del estado del paciente.

## Materiales utilizados

- Tarjeta STM32 NUCLEO-F446RE.
- Pantalla TFT ILI9341 de 2.4 pulgadas.
- Resistencia de 1 kΩ para la retroiluminación.
- Protoboard y cables de conexión.
- STM32CubeIDE y STM32CubeMX.

## Conexiones de la pantalla TFT

Se utilizó el periférico SPI1 del STM32 para establecer
la comunicación con la pantalla.

| Pin TFT ILI9341 | Pin STM32 | Función |
|-----------------|-----------|---------|
| VCC | 3V3 | Alimentación |
| GND | GND | Tierra |
| CS | PB6 | Selección de pantalla |
| RESET / RST | PA9 | Reinicio de pantalla |
| DC / RS | PC7 | Selección de comando o dato |
| SDI / MOSI | PA7 | Transmisión de datos |
| SCK / CLK | PA5 | Señal de reloj SPI1 |
| SDO / MISO | PA6 | Recepción SPI1 (opcional) |
| LED | 3V3 | Retroiluminación con resistencia de 1 kΩ |

## Configuración de SPI1

Se configuró SPI1 con los siguientes parámetros:

- **Modo:** Master.
- **Tamaño de datos:** 8 bits.
- **Prescaler:** 16.
- **Polaridad del reloj:** Low.
- **Fase del reloj:** First Edge.
- **NSS:** Software.

## Desarrollo del programa

Se implementaron funciones en lenguaje C utilizando
la biblioteca HAL de STM32.

Las funciones principales permitieron:

- Inicializar y configurar la pantalla ILI9341.
- Transmitir comandos y datos mediante SPI1.
- Establecer coordenadas de dibujo.
- Pintar el fondo y dibujar rectángulos.
- Dibujar píxeles y líneas mediante el algoritmo de Bresenham.
- Representar ondas cardíacas y respiratorias.
- Mostrar letras y números con una fuente de 5 x 7 píxeles.
- Dibujar íconos de corazón, pulmones y termómetro.
- Mostrar una ilustración en formato RGB565.

## Funcionamiento

Al iniciar el programa, se configuró la pantalla TFT
con una resolución de 320 x 240 píxeles y formato
de color RGB565.

Posteriormente, se dibujó la interfaz VitaCheck,
que incluyó un encabezado, tres secciones para
los signos vitales y un mensaje de estado del paciente.

Los valores fisiológicos fueron definidos directamente
en el código, por lo que no se utilizaron sensores
para realizar mediciones reales.

## Resultado

Se logró implementar una interfaz gráfica personalizada
para visualizar signos vitales simulados mediante una
pantalla TFT controlada por SPI1.

El proyecto permitió aplicar conceptos de comunicación SPI,
manejo de GPIO, transmisión de datos y programación de
funciones gráficas en un microcontrolador STM32.
