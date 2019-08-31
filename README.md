# Wireless light controller
Arduino powered wireless light controller

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
