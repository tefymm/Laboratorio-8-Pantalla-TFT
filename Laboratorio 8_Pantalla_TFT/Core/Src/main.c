/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  * Universidad del Valle de Guatemala
 * BE3029 - Electronica Digital 2
 * Laboratorio 8 - Pantalla TFT
 *
 *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

//  Incluye el archivo doctora.h, que contiene la imagen de la doctora convertida a un arreglo de pixeles RGB565
#include "doctora.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//-----------------------------------------------------------------------------------
//                  TAMANO DE PANTALLA Y COLORES
//-----------------------------------------------------------------------------------
// DIMENSIONES DE LA PANTALLA HORIZONTAL 320 x 240 pixeles
#define TFT_WIDTH  320 //columnas
#define TFT_HEIGHT 240 //filas

// Colores RGB565

//Blanco: fondo principal de la interfaz
#define COLOR_BLANCO      0xFFFF

// Negro:  textos y elementos oscuros
#define COLOR_NEGRO       0x0000

// Azul: encabezado y barra inferior del monitor
#define COLOR_AZUL        0x1C9F

//Azul oscuro: textos y detalles dentro de los cuadros
#define COLOR_AZUL_OSCURO 0x11B3

// Rosado: frecuencia cardiaca, corazon y onda ECG
#define COLOR_ROSADO      0xF38E

// Celeste: frecuencia respiratoria e icono de pulmones.
#define COLOR_CELESTE     0x2DDF

// Amarillo: cuadro e indicador de temperatura corporal
#define COLOR_AMARILLO    0xFE40

// Gris: fondo del indicador de temperatura
#define COLOR_GRIS        0xDEFB

// COLORES DE FONDO PARA LOS CUADROS

// Colores claros para el interior de los cuadros
#define FONDO_ROSADO      0xFF3A

// Fondo del cuadro de frecuencia respiratoria
#define FONDO_CELESTE     0xE71F

// Fondo del cuadro de temperatura corporal
#define FONDO_AMARILLO    0xFFF5


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/

/* USER CODE BEGIN 0 */



// Enviar comando al controlador ILI9341
// 8 bits - SPI
void TFT_WriteCommand(uint8_t cmd)
{
	// DC = 0: indicar que se enviara un comando
    HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET);

       // CS = 0: seleccionar la pantalla TFT
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

    // Enviar un comando de 1 byte mediante SPI1
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);

    // CS = 1: finalizar la comunicacion

    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}




// Permite enviar datos mediante SPI1.

void TFT_WriteData(uint8_t *data, uint16_t size)
{
    // DC en alto: indica que se transmitiran datos y no comandos
    HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);

    // CS en bajo: habilita la comunicacion con la pantalla TFT
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

    // Transmite size bytes del arreglo data mediante SPI1
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);

    // CS en alto: deselecciona la pantalla al finalizar el envio
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}




// Inicializar la pantalla en horizontal
void TFT_Init(void)
{

    // RST = 0: inicia el reinicio fisico del controlador
    HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET);
    // Mantiene el reinicio durante 20 milisegundos
    HAL_Delay(20);

    // RST = 1: libera el controlador del estado de reinicio
    HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);
    // Espera 150 ms para permitir la estabilizacion
    HAL_Delay(150);

    // Comando 0x01: realiza un reinicio por software
    TFT_WriteCommand(0x01);
    HAL_Delay(150);

    // Variable de 8 bits para enviar valores de configuracion
    uint8_t data;

    // COMANDO 0x3A: configura el formato de los pixeles
    TFT_WriteCommand(0x3A);
    // 0x55: selecciona RGB565, con 16 bits por pixel
    data = 0x55;
    // Envia el valor de configuracion (1 byte)
    TFT_WriteData(&data, 1);

    // COMANDO 0x36: configura la direccion de memoria
    TFT_WriteCommand(0x36);
    data = 0x28; //orientacion horizontal
    TFT_WriteData(&data, 1);



    // Activar pantalla
    TFT_WriteCommand(0x11);
    // Espera 120 ms antes de encender la visualizacion
    HAL_Delay(120);

    TFT_WriteCommand(0x29);
    // Espera 20 ms para finalizar la inicializacion
    HAL_Delay(20);
}

// Area de dibujo
void TFT_SetAddress(uint16_t x0, uint16_t y0, //  x0, y0: posicion inicial del area.

                    uint16_t x1, uint16_t y1)  //x1, y1: posicion final del area.
{
    // Arreglo para almacenar las coordenadas en 4 bytes
    uint8_t data[4];


    // Seleccionar columnas
    TFT_WriteCommand(0x2A);

    // Separar la coordenada inicial X en byte alto y bajo
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;

    // Separar la coordenada final X en byte alto y bajo
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;

    // Enviar los 4 bytes de las coordenadas X por SPI
    TFT_WriteData(data, 4);

    // Seleccionar filas
    TFT_WriteCommand(0x2B);

    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;

    TFT_WriteData(data, 4);

    // Iniciar escritura de pixeles
    TFT_WriteCommand(0x2C);
}

// Pintar toda la pantalla de un color
void TFT_FillScreen(uint16_t color)
{

    // Reservar memoria para una fila de 320 pixeles (640 bytes)
    uint8_t buffer[TFT_WIDTH * 2];


    // Recorrer los pixeles de una fila para asignarles el color
    for (uint16_t i = 0; i < TFT_WIDTH; i++)
    {
        buffer[i * 2] = color >> 8;
        buffer[i * 2 + 1] = color & 0xFF;
    }


    // Seleccionar toda la pantalla, desde (0,0) hasta (319,239)

    TFT_SetAddress(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

    // DC = 1: indicar que se enviaran datos de pixeles

    HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);

    // CS = 0: seleccionar la pantalla TFT para la transmision

    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);


    // Recorrer las 240 filas de la pantalla
    for (uint16_t y = 0; y < TFT_HEIGHT; y++)
    {
        // Enviar los 640 bytes de la fila mediante SPI1
        HAL_SPI_Transmit(&hspi1, buffer,
                         sizeof(buffer), HAL_MAX_DELAY);
    }

    // CS = 1: finalizar la comunicacion con la pantalla
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

// Mostrar una imagen RGB565
void TFT_DrawImage(uint16_t x, uint16_t y,
                   uint16_t ancho, uint16_t alto,
                   const uint8_t *imagen)
{

    // Verificar que las dimensiones sean validas y que la imagen no sobrepase los limites de 320 x 240 pixeles

    if (ancho == 0 || alto == 0 ||
        (uint32_t)x + ancho > TFT_WIDTH ||
        (uint32_t)y + alto > TFT_HEIGHT)
    {
        // Salir de la funcion si la imagen no cabe en pantalla

        return;
    }
    // Seleccionar el area de la pantalla donde se dibujara,  Se resta 1 porque las coordenadas comienzan desde cero

    TFT_SetAddress(x, y, x + ancho - 1, y + alto - 1);

    // DC = 1: indicar que se enviaran datos de pixeles

    HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);

    // CS = 0: seleccionar la pantalla para transmitir datos

    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

    for (uint16_t fila = 0; fila < alto; fila++)
    {
        HAL_SPI_Transmit(
            &hspi1,
            (uint8_t *)&imagen[(uint32_t)fila * ancho * 2],
            ancho * 2,
            // Esperar hasta completar la transmision
            HAL_MAX_DELAY
        );
    }

    // CS = 1: finalizar la comunicacion con la pantalla

    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

// Dibujar un rectangulo
void TFT_FillRect(uint16_t x, uint16_t y,
                  uint16_t ancho, uint16_t alto,
                  uint16_t color)
{
    // Verificar que el inicio este dentro de la pantalla  y que las dimensiones del rectangulo sean validas

    if (x >= TFT_WIDTH || y >= TFT_HEIGHT ||
        ancho == 0 || alto == 0)
    {
        // No dibujar si las coordenadas son invalidas

        return;
    }
    // Ajustar el ancho si supera el limite horizontal

    if (ancho > TFT_WIDTH - x)
        ancho = TFT_WIDTH - x;

    // Ajustar el alto si supera el limite vertical

    if (alto > TFT_HEIGHT - y)
        alto = TFT_HEIGHT - y;

    uint8_t buffer[TFT_WIDTH * 2];

    for (uint16_t i = 0; i < ancho; i++)
    {
        buffer[i * 2] = color >> 8;
        buffer[i * 2 + 1] = color & 0xFF;
    }

    TFT_SetAddress(x, y,
                   x + ancho - 1,
                   y + alto - 1);

    HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

    for (uint16_t fila = 0; fila < alto; fila++)
    {
        HAL_SPI_Transmit(&hspi1, buffer,
                         ancho * 2, HAL_MAX_DELAY);
    }

    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

// Dibujar un pixel
void TFT_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT)
        return;

    uint8_t pixel[2] = {color >> 8, color & 0xFF};

    TFT_SetAddress(x, y, x, y);

    HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi1, pixel, 2, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

// Dibujar una linea entre dos puntos
void TFT_DrawLine(int x0, int y0,
                  int x1, int y1,
                  uint16_t color)
{
    // Calcular la distancia horizontal absoluta entre los puntos

    int dx = (x1 > x0) ? x1 - x0 : x0 - x1;

    // Definir la direccion horizontal: derecha (+1) o izquierda (-1)

    int sx = (x0 < x1) ? 1 : -1;

    // Calcular la distancia vertical con signo negativo

    int dy = (y1 > y0) ? y0 - y1 : y1 - y0;
    // Definir la direccion vertical: abajo (+1) o arriba (-1)


    int sy = (y0 < y1) ? 1 : -1;

    int error = dx + dy;

    // Repetir hasta alcanzar el punto final de la linea

    while (1)
    {
        // Verificar que las coordenadas esten dentro de la pantalla

        if (x0 >= 0 && y0 >= 0 &&
            x0 < TFT_WIDTH && y0 < TFT_HEIGHT)
        {
            TFT_DrawPixel((uint16_t)x0,
                          (uint16_t)y0, color);
        }

        // Finalizar cuando se alcance el punto final


        if (x0 == x1 && y0 == y1)
            break;

        // Duplicar el error para evaluar el siguiente movimiento

        int e2 = 2 * error;

        // Avanzar horizontalmente cuando el error lo indique

        if (e2 >= dy)
        {
            error += dy;
            x0 += sx;
        }

        // Avanzar verticalmente cuando el error lo indique

        if (e2 <= dx)
        {
            error += dx;
            y0 += sy;
        }
    }
}

// Onda de electrocardiograma
void TFT_DrawECGWave(uint16_t x, uint16_t y,
                     uint16_t color)
{
    // Arreglo de 10 puntos con coordenadas relativas (X, Y)

    const int puntos[][2] = {
        {0, 12}, {8, 12}, {13, 9},
        {18, 15}, {23, 12}, {28, 12},
        {33, 2}, {38, 23}, {43, 12},
        {52, 12}
    };

    // Unir los 10 puntos mediante 9 segmentos de linea

    for (int i = 0; i < 9; i++)
    {
        // Dibujar una linea entre el punto actual y los siguientes

        TFT_DrawLine(x + puntos[i][0],
                     y + puntos[i][1],
                     x + puntos[i + 1][0],
                     y + puntos[i + 1][1],
                     color);
    }
}

// Onda respiratoria con subidas y bajadas suaves
void TFT_DrawRespWave(uint16_t x, uint16_t y,
                      uint16_t color)
{
    const int puntos[][2] = {
        {0, 12}, {3, 9}, {6, 6}, {9, 3},
        {12, 2}, {15, 3}, {18, 6}, {21, 10},
        {24, 14}, {27, 18}, {30, 20},
        {33, 18}, {36, 14}, {39, 10},
        {42, 6}, {45, 3}, {48, 2},
        {51, 6}, {54, 12}
    };

    for (int i = 0; i < 18; i++)
    {
        TFT_DrawLine(x + puntos[i][0],
                     y + puntos[i][1],
                     x + puntos[i + 1][0],
                     y + puntos[i + 1][1],
                     color);
    }
}

// Fuente de letras de 5 x 7 pixeles
// Cada numero indica los pixeles encendidos de una fila


// Almacena los patrones necesarios para dibujar y letras, numeros y algunos simbolos en la TFT.
static const uint8_t fuente5x7[40][7] = {
	    // Letras del abecedario de A hasta Z
    {14,17,17,31,17,17,17}, // A
    {30,17,17,30,17,17,30}, // B
    {14,17,16,16,16,17,14}, // C
    {30,17,17,17,17,17,30}, // D
    {31,16,16,30,16,16,31}, // E
    {31,16,16,30,16,16,16}, // F
    {14,17,16,23,17,17,14}, // G
    {17,17,17,31,17,17,17}, // H
    {31,4,4,4,4,4,31},      // I
    {7,2,2,2,18,18,12},     // J
    {17,18,20,24,20,18,17},// K
    {16,16,16,16,16,16,31},// L
    {17,27,21,21,17,17,17},// M
    {17,25,21,19,17,17,17},// N
    {14,17,17,17,17,17,14},// O
    {30,17,17,30,16,16,16},// P
    {14,17,17,17,21,18,13},// Q
    {30,17,17,30,20,18,17},// R
    {15,16,16,14,1,1,30},  // S
    {31,4,4,4,4,4,4},       // T
    {17,17,17,17,17,17,14},// U
    {17,17,17,17,17,10,4}, // V
    {17,17,17,21,21,21,10},// W
    {17,17,10,4,10,17,17}, // X
    {17,17,10,4,4,4,4},    // Y
    {31,1,2,4,8,16,31},   // Z

    // 0-9
    {14,17,19,21,25,17,14}, // 0
    {4,12,4,4,4,4,14},      // 1
    {14,17,1,2,4,8,31},     // 2
    {30,1,1,14,1,1,30},     // 3
    {2,6,10,18,31,2,2},    // 4
    {31,16,16,30,1,1,30},  // 5
    {14,16,16,30,17,17,14},// 6
    {31,1,2,4,8,8,8},      // 7
    {14,17,17,14,17,17,14},// 8
    {14,17,17,15,1,1,14},  // 9

    // Caracteres especiales
    {0,0,0,0,0,0,0},       // Espacio
    {0,0,0,0,0,12,12},    // Punto
    {0,12,12,0,12,12,0},  // Dos puntos
    {0,0,0,31,0,0,0}      // Guion
};




//Busca el patron de pixeles correspondiente
//letra, numero o simbolo de la fuente 5x7.
const uint8_t *TFT_GetGlyph(char letra)
{
    // Caracteres disponibles en la fuente, en orden

    static const char caracteres[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .:-";

    // Convertir minusculas a mayusculas
    if (letra >= 'a' && letra <= 'z')
        letra = letra - 'a' + 'A';

    // Buscar el caracter dentro de las 40 posiciones

    for (uint8_t i = 0; i < 40; i++)
    {
        // Comparar el caracter solicitado con el actual

        if (caracteres[i] == letra)
            return fuente5x7[i];
    }

    // Caracter no reconocid -> espacio
    return fuente5x7[36];
}



// Dibujar una letra de la fuente 5x7
void TFT_DrawChar(uint16_t x, uint16_t y,
                  char letra, uint16_t color,
                  uint8_t escala)
{

    // Obtener el patron de la letra seleccionada

    const uint8_t *glifo = TFT_GetGlyph(letra);

    if (escala == 0)
        return;

    // Recorrer las 7 filas del caracter

    for (uint8_t fila = 0; fila < 7; fila++)
    {
        // Recorrer las 5 columnas de cada fila

        for (uint8_t columna = 0; columna < 5; columna++)
        {
            if (glifo[fila] & (1U << (4 - columna)))
            {
                // Dibujar el pixel mediante un rectangulo

                TFT_FillRect(
                    x + columna * escala,
                    y + fila * escala,
                    escala,
                    escala,
                    color
                );
            }
        }
    }
}

/* Escribir una cadena de texto */

//Permite escribir palabras y numeros en la pantalla

void TFT_DrawText(uint16_t x, uint16_t y,
                  const char *texto,
                  uint16_t color, uint8_t escala)
{
    while (*texto != '\0')
    {
        // Dibujar el caracter actual en la posicion indicada

        TFT_DrawChar(x, y, *texto, color, escala);

        // Avanzar a la siguiente letra
        x += 6 * escala;
        texto++;
    }
}

/* Dibujar la letra I con tilde */
void TFT_DrawIConTilde(uint16_t x, uint16_t y,
                       uint16_t color, uint8_t escala)
{
    // Dibujar la letra I
    TFT_DrawChar(x, y, 'I', color, escala);

    // Agregar el acento encima
    // Se utiliza un rectangulo de 2 x 1 pixeles escalados

    TFT_FillRect(x + 3 * escala,
                 y - 2 * escala,
                 2 * escala, escala, color);
}

/* Dibujar el simbolo de grados */
void TFT_DrawGrado(uint16_t x, uint16_t y,
                   uint16_t color, uint8_t escala)
{
    // Dibujar el borde superior del simbolo

    TFT_FillRect(x + escala, y,
                 3 * escala, escala, color);

    // Dibujar el borde izquierdo

    TFT_FillRect(x, y + escala,
                 escala, 2 * escala, color);

    // Dibujar el borde derecho

    TFT_FillRect(x + 4 * escala, y + escala,
                 escala, 2 * escala, color);

    // Dibujar el borde inferior para cerrar la figura

    TFT_FillRect(x + escala, y + 3 * escala,
                 3 * escala, escala, color);
}

// Dibujar un corazon sencillo con pixeles
void TFT_DrawHeart(uint16_t x, uint16_t y,
                   uint16_t color)
{

    // Patron de bits que representa la figura del corazon

    const uint16_t corazon[9] = {
        0x0CC6, 0x1FFE, 0x3FFF,
        0x3FFF, 0x1FFE, 0x0FFC,
        0x07F8, 0x03F0, 0x01E0
    };

    for (uint8_t fila = 0; fila < 9; fila++)
    {
        for (uint8_t col = 0; col < 14; col++)
        {
            if (corazon[fila] & (1U << (13 - col)))
            {

                // Dibujar un pixel en la posicion indicada

                TFT_FillRect(x + col, y + fila,
                             1, 1, color);
            }
        }
    }
}

// Dibujar un icono sencillo de pulmones
void TFT_DrawLungs(uint16_t x, uint16_t y,
                   uint16_t color)
{
    // Traquea
    TFT_FillRect(x + 13, y, 3, 10, color);

    // Pulmon izquierdo
    TFT_FillRect(x + 3, y + 8, 9, 13, color);
    TFT_FillRect(x + 5, y + 5, 7, 4, color);
    TFT_FillRect(x + 6, y + 20, 6, 3, color);

    // Pulmon derecho
    TFT_FillRect(x + 17, y + 8, 9, 13, color);
    TFT_FillRect(x + 17, y + 5, 7, 4, color);
    TFT_FillRect(x + 17, y + 20, 6, 3, color);
}

// Dibujar un termometro sencillo
void TFT_DrawThermometer(uint16_t x, uint16_t y)
{
    // Contorno
    TFT_FillRect(x + 7, y, 11, 29, COLOR_AZUL_OSCURO);

    // Interior blanco
    TFT_FillRect(x + 10, y + 3, 5, 25, COLOR_BLANCO);

    // Columna de temperatura
    TFT_FillRect(x + 11, y + 11, 3, 17, COLOR_ROSADO);

    // Bulbo inferior
    TFT_FillRect(x + 4, y + 26, 17, 12, COLOR_AZUL_OSCURO);
    TFT_FillRect(x + 7, y + 28, 11, 8, COLOR_ROSADO);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  // Inicializar pantalla
  TFT_Init();

  // Fondo blanco
  TFT_FillScreen(COLOR_BLANCO);

  // ======================================
  // ENCABEZADO VITACHECK
  // ======================================

  // Barra superior azul
  TFT_FillRect(5, 5, 310, 38, COLOR_AZUL);

  // Cruz medica roja
  TFT_FillRect(20, 14, 8, 21, COLOR_ROSADO);
  TFT_FillRect(14, 20, 20, 8, COLOR_ROSADO);

  // Nombre del monitor
  TFT_DrawText(76, 9, "VITA",
               COLOR_BLANCO, 2);

  TFT_DrawText(124, 9, "CHECK",
               COLOR_ROSADO, 2);

  // Subtitulo
  TFT_DrawText(81, 29, "SIGNOS VITALES",
               COLOR_BLANCO, 1);

  // Fecha
  TFT_DrawText(275, 18, "2026",
               COLOR_BLANCO, 1);

  // ======================================
  // TRES CUADROS DE SIGNOS VITALES
  // ======================================

  // Cuadro 1: Frecuencia cardiaca
  TFT_FillRect(102, 48, 65, 132, COLOR_ROSADO);
  TFT_FillRect(105, 51, 59, 126, FONDO_ROSADO);

  // Cuadro 2: Frecuencia respiratoria
  TFT_FillRect(175, 48, 65, 132, COLOR_CELESTE);
  TFT_FillRect(178, 51, 59, 126, FONDO_CELESTE);

  // Cuadro 3: Temperatura corporal
  TFT_FillRect(248, 48, 67, 132, COLOR_AMARILLO);
  TFT_FillRect(251, 51, 61, 126, FONDO_AMARILLO);

  // ======================================
  // IMAGEN DE LA DOCTORA
  // ======================================

  TFT_DrawImage(5, 48,
                DOCTORA_WIDTH,
                DOCTORA_HEIGHT,
                doctora_rgb565);

  // ======================================
  // CUADRO 1: FRECUENCIA CARDIACA
  // ======================================

  // Titulo
  TFT_DrawText(119, 57, "FREC.",
               COLOR_AZUL_OSCURO, 1);
  // Escribir CARDÍACA con tilde
  TFT_DrawText(110, 68, "CARD",
               COLOR_AZUL_OSCURO, 1);

  TFT_DrawIConTilde(134, 68,
                     COLOR_AZUL_OSCURO, 1);

  TFT_DrawText(140, 68, "ACA",
               COLOR_AZUL_OSCURO, 1);

  // Icono de corazon
  TFT_DrawHeart(126, 86, COLOR_ROSADO);

  // Valor cardiaco
  TFT_DrawText(122, 112, "72",
               COLOR_AZUL_OSCURO, 2);

  // Unidad BPM
  TFT_DrawText(125, 133, "BPM",
               COLOR_AZUL_OSCURO, 1);

  // Onda cardiaca ECG
  TFT_DrawECGWave(109, 151, COLOR_ROSADO);

  // ======================================
  // CUADRO 2: FRECUENCIA RESPIRATORIA
  // ======================================

  // Titulo
  TFT_DrawText(192, 57, "FREC.",
               COLOR_AZUL_OSCURO, 1);

  TFT_DrawText(190, 68, "RESP.",
               COLOR_AZUL_OSCURO, 1);

  // Icono de pulmones
  TFT_DrawLungs(193, 84, COLOR_CELESTE);

  // Frecuencia respiratoria
  TFT_DrawText(195, 112, "16",
               COLOR_AZUL_OSCURO, 2);

  // Unidad rpm
  TFT_DrawText(198, 133, "rpm",
               COLOR_AZUL_OSCURO, 1);

  // Onda respiratoria
  TFT_DrawRespWave(180, 151, COLOR_CELESTE);

  // ======================================
  // CUADRO 3: TEMPERATURA CORPORAL
  // ======================================

  // Titulo
  TFT_DrawText(266, 57, "TEMP.",
               COLOR_AZUL_OSCURO, 1);

  TFT_DrawText(257, 68, "CORPORAL",
               COLOR_AZUL_OSCURO, 1);

  // Icono de termometro
  TFT_DrawThermometer(267, 82);

  // Temperatura
  TFT_DrawText(258, 124, "36.8",
               COLOR_AZUL_OSCURO, 2);

  // Mostrar grados Celsius
  TFT_DrawGrado(265, 143,
                COLOR_AZUL_OSCURO, 1);

  TFT_DrawText(271, 143, "C",
               COLOR_AZUL_OSCURO, 1);

  // Indicador de temperatura
  TFT_FillRect(257, 161, 50, 9, COLOR_GRIS);
  TFT_FillRect(259, 163, 32, 5, COLOR_AMARILLO);

  // ======================================
  // BARRA INFERIOR: ESTADO DEL PACIENTE
  // ======================================

  // Barra azul inferior
  TFT_FillRect(5, 190, 310, 38, COLOR_AZUL);

  // Corazon decorativo
  TFT_DrawHeart(22, 203, COLOR_ROSADO);

  // Estado del paciente
  TFT_DrawText(61, 198, "ESTADO:   PACIENTE ESTABLE ",
               COLOR_BLANCO, 1);

  // Eslogan de VitaCheck
      TFT_DrawText(72, 213, "CERCA DE TI, CUIDANDO TU SALUD",
                   COLOR_BLANCO, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : TFT_DC_Pin */
  GPIO_InitStruct.Pin = TFT_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TFT_DC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TFT_RST_Pin */
  GPIO_InitStruct.Pin = TFT_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TFT_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TFT_CS_Pin */
  GPIO_InitStruct.Pin = TFT_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TFT_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
