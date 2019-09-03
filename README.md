# Wireless light controller
Arduino powered wireless light controller

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

  * RF24 library: https://github.com/nRF24/RF24
  * Vixen lighting software: http://www.vixenlights.com/
  * nRF24L01 tutorial courtesy of RalphBacon:
    - https://github.com/RalphBacon/nRF24L01-transceiver
    - https://www.youtube.com/watch?v=JSHJ-RLbNJk
  * nRF Wiring tutorial: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  * Shift Register library: https://github.com/Simsso/ShiftRegister74HC595

## Getting started
The following set of instructions will guide you through setup and configuration of your wireless light controllers.

### Prerequisites
```
Download and install [Arduino IDE](https://www.arduino.cc/en/main/software)
Download and install [Vixen Lights](http://www.vixenlights.com/)
Understanding of how to safely work with high voltage electricity such as mains voltage
```

### Hardware
#### Building the transmitter
The transmitter acts as a server between Vixen and the client light controllers. One transmitter can provide multiple clients with the information they need to control their lights. This will be explaining in a following section on how to program the hardware.

The transmitter is as simple as an Arduino and nrF24L01 module. This will connect to your computer that is running Vixen Lights which will direct the Arduino which brightness each light channel should be. This will be discussed later when programming the transmitter.

![Transmitter example schematic](transmitter.jpg?raw=true "Transmitter example schematic")

The nRF24L01 uses the SPI bus of the Arduino. In this example, an Arduino Nano v3.0 was used. If using a different Arduino, check the datasheet for which SPI pins to use. Pins 3 (CE) and 4 (CSN) of the nRF2401 connect to digital pins 9 and 10 in this example. Those pins can be any output pin and are used by the Arduino for chip enable and chip select functions.

> Notice the nRF24L01 operates on 1.9-3.6 volts. You must use the 3.3v power rail from the Arduino.

### Building the receiver
The receiver is a client to the transmitter which listens for instruction of its light channels. There are multiple ways of controlling your lights depending on which you are using. The following schematics vary in how many lights they control, however they all share the use of an nrf24L01 to receive data from the transmitter.

![](transmitter.jpg?raw=true "Transmitter example schematic")

![](transmitter.jpg?raw=true "Transmitter example schematic")

![](transmitter.jpg?raw=true "Transmitter example schematic")

