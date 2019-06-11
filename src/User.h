#define SERIAL_DEBUG                                                 // Uncomment to enable serial debug output
//#define WIPE_SPIFFS                                                  // Uncomment to wipe data (does not include WiFi)

#include <FS.h>                                                      // SPIFFS support
#include <ArduinoJson.h>                                             // Read, write JSON format
#include <ESP8266WiFi.h>                                             // ESP8266 Core WiFi Library
#include <ESP8266WebServer.h>                                        // Local WebServer used to serve the configuration portal
#include <PubSubClient.h>                                            // MQTT Client Publish and Subscribe
#include <DNSServer.h>                                               // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266mDNS.h>                                             // Include the mDNS library
#include <WiFiManager.h>                                             // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include "Functions.h"                                               // Our functions
#include "mainPage.h"                                                // Our main HTML page

#define LED_PIN								13																		 // LED indicator
#define RELAY_PIN							12																		 // Relay for power switch
#define TOGGLE_PIN					  00																		 // On board button
#define EXT_PIN   					  14																		 // GPIO14 external switch

#define MQTT_DELAY_RECONNECT  10000                                  // Delay between MQTT reconnects
#define WWW_USER              "admin"                                // User for web page
#define WWW_PASSWD            "esp8266ex"                            // Password for web page
#define CONFIGFILE            "/config.json"                         // Configuration file holding various values
#define IP_REQUEST            "IP/Request"                           // MQTT IP Request Message
#define IP_REPLY              "IP/Reply/"                            // MQTT IP Reply Message

//
// Following values are used if no config file is found
//
#define NEW_deviceID					"NEW-sonoff"
#define NEW_mqtt_server       "0.0.0.0"
#define NEW_mqtt_port         1883
#define NEW_mqtt_user         "user"
#define NEW_mqtt_password     "password"
#define NEW_mqtt_inTopic      "cmnd/Power/NEW-sonoff"
#define NEW_mqtt_outTopic     "stat/Power/NEW-sonoff"
