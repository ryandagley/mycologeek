#include <Wire.h>
#include "ssd1306.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"


#define VCCSTATE SSD1306_SWITCHCAPVCC
#define WIDTH     128
#define HEIGHT     64
#define PAGES       8

#define OLED_RST    9 
#define OLED_DC     8
#define OLED_CS    10
#define SPI_MOSI   11    /* connect to the DIN pin of OLED */
#define SPI_SCK    13     /* connect to the CLK pin of OLED */

#define DHTPIN 2 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float LIGHT;
float MOISTURE;
float TEMP;
float HUMID;

int counter=0;
int scroll=23;


uint8_t oled_buf[WIDTH * HEIGHT / 8];

const uint8_t percent [] PROGMEM = {
    0x70, 0x20, 
    0xd8, 0x60, 
    0xcc, 0x40, 
    0x8c, 0xc0, 
    0xcd, 0x80, 
    0xd9, 0x00, 
    0x73, 0x38, 
    0x02, 0x6c, 
    0x06, 0xcc, 
    0x04, 0xc4, 
    0x08, 0xc4, 
    0x18, 0x4c, 
    0x10, 0x38, 
    0x00, 0x00
};

const uint8_t mmenu [] PROGMEM ={
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x03, 0xff, 0x00, 0x00, 0x00,
    0x00, 0x0f, 0xc7, 0xe0, 0x00, 0x00,
    0x00, 0x38, 0x00, 0x78, 0x00, 0x00,
    0x00, 0xe0, 0x00, 0x1c, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x80, 0x00,
    0x06, 0x00, 0x00, 0x00, 0xc0, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0xc0, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x60, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x30, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x30, 0x00,
    0x30, 0x00, 0x00, 0x00, 0x18, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x18, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x08, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x08, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x0c, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x0c, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x0c, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x0c, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x0c, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x0c, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x0c, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x08, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x18, 0x00,
    0x30, 0x00, 0x00, 0x00, 0x18, 0x00,
    0x30, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x30, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x60, 0x00,
    0x0c, 0x00, 0x00, 0x00, 0x60, 0x00,
    0x06, 0x00, 0x00, 0x00, 0xc0, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x80, 0x00,
    0x03, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x0e, 0x00, 0x00,
    0x00, 0x70, 0x00, 0x1c, 0x00, 0x00,
    0x00, 0x3c, 0x00, 0xf0, 0x00, 0x00,
    0x00, 0x0f, 0x00, 0xc0, 0x00, 0x00,
    0x00, 0x00, 0xfe, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t FAHRENHEIT [] PROGMEM = {
    0x1F, 0x1c,
    0x1F, 0x14, 
    0x18, 0x1c, 
    0x1E, 0x00, 
    0x1E, 0x00, 
    0x18, 0x00, 
    0x18, 0x00, 
    0x18, 0x00,
    0x18, 0x00, 
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00
};

const uint8_t SAD [] PROGMEM = {
      0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x01, 0xff, 0xc0, 0x00, 
    0x00, 0x0f, 0xc1, 0xf8, 0x00, 
    0x00, 0x1c, 0x00, 0x1e, 0x00, 
    0x00, 0x70, 0x00, 0x07, 0x00, 
    0x00, 0xe0, 0x00, 0x01, 0x80, 
    0x01, 0x80, 0x00, 0x00, 0xc0, 
    0x03, 0x00, 0x00, 0x00, 0x60, 
    0x06, 0x00, 0x00, 0x00, 0x30, 
    0x06, 0x00, 0x00, 0x00, 0x18, 
    0x0c, 0x3f, 0x80, 0x7e, 0x18, 
    0x0c, 0x71, 0xc1, 0xc3, 0x8c, 
    0x18, 0x40, 0x41, 0x81, 0x8c, 
    0x18, 0x00, 0x00, 0x00, 0x04, 
    0x10, 0x00, 0x00, 0x00, 0x06, 
    0x30, 0x00, 0x00, 0x03, 0x86, 
    0x30, 0x00, 0x00, 0x03, 0xc2, 
    0x30, 0x00, 0x7f, 0x03, 0x62, 
    0x30, 0x01, 0xff, 0xc3, 0x22, 
    0x30, 0x03, 0x80, 0x63, 0x32, 
    0x33, 0x06, 0x00, 0x33, 0x32, 
    0x37, 0x06, 0x00, 0x1b, 0x32, 
    0x3f, 0x0c, 0x00, 0x19, 0xb6, 
    0x3b, 0x08, 0x00, 0x0c, 0xe6, 
    0x33, 0x18, 0x00, 0x0c, 0x06, 
    0x23, 0x1b, 0xff, 0xec, 0x04, 
    0x63, 0x1f, 0xff, 0xfc, 0x0c, 
    0x63, 0x00, 0x00, 0x00, 0x0c, 
    0x63, 0x00, 0x00, 0x00, 0x18, 
    0x66, 0x00, 0x00, 0x00, 0x30, 
    0x3f, 0x00, 0x00, 0x00, 0x30, 
    0x0b, 0x80, 0x00, 0x00, 0x60, 
    0x01, 0xc0, 0x00, 0x01, 0xc0, 
    0x00, 0xe0, 0x00, 0x03, 0x80, 
    0x00, 0x38, 0x00, 0x0f, 0x00, 
    0x00, 0x1e, 0x00, 0x3c, 0x00, 
    0x00, 0x07, 0xff, 0xf0, 0x00, 
    0x00, 0x00, 0xff, 0x80, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t SMILE [] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x03, 0xff, 0x00, 0x00, 
    0x00, 0x0f, 0xc7, 0xe0, 0x00, 
    0x00, 0x38, 0x00, 0x78, 0x00, 
    0x00, 0xe0, 0x00, 0x1c, 0x00, 
    0x01, 0xc0, 0x00, 0x07, 0x00, 
    0x03, 0x00, 0x00, 0x03, 0x80, 
    0x06, 0x00, 0x00, 0x01, 0xc0, 
    0x0c, 0x00, 0x00, 0x00, 0xc0, 
    0x0c, 0x00, 0x00, 0x00, 0x60, 
    0x18, 0x00, 0x00, 0x00, 0x30, 
    0x10, 0x00, 0x00, 0x00, 0x30, 
    0x30, 0x00, 0x00, 0x00, 0x18, 
    0x20, 0x00, 0x00, 0x00, 0x18, 
    0x60, 0x7e, 0x01, 0xfc, 0x08, 
    0x60, 0xe7, 0x83, 0x8e, 0x08, 
    0x61, 0x81, 0x83, 0x02, 0x0c, 
    0x60, 0x00, 0x00, 0x00, 0x0c, 
    0x40, 0x00, 0x00, 0x00, 0x0c, 
    0x40, 0x00, 0x00, 0x04, 0x0c, 
    0x40, 0xf0, 0x00, 0x3e, 0x0c, 
    0x60, 0xff, 0x83, 0xfc, 0x0c, 
    0x60, 0x67, 0xff, 0xcc, 0x0c, 
    0x60, 0x60, 0x10, 0x0c, 0x08, 
    0x20, 0x20, 0x00, 0x18, 0x18, 
    0x30, 0x30, 0x00, 0x18, 0x18, 
    0x30, 0x18, 0x00, 0x30, 0x10, 
    0x18, 0x0c, 0x00, 0x60, 0x30, 
    0x18, 0x0e, 0x00, 0xc0, 0x60, 
    0x0c, 0x03, 0x87, 0x80, 0x60, 
    0x06, 0x01, 0xff, 0x00, 0xc0, 
    0x07, 0x00, 0x30, 0x01, 0x80, 
    0x03, 0x80, 0x00, 0x03, 0x00, 
    0x01, 0xc0, 0x00, 0x0e, 0x00, 
    0x00, 0x70, 0x00, 0x1c, 0x00, 
    0x00, 0x3c, 0x00, 0xf0, 0x00, 
    0x00, 0x0f, 0xff, 0xc0, 0x00, 
    0x00, 0x00, 0xfe, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t TEM [] PROGMEM = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 
    0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 
    0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 
    0x00, 0x00, 0x10, 0x18, 0x00, 0x00, 
    0x00, 0x00, 0x1e, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1f, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x18, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1c, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1c, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1c, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1c, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x18, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1f, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1f, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1c, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x1c, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x18, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 
    0x00, 0x00, 0x11, 0x88, 0x00, 0x00, 
    0x00, 0x00, 0x11, 0x88, 0x00, 0x00, 
    0x00, 0x00, 0x11, 0x88, 0x00, 0x00, 
    0x00, 0x00, 0x11, 0x88, 0x00, 0x00, 
    0x00, 0x00, 0x11, 0x88, 0x00, 0x00, 
    0x00, 0x00, 0x31, 0x8c, 0x00, 0x00, 
    0x00, 0x00, 0x71, 0x8e, 0x00, 0x00, 
    0x00, 0x00, 0xc1, 0x83, 0x00, 0x00, 
    0x00, 0x01, 0x81, 0x81, 0x80, 0x00, 
    0x00, 0x01, 0x83, 0xc1, 0x80, 0x00, 
    0x00, 0x03, 0x0f, 0xf0, 0xc0, 0x00, 
    0x00, 0x03, 0x0f, 0xf0, 0xc0, 0x00, 
    0x00, 0x03, 0x1f, 0xf8, 0xc0, 0x00, 
    0x00, 0x03, 0x1f, 0xf8, 0xc0, 0x00, 
    0x00, 0x03, 0x0f, 0xf0, 0xc0, 0x00, 
    0x00, 0x03, 0x0f, 0xf0, 0xc0, 0x00, 
    0x00, 0x01, 0x87, 0xe1, 0x80, 0x00, 
    0x00, 0x01, 0x80, 0x01, 0x80, 0x00, 
    0x00, 0x00, 0xc0, 0x03, 0x00, 0x00, 
    0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 
    0x00, 0x00, 0x38, 0x1c, 0x00, 0x00, 
    0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 
    0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t SAND [] PROGMEM = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x73, 0x80, 
    0x00, 0x00, 0x00, 0x00, 0xe1, 0xc0, 
    0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 
    0x00, 0x00, 0x00, 0x01, 0xc0, 0xe0, 
    0x00, 0xe0, 0x00, 0x03, 0x80, 0x70, 
    0x01, 0xf0, 0x00, 0x03, 0x00, 0x30, 
    0x03, 0xf8, 0x00, 0x07, 0x00, 0x38, 
    0x07, 0xfc, 0x00, 0x06, 0x00, 0x18, 
    0x0f, 0x9e, 0x00, 0x0e, 0x00, 0x1c, 
    0x0f, 0x0f, 0x00, 0x0e, 0x00, 0x1c, 
    0x1e, 0x07, 0x80, 0x0c, 0x00, 0x0c, 
    0x1c, 0x07, 0x80, 0x0c, 0xc0, 0x0c, 
    0x3c, 0x03, 0xc0, 0x0c, 0xe0, 0x0c, 
    0x7c, 0x03, 0xc0, 0x0e, 0xf0, 0x1c, 
    0x78, 0x01, 0xc0, 0x06, 0x78, 0x18, 
    0x78, 0x01, 0xc0, 0x07, 0x18, 0x38, 
    0x78, 0x01, 0xc0, 0x03, 0x80, 0x70, 
    0x79, 0xe1, 0xc0, 0x01, 0xe1, 0xe0, 
    0x79, 0xe1, 0xe0, 0x00, 0xff, 0xc0, 
    0x79, 0xe1, 0xff, 0xf8, 0x3f, 0x00, 
    0x79, 0xc3, 0xff, 0xff, 0x00, 0x00, 
    0x79, 0xc3, 0xff, 0xff, 0xc0, 0x00, 
    0x3d, 0xef, 0xc0, 0x0f, 0xf0, 0x00, 
    0x1f, 0xff, 0x00, 0x01, 0xfc, 0x00, 
    0x1f, 0xfe, 0x00, 0x00, 0x7e, 0x00, 
    0x0f, 0xfc, 0x00, 0x00, 0x1f, 0x00, 
    0x07, 0xf8, 0x00, 0x00, 0x0f, 0x80, 
    0x07, 0xf0, 0x0f, 0xf8, 0x0f, 0x80, 
    0x01, 0xf0, 0x7f, 0xfc, 0x0f, 0x00, 
    0x00, 0xe1, 0xff, 0xf8, 0x1e, 0x00, 
    0x00, 0x63, 0xf8, 0x00, 0x3e, 0x00, 
    0x00, 0x07, 0xe0, 0x00, 0x7c, 0x00, 
    0x00, 0x1f, 0x80, 0x01, 0xf8, 0x00, 
    0x00, 0x1f, 0x80, 0x07, 0xe0, 0x00, 
    0x00, 0x3f, 0xc0, 0x1f, 0xc0, 0x00, 
    0x00, 0x7f, 0xff, 0xff, 0x80, 0x00, 
    0x00, 0xff, 0xff, 0xfe, 0x00, 0x00, 
    0x00, 0xe0, 0xff, 0xf0, 0x00, 0x00, 
    0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t SUN [512] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x80, 0x03, 0x00, 0x02, 0x00, 
    0x00, 0xc0, 0x03, 0x00, 0x06, 0x00, 
    0x00, 0xe0, 0x01, 0x00, 0x0e, 0x00, 
    0x00, 0x70, 0x00, 0x00, 0x1c, 0x00, 
    0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 
    0x00, 0x1c, 0x00, 0x00, 0x70, 0x00, 
    0x00, 0x0c, 0x0f, 0xe0, 0x60, 0x00, 
    0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 
    0x00, 0x00, 0xf0, 0x3c, 0x00, 0x00, 
    0x00, 0x01, 0xc0, 0x0e, 0x00, 0x00, 
    0x00, 0x01, 0x80, 0x07, 0x00, 0x00, 
    0x00, 0x03, 0x00, 0x03, 0x80, 0x00, 
    0x00, 0x03, 0x00, 0x01, 0x80, 0x00, 
    0x00, 0x06, 0x00, 0x01, 0x80, 0x00, 
    0x00, 0x06, 0x00, 0x01, 0xc0, 0x00, 
    0x3f, 0x86, 0x00, 0x00, 0xc3, 0xf8, 
    0x7f, 0xc6, 0x00, 0x00, 0xc7, 0xf8, 
    0x00, 0x06, 0x00, 0x00, 0xc0, 0x00, 
    0x00, 0x06, 0x00, 0x01, 0xc0, 0x00, 
    0x00, 0x07, 0x00, 0x01, 0x80, 0x00, 
    0x00, 0x03, 0x00, 0x01, 0x80, 0x00, 
    0x00, 0x03, 0x80, 0x03, 0x00, 0x00, 
    0x00, 0x01, 0xc0, 0x07, 0x00, 0x00, 
    0x00, 0x00, 0xe0, 0x0e, 0x00, 0x00, 
    0x00, 0x00, 0x7c, 0x7c, 0x00, 0x00, 
    0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 
    0x00, 0x0c, 0x07, 0xc0, 0x60, 0x00, 
    0x00, 0x1c, 0x00, 0x00, 0x70, 0x00, 
    0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 
    0x00, 0x70, 0x00, 0x00, 0x1c, 0x00, 
    0x00, 0xe0, 0x03, 0x00, 0x0e, 0x00, 
    0x00, 0xc0, 0x03, 0x00, 0x06, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00


};

void SADD(void){
      SSD1306_clear(oled_buf);
while (counter<5){
  if ((counter%2)==0)
  { SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 20, SAD, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 19, SAD, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 18, SAD, 40, 40, oled_buf);
    SSD1306_display(oled_buf);}

  else  
  { SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 18, SAD, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 19, SAD, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 20, SAD, 40, 40, oled_buf);
    SSD1306_display(oled_buf);}

    counter++;}
    counter=0;}

void HAPPY(void){
  SSD1306_clear(oled_buf);
while (counter<5){
  if ((counter%2)==0)
  { SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 23, SMILE, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 19, SMILE, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 16, SMILE, 40, 40, oled_buf);
    SSD1306_display(oled_buf);}

  else  
  { SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 16, SMILE, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 19, SMILE, 40, 40, oled_buf);
    SSD1306_display(oled_buf);
    SSD1306_clear(oled_buf);
    SSD1306_bitmap(40, 23, SMILE, 40, 40, oled_buf);
    SSD1306_display(oled_buf);}

    counter++;}
    counter=0;}    
void dsply(int input){
  int n1,n2;
  if (input<10){ SSD1306_char3216(10, 25, '0', oled_buf); switch(input) { case 0 : SSD1306_char3216(26, 25, '0', oled_buf); break; case 1 : SSD1306_char3216(26, 25, '1', oled_buf); 
  break; 
  case 2 : SSD1306_char3216(26, 25, '2', oled_buf); break; case 3 : SSD1306_char3216(26, 25, '3', oled_buf); break; case 4 : SSD1306_char3216(26, 25, '4', oled_buf); 
  break; 
  case 5 : SSD1306_char3216(26, 25, '5', oled_buf); break; case 6 : SSD1306_char3216(26, 25, '6', oled_buf); break; case 7 : SSD1306_char3216(26, 25, '7', oled_buf); 
  break; 
  case 8 : SSD1306_char3216(26, 25, '8', oled_buf); break; case 9 : SSD1306_char3216(26, 25, '9', oled_buf); 
  break; } } else{ n1=input/10; n2=input%10; 
  switch(n1) { case 0 : SSD1306_char3216(10, 25, '0', oled_buf); 
  break; 
  case 1 : SSD1306_char3216(10, 25, '1', oled_buf); break; case 2 : SSD1306_char3216(10, 25, '2', oled_buf); 
  break; case 3 : SSD1306_char3216(10, 25, '3', oled_buf); 
  break; case 4 : SSD1306_char3216(10, 25, '4', oled_buf); break; case 5 : SSD1306_char3216(10, 25, '5', oled_buf); 
  break; case 6 : SSD1306_char3216(10, 25, '6', oled_buf); break; 
  case 7 : SSD1306_char3216(10, 25, '7', oled_buf); break; case 8 : SSD1306_char3216(10, 25, '8', oled_buf); 
  break; case 9 : SSD1306_char3216(10, 25, '9', oled_buf); 
  break; } switch(n2) { case 0 : SSD1306_char3216(26, 25, '0', oled_buf); break; case 1 : SSD1306_char3216(26, 25, '1', oled_buf); break; case 2 : SSD1306_char3216(26, 25, '2', oled_buf); break; 
  case 3 : SSD1306_char3216(26, 25, '3', oled_buf); break; case 4 : SSD1306_char3216(26, 25, '4', oled_buf); 
  break; case 5 : SSD1306_char3216(26, 25, '5', oled_buf); break; 
  case 6 : SSD1306_char3216(26, 25, '6', oled_buf); break; case 7 : SSD1306_char3216(26, 25, '7', oled_buf); 
  break; case 8 : SSD1306_char3216(26, 25, '8', oled_buf); 
  break; case 9 : SSD1306_char3216(26, 25, '9', oled_buf); break; } } } void setup() { Serial.begin(9600); dht.begin(); 
  SSD1306_begin(); 
  SSD1306_clear(oled_buf); 
  SSD1306_string(17, 0, "MYCOLOGEEK", 16 ,1 ,oled_buf); 
  SSD1306_bitmap(42, 16, mmenu, 48, 48, oled_buf); 
  SSD1306_display(oled_buf); 
  
  delay(10000); } 

  void loop(){ 
  
  LIGHT=analogRead(A0); 
  LIGHT=(LIGHT/1024)*100; Serial.print("LIGHT : "); Serial.print(LIGHT); Serial.println(" %"); 
  
  SSD1306_clear(oled_buf); 
  SSD1306_string(30, 0, "LIGHTING", 16 ,1 ,oled_buf); 
  SSD1306_bitmap(80, 16, SUN, 48, 48, oled_buf); 
  SSD1306_bitmap(45, 40, percent, 14, 14, oled_buf);
  dsply(LIGHT);
  SSD1306_display(oled_buf); 
  delay(1000);
  

  
  MOISTURE=analogRead(A1); 
  MOISTURE=(MOISTURE/1024)*100; 
  MOISTURE=(MOISTURE-100)*(-1); Serial.print("MOISTURE : "); Serial.print(MOISTURE); Serial.println(" %"); 
  
  SSD1306_clear(oled_buf); 
  SSD1306_string(22, 0, " MOISTURE", 16 ,1 ,oled_buf); 
  SSD1306_bitmap(80, 16, SAND, 48, 48, oled_buf); 
  SSD1306_bitmap(45, 40, percent, 14, 14, oled_buf); 
  dsply(MOISTURE); 
  SSD1306_display(oled_buf); 
  delay(1000); 
  

  HUMID = dht.readHumidity(); 
  TEMP = dht.readTemperature();
  TEMP = ((int)round(1.8*TEMP+32)); 
  Serial.print("TEMPERATURE : "); 
  Serial.print(TEMP); 
  Serial.println(" *F"); 
  
  SSD1306_clear(oled_buf); 
  SSD1306_string(21, 0, "TEMPERATURE!", 16 ,1 ,oled_buf); 
  SSD1306_bitmap(80, 16, TEM, 48, 48, oled_buf); 
  SSD1306_bitmap(45, 40, FAHRENHEIT, 14, 14, oled_buf); 
  dsply(TEMP); 
  SSD1306_display(oled_buf);
 
  delay(1000); 
  

if (TEMP>80) {
SADD();
}