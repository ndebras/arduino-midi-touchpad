/***************************************************
  This is a touchpad sketch allowing a Arduino UNO 
  and a Adafruit 2.8'' TFT Touchscreen to send MIDI 
  control change message over USB.
  
  It is based on this article : 
  https://www.instructables.com/id/Turn-your-Arduino-Uno-into-an-USB-HID-Mididevice/

  It uses the following libraries : 
  - https://github.com/ddiakopoulos/hiduino/blob/master/src/arduino_midi/arduino_midi.h
  - https://github.com/adafruit/Adafruit-GFX-Library
  -  https://github.com/FortySevenEffects/arduino_midi_library

 Arduino UNO R3 is available here : 
 https://shop.mchobby.be/fr/carte-arduino-officielle/10-arduino-uno-r3e-atmega328-assemble-3232100000100-arduino.html?search_query=arduino+uno&results=485
 
 TFT shield available here :
 https://shop.mchobby.be/fr/shields/384-ecran-tactile-28-tft-pour-arduino-3232100003842-adafruit.html?search_query=arduino+TFT+shield&results=560

 Author : 
 Nicolas Debras <nicolas@debras.fr>
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <MIDI.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// MIDI setup
#define MIDI_CHANNEL 10
#define CONTROL_A 110
#define CONTROL_B 111

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#include <SoftwareSerial.h>
SoftwareSerial BTSerial(A8,A9); // RX, TX

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

void setup(void) {
  //Serial.println("Setting up MIDI..");
  MIDI.begin(MIDI_CHANNEL_OMNI); //receiving MIDI message on all channels

  SPI.begin();

  //Serial.println("Setting up TFT..");
  tft.begin();
  tft.setTextSize(1);
  tft.fillScreen(0x0000);

  //Serial.println("Setting up Touchscreen..");
  if (!ts.begin()) {
    //Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  //Serial.println("...done");
  drawScreen();

  //bluetooth communication init
  Serial.begin(57600);
  Serial.println("BT setup...");
  BTSerial.begin(57600);
  while(!BTSerial){
    Serial.println("Waiting for BT connection...");
  }
  Serial.println("BT Setup Done.");
  Serial.println();
  
  BTSerial.listen();  
}


void loop()
{
  delay(100);
   //BlueTooth setup
  if (BTSerial.available()) {
    int inByte=BTSerial.read();
    if(inByte!=-1){
      //Serial.println(inByte);
      tftDisplayMsg(inByte);
    }
    
  }

  // See if there's any  touch data for us
  if (ts.bufferEmpty()) {
    return;
  }

  // Skip the loop if the screen is not touched
  if (! ts.touched()) {
    return;
  }

  // Retrieve a point
  TS_Point p = ts.getPoint();

  // Scale from ~0->4000 to tft.width using the calibration #'s
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

  //sending data over USB MIDI
  //As MIDI CC message accepts a value from 0 to 127 we have to do some calculation here
  float screen_width= tft.width();
  float x_value = p.x;
  float x_midi_value= (127/screen_width) * x_value;

  float screen_height= tft.height();
  float y_value = p.y;
  float y_midi_value= (127/screen_height) * y_value;
  
  //MIDI.sendControlChange(CONTROL_A, x_midi_value, MIDI_CHANNEL);
  //MIDI.sendControlChange(CONTROL_B, y_midi_value, MIDI_CHANNEL);
  
  readAndClearScreen(x_midi_value, y_midi_value);
  

  //MIDI.read();



}

void tftDisplayMsg(int string){ 
  tft.fillRect(200, 0, 40, 9, 0x0000);
  tft.setCursor(200,0);
  tft.print("BT:");
  tft.setCursor(220,0);
  tft.print(string);
}

void debugTFT(int string){
  
  tft.fillRect(0, 0, 20, 9, 0x0000);
  tft.setCursor(0,20);
  tft.print(string);
}

void readAndClearScreen(int x, int y) {

  //displaying touch coordinates
  tft.setCursor(0, 0);
  // Clearing top of the screen
  tft.fillRect(0, 0, 199, 9, 0x0000);
  tft.print("X: ");
  tft.setCursor(18, 0);
  tft.print(x);
  tft.setCursor(50, 0);
  tft.print("Y: ");
  tft.setCursor(68, 0);
  tft.print(y);

  /*
  Serial.print("("); Serial.print(x);
  Serial.print(", "); Serial.print(y);
  Serial.println(")");
  */
}

void drawScreen() {
  //drawing screen
  tft.drawRect(0, 9, 240, 310, 0xAAAA);
  
  //vertical line
  for (int i=0; i<=240 ; i=i+40){
    tft.drawLine(i, 9, i, 320, 0x8888);
  }

  //horizontal line
  for (int i=0; i<=320 ; i=i+40){
    tft.drawLine(0, i, 240, i, 0x8888);
  }
  
  //middle lines
  tft.drawLine(120, 9, 120, 320, 0x4444);
  tft.drawLine(0, 160, 240, 160, 0x4444);


}
