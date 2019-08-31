/*
  server.ino

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
  - 16 channel CD74HC4067 MUX

  Uses the RF24 library: https://github.com/nRF24/RF24
  Vixen lighting software: http://www.vixenlights.com/
  nRF24L01 tutorial courtesy of RalphBacon:
    - https://github.com/RalphBacon/nRF24L01-transceiver
    - https://www.youtube.com/watch?v=JSHJ-RLbNJk
  Wiring tutorial: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  Shift Register library: https://github.com/Simsso/ShiftRegister74HC595
  
  created 26 August 2019
  by Mitchell Sulkowski
  modified 28 August 2019
  by Mitchell Sulkowski

  https://github.com/electro2560/wireless-light-controller
*/
#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

//How many channels Vixen will control
#define MAX_CHANNELS 28

//Stores the value of each channel
byte incomingByte[MAX_CHANNELS];

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
  radio.openWritingPipe(addresses[0]);
}

void loop() {

  //Keeps track of place in byte array and how many channels are left to be read
  unsigned int remaining;
  unsigned int channel;

  for (;;) {

    //Header character is used to keep the arduino in sync with first channel as Vixen sequentially
    //writes out each byte for every channel.
    while (!Serial.available());
    if (Serial.read() != '>') {
      continue;
    }

    remaining = MAX_CHANNELS;
    channel = 0;

    do {
      while (!Serial.available());
      incomingByte[channel++] = Serial.read();
    }
    while (--remaining);

    //Write out the byte array of channel values to the nRF
    radio.write(&incomingByte, sizeof(byte) * MAX_CHANNELS);

  }

}
