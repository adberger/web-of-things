#ifndef __APP_HPP__
#define __APP_HPP__

#include <AppConfig.h>

#define APP_NAME "DSIOT LoRaWAN TTNv3"
#define APP_AUTHOR "Fabian Küng, Adrian Berger"

#ifdef HELTEC
#define LMIC_NSS  SS
#define LMIC_RXTX cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN
#define LMIC_RST  RST_LoRa
#define LMIC_DIO0 DIO0 
#define LMIC_DIO1 DIO1
#define LMIC_DIO2 DIO2

#define OLED_SDA  SDA_OLED
#define OLED_SCL  SCL_OLED
#define OLED_RST  RST_OLED

#define DHT_PIN   14
#define DHT_TYPE  DHT11

#endif

#ifdef TTGO_LORA32_V21

#define LMIC_NSS  18
#define LMIC_RXTX cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN
#define LMIC_RST  23
#define LMIC_DIO0 26
#define LMIC_DIO1 33
#define LMIC_DIO2 32

#define OLED_SDA  21
#define OLED_SCL  22
#define OLED_RST  16

// TODO define for other boards
#define DHT_PIN   14
#define DHT_TYPE  DHT11

// #define ADC_PIN   35 TODO
#endif

#ifdef FEATHER_M0

#define LMIC_NSS  8
#define LMIC_RXTX cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN
#define LMIC_RST  4
#define LMIC_DIO0 3
#define LMIC_DIO1 6
#define LMIC_DIO2 cMyLoRaWAN::lmic_pinmap::LMIC_UNUSED_PIN

//#define ADC_PIN   35 TODO
#endif

#endif