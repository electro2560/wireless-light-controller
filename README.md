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

  Libraries and tutorials:
  * RF24 library: https://github.com/nRF24/RF24
  * Vixen lighting software: http://www.vixenlights.com/
  * nRF24L01 tutorial courtesy of RalphBacon:
    - https://github.com/RalphBacon/nRF24L01-transceiver
    - https://www.youtube.com/watch?v=JSHJ-RLbNJk
  * nRF Wiring tutorial: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  * Shift Register library: https://github.com/Simsso/ShiftRegister74HC595

# Getting started
The following set of instructions will guide you through setup and configuration of your wireless light controllers.

## Prerequisites
* Download and install [Arduino IDE](https://www.arduino.cc/en/main/software)
* Download and install [Vixen Lights](http://www.vixenlights.com/)
* Understanding of how to safely work with high voltage electricity such as mains voltage

## Hardware
### Building the transmitter
The transmitter acts as a server between Vixen and the client light controllers. One transmitter can provide multiple clients with the information they need to control their lights. This will be explaining in a following section on how to program the hardware.

The transmitter is as simple as an Arduino and nrF24L01 module. This will connect to your computer that is running Vixen Lights which will direct the Arduino which brightness each light channel should be. This will be discussed later when programming the transmitter.

![Transmitter example schematic](img/transmitter.jpg?raw=true "Transmitter example schematic")

The nRF24L01 uses the SPI bus of the Arduino. In this example, an Arduino Nano v3.0 was used. If using a different Arduino, check the datasheet for which SPI pins to use. Pins 3 (CE) and 4 (CSN) of the nRF24L01 connect to digital pins 9 and 10 in this example. Those pins can be any output pin and are used by the Arduino for chip enable and chip select functions.

> Notice the nRF24L01 operates on 1.9-3.6 volts. You must use the 3.3v power rail from the Arduino.

### Building the receiver
The receiver is a client to the transmitter which listens for instruction of its light channels. There are multiple ways of controlling your lights depending on which you are using. The following schematics vary in how many lights they control, however they all share the use of an nrf24L01 to receive data from the transmitter.

> 16 channel controller using 2 shift registers.
![16 Channel Receiver](img/16_channel_receiver.jpg?raw=true "16 Channel Receiver")

> 8 channel controller using 1 shift register.
![8 Channel Receiver](img/8_channel_receiver.jpg?raw=true "8 Channel Receiver")

> 4 channel controller using pins directly from the Arduino.
![4 Channel Receiver](img/4_channel_receiver.jpg?raw=true "4 Channel Receiver")

#### Dimming Lights
It is possible to dim lights through the use of PWM pins on the Arduino. To do this, you must enable PWM on the receiving Arduino's code and use pins which support PWM. This will be explained in more detail in configurating your receivers.
For lights which operate on DC, you must use solid state relays which support DC. For lights which operate on AC, you must use solid state relays designed for AC loads and must provide a zero-cross output. This is a special output which will be used as an interrupt on the Arduino to sync the PWM signal with the AC cycle. 

![AC PWM Example](img/ac_pwm.jpg?raw=true "AC PWM Example")

![DC PWM Example](img/dc_pwm.jpg?raw=true "DC PWM Example")

## Setting up Vixen
### Step 1
> Download and install [Vixen lights](http://www.vixenlights.com/)
### Step 2
> Once you have launched Vixen, you will be greeted with this start menu. From here we will need to setup a display.\
![Vixen start menu](img/vixen_start_menu.jpg?raw=true "Vixen start menu")
### Step 3
> To setup our display, select Generic Serial from the drop down menu and click the plus mark to add it.\
![Add generic serial](img/vixen_add_controller.jpg?raw=true "Add generic serial")

> You may assign it any controller name.\
![Assign controller name](img/vixen_controller_name.jpg?raw=true "Assign controller name")

> The number outputs should be the amount of lights you plan on your clients controlling.\
![Outputs](img/vixen_outputs.jpg?raw=true "Outputs")
### Step 4
> With your transmitting Arduino connected to your computer over USB, we now need to select the serial port Vixen will send the light values to. Select your controller and open the options view through the gear icon.\
![Serial port settings](img/vixen_serial_port_settings.jpg?raw=true "Serial port settings")

> Select the serial port of your Arduino and set the baud rate to 115200. Click OK to get back to the controller options view.\
![Vixen buad rate](img/vixen_baud_rate.jpg?raw=true "Baud rate")

> Again at the controller options view, check to send a text header and set it to `>`. That is the header the transmitter will use to keep in sync with the channels Vixen sends over serial.\
![Text header](img/vixen_text_header.jpg?raw=true "Text header")

> Back at the display setup, add a generic numbered group. The group name may be anything you'd like. The item prefix will be used to identify each channel. Ex: If Outlet was the prefix, you would have a channel called Outlet-1, Outlet-2, etc.\
![Add generic numbered group](img/vixen_add_generic_numbered_group.jpg?raw=true "Add generic numbered group")\
![Group outputs](img/vixen_generic_numbered_group_outputs.jpg?raw=true "Group outputs")

> With your controller and group of elements selected, you can now click the Patch Elements button at the bottom of the screen to assign those elements to the controller.\
![Patch elements](img/vixen_patch_elements.jpg?raw=true "Patch elements")\
![Patch complete](img/vixen_patch_complete.jpg?raw=true "Patch complete")
### Step 5
> Now that your display is setup, you can create a new sequence and start programming your light show.

## Programming the transmitter
The transmitter is easy to setup and get running without much configuration. With your Arduino IDE, open the sample server.ino file. From here, the only option that you will need to configure is `MAX_CHANNELS`, the amount of outputs you set to control in Vixen. If you are using different pins than 9 and 10 for the CE and CSN pins, you will also to change those to the correct pins in the code.

![Transmitter configuration](img/arduino_transmitter_configuration.jpg?raw=true "Transmitter configuration")

Optionally you may change the address the nRF24L01 module which is explained in the file.

## Programming the receivers
The receivers have far more options than the server requires. Open the client.ino to configure and upload the code to your receivers.

`MAX_CHANNELS` should be set to match your\
`CHANNELS` is how many lights this receiver controls\
`START_ADDRESS` is the first channel of this receiver. This allows one receiver to work with channels 0-15 and a separate receiver to control channels 16-23 for example. The first channel of Vixen uses the start address 0.\
`threshold` is the point where the output should enable. Vixen can fade lights so it outputs a brightness of 0-255 for each light. If you're not using PWM capable relays, this threshold is used to determine at which point the light should be on or off.

There are 2 ways to control your outputs: `SHIFT_REGISTER` or `PINS`\
Define which one you will be using on this receiver. If you wish to use PWM, you must use `PINS` and set `USE_PWM` to true. With AC loads, you can only use PWM with supported relay boards that provide the Arduino with a zero-cross output.

Define either `ACTIVE_HIGH` or `ACTIVE_LOW` depending on which relays you're using.

If you are using shift registers, you will then need to define your latch, clock, and data pins. The controller will expect you to have a shift register for every set of up to 8 channels it is set to control. For example, to control 16 channels you would need 2 shift registers, whereas to control 17 outputs you would need 3.

If you are using direct pins from the Arduino, put them in order into the `outputs` list. The first pin in the list represents the first channel controlled by this receiver. For PWM, check your Arduino datasheet and use pins which support PWM as typically indicated by a `~` mark.

If you are using different pins than 9 and 10 for the CE and CSN pins, you will also to change those to the correct pins in the code.

Your address of all clients must match the address used by the transmitter.

![Receiver configuration](img/arduino_receiver_configuration.jpg?raw=true "Receiver configuration")

## Creating a Vixen light sequence


## Demo videos