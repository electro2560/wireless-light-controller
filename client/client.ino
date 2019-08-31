/*
  client.ino

  Listens to Vixen lighting software as a generic serial controller.
  Vixen orchestrates light show by sending serial data over USB to the
  arduino. For convenience, this solution allows the computer running
  Vixen to be located indoors and wirelessly communicates to
  client light controllers throgh nRF2401 modules.

  Hardware used:
  - Ardino Nano V3 development board
  - Arduino Nano V3 terminal adapter board
  - 8 & 4 channel solid state relay modules (high level trigger)
  - AC 120V to DC 5V 700mA power supplies
  - 74HC595 shift registers

  Uses the RF24 library: https://github.com/nRF24/RF24
  Vixen lighting software: http://www.vixenlights.com/
  nRF24L01 tutorial courtesy of RalphBacon:
    - https://github.com/RalphBacon/nRF24L01-transceiver
    - https://www.youtube.com/watch?v=JSHJ-RLbNJk
  Wiring tutorial: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  Shift Register library: https://github.com/Simsso/ShiftRegister74HC595

  created 26 August 2019
  by Mitchell Sulkowski
  modified 30 August 2019
  by Mitchell Sulkowski

  https://github.com/electro2560/wireless-light-controller
*/
#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

//How many channels Vixen will control. This number should be at least the number of channels
//as defined in your server transmitting.
#define MAX_CHANNELS 28

//Stores the value of each channel
byte incomingByte[MAX_CHANNELS];

//How many channels does this client control and what is its starting address
//The starting address is similar to how DMX fixtures have a starting address
#define CHANNELS 16
#define START_ADDRESS 0

//Vixen outputs a byte value, 0 - 255, but we are using lights AC lights that can only be on or off so 
//if the value passes this threshold, the light will turn on.
#define threshold 120

/*
 * Define SHIFT_REGISTER or PINS depending on how you want to control your lights
 */
#define SHIFT_REGISTER
//#define PINS

/*
 * Define ACTIVE_HIGH or ACTIVE_LOW depending on which relays you're using.
 */
//#define ACTIVE_HIGH
#define ACTIVE_LOW

#ifdef SHIFT_REGISTER
  #include <ShiftRegister74HC595.h>
  const int numOfRegisters = CHANNELS / 8;

  const int latchPin = 2; //green
  const int clockPin = 4; //yellow
  const int dataPin = 3; //blue

  ShiftRegister74HC595 sr (numOfRegisters, dataPin, clockPin, latchPin);
#endif

#ifdef PINS
  const int outputs[CHANNELS] = {2, 3, 4, 5};
#endif

//Uses SPI bus + two digital pins for chip enable (CE) and chip select (CSN)
RF24 radio(9, 10);

//An address use by the transmitter and receiver
//Changing this address would allow multiple vixen server instances to control
//differents sets of clients while still operating on the nRF channel.
byte addresses[][6] = {"Vixen1"};

void setup() {
  Serial.begin(115200);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(124);
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();

//All outputs off
#ifdef SHIFT_REGISTER
  #ifdef ACTIVE_HIGH
  sr.setAllLow();
  #endif
  #ifdef ACTIVE_LOW
  sr.setAllHigh();
  #endif
#endif
#ifdef PINS
  for(const int channel : outputs){
    pinMode(channel, OUTPUT);
    digitalWrite(channel, getValue(0));
  }
#endif

}

void loop() {

  if (radio.available()) {

    while (radio.available()) {
      radio.read(&incomingByte, sizeof(byte) * MAX_CHANNELS);
    }

    Serial.println("Received: ");
    for (int i = START_ADDRESS; i < START_ADDRESS + CHANNELS; i++) {
      byte brightness = incomingByte[i];
      int address = i - START_ADDRESS;
      
      Serial.print(address);
      Serial.print(": ");
      Serial.println(brightness);

      bool output = getValue(brightness);
      
      #ifdef SHIFT_REGISTER
        sr.set(address, output);
      #endif
      #ifdef PINS
        digitalWrite(outputs[address], output);
      #endif

    }

  }

}

bool getValue(byte brightness){

  bool output = false;
  
  if(brightness > threshold) output = true;
  
  #ifdef ACTIVE_HIGH
    if(output) return true;
    return false;
  #endif
  #ifdef ACTIVE_LOW
    if(output) return false;
    return true;
  #endif
}
