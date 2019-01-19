# Sonoff Basic

### This project assumes you know what a "sonoff Basic" device is and how to upload code to it.

-------------------------------------------------------------------------------------------------------------
## Features

- Custom firmware to take greater control and integrate with an MQTT Broker. The Broker would typically integrate with your Home Automation system

- Device will respond to an MQTT message, onboard toggle and a switch on GPIO-14 if connected

- WiFi or MQTT drop outs are handled automatically

- You can set a periodic reboot option to keep things fresh



-------------------------------------------------------------------------------------------------------------
## Version
1.0 Initial version  
1.1 Testing completed  
1.2 Updated README  
1.3 Added IP lookup  
1.4 Added over the air serial output with Telnet (RemoteDebug library)  

-------------------------------------------------------------------------------------------------------------
## Device Setup
1. Flash SPIFFs to upload the configuration files - sonoff_Basic/data/config.json. You may modify the contents prior to upload but not necessary

2. Flash firmware

3. Device will initially come up with its own *Access Point* called esp82XX-xxxxxxx. Connect to this and configure WiFi parameters. Once saved, device will reboot and connect to your WiFi  
   See section **Finding device IP Address**

4. Once device is connected to WiFi, connect to it using a Browser. User/Password are stored in MultiSensor/src/User.h

5. Configure device parameters on web page and save settings. Once saved, device will reboot and reconnect to your WiFi and MQTT Broker

- Above steps should be done over USB-->Serial interface until device is fully functioning  
- Future firmware updates can be performed over the air with no need for USB-->Serial interface

7. Test device and once ok, turn off debugging and upload new compiled firmware  
   See section **Debug - Serial/Telnet output**

-------------------------------------------------------------------------------------------------------------
## Finding device IP Address
	To get the device IP address you have the following options:
	1. Look at the Serial output where it will show on startup (assuming you have debug output turned on)
	2. Look in your router to see WiFi clients
	3. Try an mDNS browser app but this often takes time to get the device showing up

	4. If already connected to WiFi and MQTT Broker, you can send a blank MQTT message as defined in user.h at "IP_REQUEST"  
     Each device will respond with a MQTT message such as defined with "IP/REPLY/<deviceID>" with the IP address in the payload.

-------------------------------------------------------------------------------------------------------------
## Debug - Serial/Telnet output
	You have two options after turning on SERIAL_DEBUG within sonoff_Basic\src\User.h:
		- Serial output over USB if connected
		- Telnet if connected

**Do not leave SERIAL_DEBUG enabled for normal use**

-------------------------------------------------------------------------------------------------------------
## OTA Firmware Updates
Once device is connected to your WiFi, find its IP and connect to it through using a Browser  
User/Password are stored in sonoff_Basic/src/User.h and you can always modify and flash new firmware to change it  
Follow on screen firmware update instructions to flash new firmware

-------------------------------------------------------------------------------------------------------------
## Credits
I am simply reusing other peoples amazing work for instance the following libraries:  
	- [PubSubClient](https://github.com/knolleary/pubsubclient)  
	- [WifiManager](https://github.com/tzapu/WiFiManager)  
	- [RemoteDebug](https://github.com/JoaoLopesF/RemoteDebug)  
	- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)  
	- [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor)  
	- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)  

AND OF COURSE the many examples on github

My development environment is Atom with its builtin PlatformIO toolset. Its a fantastic build and debug environment.
