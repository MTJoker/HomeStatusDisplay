# FhemStatusDisplay

Show status information from FHEM server sent via MQTT using RGB LEDs.

## Requirements
### Hardware
This code was tested on a wemos D1 mini ESP8266 board. It <i>should</i> run on any ESP8266 board. You need a number of WS2812B LEDs (e.g. a NeoPixel stripe) connected to 5V and GND and a data pin of the ESP. Please notice the wiring tips from Adafruit- use a large capacitor (around 1000uF) between the 5V and GND of the LEDs and put a 300-500 ohm resistor into the data line connection to the LEDs.<br>
An optional heartbeat LED can be connected to another data pin of the ESP and over a 1k resistor to GND. This led will flash every 3s as long as the ESP executes the main loop.

### Software
The code was developed using the Arduino IDE and the ESP8266 plugin. You need to install the following libraries to compile the code:<br>
Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel<br>
PubSubClient: https://github.com/knolleary/pubsubclient

You also need a running FHEM server and an installed mosquitto installation for MQTT usage. The status information which you want to display, has to be configured using the FHEM modules MQTT and MQTT_BRIDGE.

## How to use
All user-specific configuration has to be entered in the file <i>FhemStatusDisplayConfig.h</i>.
Most important are your WLAN credentials and MQTT server name and topics. Please have a look into the file for more information.
