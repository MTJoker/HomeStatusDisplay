# HomeStatusDisplay

Show status information sent via MQTT (e.g. from FHEM or any other MQTT-speaking system) using RGB LEDs.
![alt tag](http://www.bernd-schubart.de/downloads/fhem/fhemstatusdisplay/homestatusdisplay_37.jpg)

## Requirements
### Hardware
This code was tested on a Wemos D1 mini ESP8266 board. It *should* run on any ESP8266 board. You need a number of WS2812B LEDs (e.g. a NeoPixel stripe) connected to `5V`, `GND` and a `DATA` pin of the ESP. Please notice the wiring tips from Adafruit- use a large capacitor (around 1000uF) between the `5V` and `GND` of the LEDs and put a 300-500 Ohm resistor into the `DATA` line connection to the LEDs. Here is a overview of the wiring - it is the standard wiring recommended for WS2812B stripes. The board can be powered via a the 5V power supply via the USB port. Of course it should be powerful enough to drive all the LEDs used:
![alt tag](http://www.bernd-schubart.de/downloads/fhem/fhemstatusdisplay/homestatusdisplay_board.png)

### Software
The code was developed using the Arduino IDE and the ESP8266 library. You need to install the following additional libraries to compile the code:
 - Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
 - PubSubClient: https://github.com/knolleary/pubsubclient
 - ArduinoJson: https://github.com/bblanchon/ArduinoJson
All libraries can be installed using the library manager of the Arduino IDE.

You also need a running MQTT broker (e.g. https://mosquitto.org), to which the system you want to monitor (e.g. FHEM) pushes its status information. For example, in FHEM the status information which you want to display, can be configured using the FHEM modules `MQTT` and `MQTT_BRIDGE`.

## How to use
Upon first usage, the system will create an access point. Connect to this access point with any WiFi-capable device and open the page 192.168.4.1.
There you will find a configuration page in which you have to enter your settings:
 - WLAN credentials
 - MQTT setup
 - LED setup

You can find more detailed information in the tutorial on my website (it's in German, unfortunately...):
http://www.bernd-schubart.de/homestatusdisplay-smart-home-status-immer-im-blick/
