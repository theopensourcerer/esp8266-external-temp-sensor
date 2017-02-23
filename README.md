# ESP8266 External Temp Sensor

This is simple board which will connect to one of those DS18B20 temperature sensors which are inside a metal sleeve and designed for external use. 

This board will ultimately live in a box with a 12V car battery, charged using solar power. One of the new features for me is using a Switching reglator rather than a nomral LDO Linear regulator. Dropping from 12v to 3.3v is not effecient and generates a lot of heat in the Linear design.


The board's software will take a measurememt, send it over MQTT and go back to sleep. I wanted this so I could compare the temperature between inside my polytunnel (see https://github.com/theopensourcerer/esp8266_envmonitor) and the normal outdoor temperature.
