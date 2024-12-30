
/***************
  includes
****************/

#include <Adafruit_NeoPixel.h>
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

/**********************
  // Globals for bluetooth
    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"

************************/

#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "DISABLE"
#define BLUEFRUIT_UART_MODE_PIN      -1  //needed for Flora setup




#define toneC    1911
#define toneC1    1804
#define toneD    1703
#define toneEb    1607
#define toneE    1517
#define toneF    1432
#define toneF1    1352
#define toneG    1276
#define toneAb    1204
#define toneA    1136
#define toneBb    1073
#define toneB    1012
#define tonec       955
#define tonec1      902
#define toned       851
#define toneeb      803
#define tonee       758
#define tonef       716
#define tonef1      676
#define toneg       638
#define toneab      602
#define tonea       568
#define tonebb      536
#define toneb       506

#define tonep       0

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
#define PIN 6 // Pin that NeoPixel strip is connected to
#define SPEAKER 3 // Pin that speaker is connected to
#define NUM_PIXELS 11

// Global inits
// Init the neopixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN);


long vel = 20000;
boolean hasplayed = false;
int menorah[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // track whether the candles are on or off
int hanukahDay = 8;
int larsonFlag = 0; // for cylon scanner effect
int pos = 0, dir = 1; // Position, direction of "eye"

int incomingByte = 0; // for incoming serial data




/************
   more bluetooth setup code
 ************/
// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];




void setup() {
  while (!Serial);  // required for Flora & Micro
  delay(500);

  pinMode(SPEAKER, OUTPUT);
  delay(2000);
  strip.begin();
  strip.setPixelColor(4, 255, 0, 0);
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
  Serial.println("Initializing menorah controller");

  // hanukahDay = 2;

    if ( !ble.begin(VERBOSE_MODE) )
    {
      error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
    }
    Serial.println( F("OK!") );
  
  
  
  //  /* Wait for connection */
    while (! ble.isConnected()) {
      Serial.println( F("Trying to connect") );
      delay(500);
    }
  
    // LED Activity command is only supported from 0.6.6
    if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
    {
      // Change Mode LED Activity
      Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
      ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
    }
  
    /* Disable command echo from Bluefruit */
    ble.echo(false);
  
    Serial.println("Requesting Bluefruit info:");
    /* Print Bluefruit information */
    ble.info();
  
    // Set Bluefruit to DATA mode
    Serial.println( F("Switching to DATA mode!") );
    ble.setMode(BLUEFRUIT_MODE_DATA);
}




int melod[] = {tonec, tonec, toned, toned, tonee, tonec, tonee, toneg, toneg, tonef, tonee, toned, toneG, toned, toned, tonee, tonee, tonef, toned, toned, toneg, tonef, tonee, toned, tonec, toneg, toneg, tonee, toneg, tonee, toneg, tonee, tonee, toneg, toneg, tonef, tonee, toned, toned, tonef, toned, tonef, toned, tonef, toned, toneg, tonef, tonee, toned, toneC};
int ritmo[] = {16, 16, 16, 16, 16, 32, 16, 16, 16, 16, 16, 48, 16, 16, 16, 16, 16, 16, 32, 16, 16, 16, 16, 16, 32, 32, 16, 16, 16, 16, 16, 32, 16, 16, 16, 16, 16, 48, 16, 16, 16, 16, 16, 16, 32, 16, 16, 16, 16, 16, 48};

void playsong() {
  for (int i = 0; i < 50; i++) {
    int tom = melod[i];
    int tempo = ritmo[i];

    long tvalue = tempo * vel;

    tocar(tom, tvalue);

    delayMicroseconds(1000);
  }     delay(1000);

  hasplayed = true;
}

void tocar(int tom, long tempo_value) {
  long tempo_gasto = 0;
  while (tempo_gasto < tempo_value) {
    digitalWrite(SPEAKER, HIGH);
    delayMicroseconds(tom / 2);

    digitalWrite(SPEAKER, LOW);
    delayMicroseconds(tom / 2);
    tempo_gasto += tom;
  }
}

void loop() {
  int delayTime = 50;


  if (larsonFlag == 0) {

    // flicker the lit pixels
    for (int i = 0; i < hanukahDay; i++) {
      if (i != 4) {
        int c = random(64, 192);
        strip.setPixelColor(i, 255, c, 0);
      }
    }
    strip.show();
  } else {
       // Light up LEDs with the gradient
      for (int i = 0; i < NUM_PIXELS; i++) {
        if (i == pos) {
          strip.setPixelColor(i, strip.Color(255, 0, 0)); // Main LED (bright red)
        } else if (i == pos - 1 || i == pos + 1) {
          strip.setPixelColor(i, strip.Color(75, 0, 0)); // Adjacent LEDs (darker red)
        } else {
          strip.setPixelColor(i, strip.Color(0, 0, 0));   // Turn off other LEDs
        }
      }
      strip.show();
      delay(delayTime);
    
      // Move to the next position
      pos += dir;
    
      // Reverse direction at ends
      if (pos == NUM_PIXELS - 1 || pos == 0) {
        dir = -dir;
      }
  
  } // end else larson check


    // check bluetooth keypress port for what night it is
    uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
    if (len == 0) return;
  
    /* Got a packet! */
    printHex(packetbuffer, len);

   // Buttons
    if (packetbuffer[1] == 'B') {
      uint8_t buttnum = packetbuffer[2] - '0';
      boolean pressed = packetbuffer[3] - '0';
      Serial.print ("Button "); Serial.print(buttnum);
      if (pressed) {
        Serial.println(" pressed");
      } else {
        Serial.println(" released");

//  if (Serial.available() > 0) {
//    // read the incoming byte:
//    incomingByte = Serial.read();

    // say what you got:
//    Serial.print("I received: ");
//    Serial.println(incomingByte - '0');
//    int buttnum = incomingByte - '0';


    switch (buttnum) {
      case 1:
        Serial.println("Case 1");
        playsong();
        break;
      case 3:  // turn on cylon scanner
        larsonFlag = 1;
        alloff();
        break;
      case 4: // turn off cylon scanner
        larsonFlag = 0;
        break;
      case 7:
        hanukahDay--;
        break;
      case 8:
        hanukahDay++;
        break;

    } // end switch

    if (hanukahDay > NUM_PIXELS) {
      hanukahDay = NUM_PIXELS ;
    }
    if (hanukahDay == 5) {
      hanukahDay++;
    }
    if (hanukahDay < 0) {
      hanukahDay = 0;
    }
    // reset the menorah and light the shamash
    alloff();
//    strip.show();
//    delay(250);
    strip.setPixelColor(5, 255, 0, 0);
    strip.show();
    delay(250);

    // finish the loop - if pixel is 0-3 or 7-10 it's a normal candle.  If it's 5, it's the shamash. if it's 4 or 6 it's off.  start at zero

    for (int i = 0; i < hanukahDay; i++) {
      switch (i) {
        case 4:
        case 6:
          menorah[i] = 0;
          break;
        case 5:
          menorah[i] = 1;
          break;
        default: 
          menorah[i] = 1;
          strip.setPixelColor(i, 255, 128, 0);
          delay(250);
      }
//      if (i != 5) { // shamash is at 5, starting at 0
//        menorah[i] = 0;
//        strip.setPixelColor(i, 255, 128, 0);
//        delay(250);
//      } else if (i 
      strip.show();
//      Serial.println ("lighting LED");
//      Serial.println(i);
    }
  } // end else button released



} // end if packetbuffer == b



//  Serial.println("hanukah day");
//  Serial.println(hanukahDay);
//  Serial.println("Larson flag");
//  Serial.println(larsonFlag);

}





void alloff() {
  Serial.println("all off");
  for (int i = 0; i < NUM_PIXELS; i++) {
    menorah[i] = 0;
    strip.setPixelColor(i, 0, 0, 0);
//        Serial.println ("off LED");
//        Serial.println(i);
  }
}
