# ESP8266 External Temp Sensor

This is simple board which will connect to one of those DS18B20 temperature sensors which are inside a metal sleeve and designed for external use. 

This board will ultimately live in a box with a 12V car battery, charged using solar power. One of the new features for me is using a Switching regulator rather than a nomral LDO Linear regulator. I read that trying to drop from 12v to 3.3v is not effecient at all and can generate a lot of heat using a Linear design.

The board's software (not yet written) will take a measurememt, send it over MQTT and go back to sleep. I wanted this so I could compare the temperature between inside my polytunnel (see https://github.com/theopensourcerer/esp8266_envmonitor) and the normal outdoor temperature.

As with the Environment Monitor Project I plan to publish the code and a 3D case for this design. Watch this space!
