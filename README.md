# Sonoff Basic

### This project assumes you know what a "sonoff Basic" device is and how to upload code to it.

-------------------------------------------------------------------------------------------------------------
## Features

- Custom firmware to take greater control and integrate with an MQTT Broker. The Broker would typically integrate with your Home Automation system. In my case I use openHAB  

- Device will respond to:  
    1. MQTT message  
    2. Onboard toggle  
    3. GPIO-14 switch if connected  

- WiFi or MQTT drop outs are handled automatically

-------------------------------------------------------------------------------------------------------------
## Version
1.0 Initial version  
1.1 Testing completed  
1.2 Updated README  
1.3 Added IP lookup  
1.4 Added over the air serial output with Telnet (RemoteDebug library)  
1.5 Moved web page to PROGMEM  
1.6 Upgraded to Arduino 6.x  
1.7 Minor stability changes  
1.8 Removed RemoteDebug library  

-------------------------------------------------------------------------------------------------------------
## Device Setup
**For ease of initial setup, enable Serial debug output within **User.h** and monitor output. You will get the device IP and be able to monitor activity before final deployment**  

1. Flash firmware
    Use must power on/off after flashing firmware due a bug within esp8266 causing a reboot to hang  

2. Device will initially come up with its own *Access Point* called esp82XX-xxxxxxx. Connect to this and configure WiFi parameters. Once saved, device will reboot and connect to your WiFi  

3. Once device is connected to WiFi, get device IP from serial output and connect to it using Browser
   User/Password are stored in **sonoff/src/User.h**  

4. Configure device parameters on web page  
       - The MQTT section should be self explanatory  
       - The Inbound message type is received by the device containing either **ON** or **OFF**  
       - The Outbound message type is sent by the device containing the action carried out either **ON** or **OFF**  
       - You may chose to wipe out out previously configured WiFi settings which will also reboot the device  
       - Once all MQTT and message types are correct, save settings and device will reboot and reconnect to your WiFi and MQTT Broker  

**Above steps should be done over USB-->Serial interface until device is fully functioning**  

5. Test device and once ok, turn off debugging within **User.h** and upload firmware one last time  


-------------------------------------------------------------------------------------------------------------
## Finding device IP Address
To get the device IP address you have the following options:  

  1. Look at the Serial output where it will show on startup (assuming you have debug output turned on)  
  
  2. Look in your router to see WiFi clients  
  
  3. Try an mDNS browser app but this often takes a very time to get the device showing up  
  
  4. If already connected to WiFi and MQTT Broker, you can send a blank MQTT message as defined in user.h at **"IP_REQUEST"**  
     Device will respond with a MQTT message such as defined with **"IP/REPLY/IP address"** with the IP address in the payload  

-------------------------------------------------------------------------------------------------------------
## Credits
I am simply reusing other peoples amazing work for instance the following libraries:  
  - [PubSubClient](https://github.com/knolleary/pubsubclient)  
  - [WifiManager](https://github.com/tzapu/WiFiManager)  
  - [ArduinoJson](https://github.com/bblanchon/ArduinoJson)  

AND OF COURSE the many examples on github  

My development environment is with Atom with its builtin PlatformIO toolset. Its a fantastic build and debug environment!
