/*
  client.ino

  Listens to Vixen lighting software as a generic serial controller.
  Vixen orchestrates light show by sending serial data over USB to the
  arduino. For convenience, this solution allows the computer running
  Vixen to be located indoors and wirelessly communicates to
  client light controllers throgh nRF24L01 modules.

  Hardware used:
  - Ardino Nano V3 development board
  - Arduino Nano V3 terminal adapter board
  - 8 & 4 channel solid state relay modules (high level trigger)
  - AC 120V to DC 5V 700mA power supplies
  - 74HC595 shift registers
  - nRF24L01 modules

  Uses the RF24 library: https://github.com/nRF24/RF24
  Vixen lighting software: http://www.vixenlights.com/
  nRF24L01 tutorial courtesy of RalphBacon:
    - https://github.com/RalphBacon/nRF24L01-transceiver
    - https://www.youtube.com/watch?v=JSHJ-RLbNJk
  Wiring tutorial: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  Shift Register library: https://github.com/Simsso/ShiftRegister74HC595

  created 26 August 2019
  by Mitchell Sulkowski
  modified 3 September 2019
  by Mitchell Sulkowski

  https://github.com/electro2560/wireless-light-controller
*/
#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>
#include "pins_arduino.h"

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
//if the value reaches this threshold, the light will turn on.
#define threshold 128

/*
   Define SHIFT_REGISTER or PINS depending on how you want to control your lights
*/
#define SHIFT_REGISTER
//#define PINS

/*
   PWM will only work with DC loads. If you are controlling AC loads, you must define AC_DIMMING instead.
   PWM will only work for PWM pins. Any non-PWM pins will act as digital according to the threshold set above.
*/
#define USE_PWM false

/*
   AC dimming will only work for AC loads. You must be using solid state relays and must provide an Arduino
   interrupt pin with zero cross detection from the AC cycle. Some relay boards such as the RobotDyn AC Light
   Dimmer provide this zero cross detection. AC dimming requires the use of direct pins from the Arduino.
*/
#define AC_DIMMING false


int AC_LOAD = 3;    // Output to Opto Triac pin
int dimming = 128;  // Dimming level (0-128)  0 = ON, 128 = OFF

/*
   Define ACTIVE_HIGH or ACTIVE_LOW depending on which relays you're using.
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

/*
   An address used by the transmitter and receiver.
   Changing this address would allow multiple vixen server instances to control
   differents sets of clients while still operating on the nRF channel.
*/
byte addresses[][6] = {"Vixen1"};

/*
   Debug mode should be false if you are not using it for testing purposes because
   it will slow down the response of the lights and sometimes they may fail to active.
*/
const bool debug = false;

void setup() {
  if (debug) Serial.begin(115200);

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
    for (const int channel : outputs) {
      pinMode(channel, OUTPUT);
      digitalWrite(channel, getValue(0));
    }
  #endif

  if (AC_DIMMING) {
    pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
    attachInterrupt(0, zero_crosss_int, RISING);  // Choose the zero cross interrupt # from the table above
  }
}

void loop() {

  if (radio.available()) {

    while (radio.available()) {
      radio.read(&incomingByte, sizeof(byte) * MAX_CHANNELS);
    }

    if (debug) Serial.println("Received: ");
    for (int i = START_ADDRESS; i < START_ADDRESS + CHANNELS; i++) {
      byte brightness = incomingByte[i];
      int address = i - START_ADDRESS;

      if (debug) {
        Serial.print(address);
        Serial.print(": ");
        Serial.println(brightness);
      }

      bool output = getValue(brightness);

  #ifdef SHIFT_REGISTER
    sr.set(address, output);
  #endif
  #ifdef PINS
    uint8_t pin = outputs[address];

    if (USE_PWM && supportsPWM(pin)) {
      #ifdef ACTIVE_HIGH
        analogWrite(pin, brightness);
      #endif
    #ifdef ACTIVE_LOW
      analogWrite(pin, 255 - brightness);
    #endif
      } else {
        digitalWrite(pin, output);
      }
  #endif

    }

  }

}

bool getValue(byte brightness) {

  bool output = false;

  if (brightness > threshold) output = true;

#ifdef ACTIVE_HIGH
  if (output) return true;
  return false;
#endif
#ifdef ACTIVE_LOW
  if (output) return false;
  return true;
#endif
}

/*
   The pins_arduino.h has a list of which pins support PWM for the processor selected during compilation.
   PWM pins are defined by which timer they use internally on the processor. Digital pins are defined
   as NOT_ON_TIMER.
*/
bool supportsPWM(uint8_t pin) {
  return digitalPinToTimer(pin) != NOT_ON_TIMER;
}


//TODO: Make this work for multiple outputs
//Use manufacturer's library https://github.com/RobotDynOfficial/RBDDimmer
//zero_cross_int provided by https://github.com/tehniq3/ac-dimmer_with_triac/blob/54524e33f2e760ea9798577ab7894d5a33f41548/ac_dimmer_with_triac_fade_simple.ino#L42
// the interrupt function must take no parameters and return nothing
void zero_crosss_int()  // function to be fired at the zero crossing to dim the light
{
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms
  // Every zerocrossing thus: (50Hz)-> 10ms (1/2 Cycle) For 60Hz => 8.33ms (10.000/120)
  // 10ms=10000us
  // (10000us - 10us) / 128 = 75 (Approx) For 60Hz =>65

  int dimtime = (75 * dimming);  // For 60Hz =>65
  delayMicroseconds(dimtime);    // Off cycle
  digitalWrite(AC_LOAD, HIGH);   // triac firing
  delayMicroseconds(10);         // triac On propogation delay (for 60Hz use 8.33)
  digitalWrite(AC_LOAD, LOW);    // triac Off
}
