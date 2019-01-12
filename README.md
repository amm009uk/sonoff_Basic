# Sonoff Basic

### This project assumes you know what a "sonoff Basic" device is and how to upload code to it.

-------------------------------------------------------------------------------------------------------------
## Features

- Custom firmware to take greater control and integrate with an MQTT Broker. The Broker would typically integrate with your Home Automation system.

- Device will respond to an MQTT message, onboard toggle and a switch on GPIO-14 if connected.

- WiFi or MQTT drop outs are handled automatically.

- You can set a periodic reboot option to keep things fresh.

-------------------------------------------------------------------------------------------------------------
## Version
1.4 Initial testing completed.

-------------------------------------------------------------------------------------------------------------
## Setup device
1. Flash SPIFFs to upload sonoff/data/*.json. You may modify contents but not necessary.

2. To enable/disable serial debug output, uncomment/comment first line in sonoff/src/User.h.

3. Flash firmware.

All future updates can now be performed over the air no need for USB assuming above was successful.

-------------------------------------------------------------------------------------------------------------
## Usage
1. Device will initially come up with its own *Access Point* called ITEAD-xxxxxxx. Connect to this and configure WiFi parameters. Once saved, device will reboot.

2. On bootup, device will connect to your WiFi. Find its IP address through your router and connect to it. Configure all parameters and once saved, device will reboot.

3. Device can be controlled with MQTT messages, onboard button or adding a switch between GPIO-14 and GND.

4. Onboard LED light will inidicate when power is on.

- An alternative method for finding your device is to scan your mDNS network

-------------------------------------------------------------------------------------------------------------
## Sample openHAB "item" for Broker/MQTT messages. 
	- Switch Power "Power" {mqtt=">[brk:cmnd/Power:command:*:default], <[brk:stat/Power:state:default]",autoupdate="false"}
The inbound "<" message helps to keep openHAB in sync with device status.

-------------------------------------------------------------------------------------------------------------
## OTA Updates
Once device is connected to your WiFi, find its IP and connect to it. User/Password are stored in sonoff/src/User.h so you can always modify and flash new firmware easily.

-------------------------------------------------------------------------------------------------------------
- I am simply reusing other peoples amazing work for instance the following libraries PubSubClient and WifiManager.

- My development environment is Atom with its builtin PlatformIO toolset. Its a fantastic build and debug environment.