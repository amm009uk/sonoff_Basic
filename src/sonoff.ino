#include <FS.h>                                                      // SPIFFS support
#include <ESP8266WiFi.h>                                             // ESP8266 Core WiFi Library
#include <ESP8266WebServer.h>                                        // Local WebServer used to serve the configuration portal
#include <ESP8266HTTPUpdateServer.h>                                 // OTA Updater: http://<ip>
#include <PubSubClient.h>                                            // MQTT Client Publish and Subscribe
#include <DNSServer.h>                                               // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266mDNS.h>                                             // Include the mDNS library
#include <WiFiManager.h>                                             // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoJson.h>                                             // Read, write JSON format
#include <Functions.h>                                               // Our functions
#include <User.h>                                                    // Our custom settings

/*----------------------------------------- Global variables -----------------------------------------*/

const String version = "1.4";                                        // Master version control

WiFiClient WiFiClient;                                               // The WiFi client object
PubSubClient MQTTclient(WiFiClient);                                 // MQTT client object
MDNSResponder mDNS;                                                  // Multi-Cast DNS object

char chipID[25]           = "                        ";              // Unique'ish chip ID found thru API
char deviceID[30]         = "                             ";         // User specified name stored in configuration file
int rebootAt;                                                        // How many mins to wait before auto-reboot
int state = 0;                                                       // Holds current state of relay
String freeHeap;

/* MQTT Settings */
char mqtt_server[40]      = "                                      ";
int  mqtt_port;
char mqtt_user[11]        = "          ";
char mqtt_password[11]    = "          ";
char mqtt_inTopic[40]     = "                                      ";
char mqtt_outTopic[40]    = "                                      ";
long lastReconnectAttempt = 0;

/* Web Server */
ESP8266WebServer        httpServer(80);                              // WebServer on port 80
ESP8266HTTPUpdateServer httpUpdater;                                 // OTA updates
String                  INDEX_HTML;                                  // To hold web page

/* SIFFS Config file */
bool    spiffsActive = false;
String  getContentType(String filename);                             // convert the file extension to the MIME type
bool    handleFileRead(String path);                                 // send the right file to the client (if it exists)

boolean btnState          = false;                                   // Hold current state of relay
long now;                                                            // Hold current time

/* ------------------------------------------ Start of code ------------------------------------------*/

void setup() {

  #ifdef SERIAL_DEBUG
    Serial.begin(115200);
    delay(5000);
  #endif

  #ifdef SERIAL_DEBUG
  debugln("\n\n************ Setup() started **************");
  #endif

  // Get chip's Unique ID to be used in various places
  uint32_t chipid = ESP.getChipId();
  snprintf(chipID, 25, "ESP82XX-%08X", chipid);
#ifdef SERIAL_DEBUG
  debug("..Chip ID: "); debugln(chipID);
#endif

  if (SPIFFS.begin()) {                                              // Start filesystem
#ifdef SERIAL_DEBUG
    debugln("..File system mounted successfully");
#endif
    spiffsActive = true;
  } else {
#ifdef SERIAL_DEBUG
    debugln("..File system failed to mount");
#endif
    return;
  }

  char* result;                                                      // Result from read/write of config file

  result = loadConfig();                                             // Load configuration file
  if (strcmp(result, "OK") != 0) {
#ifdef SERIAL_DEBUG
  	debug("..");debugln(result);
#endif
  	return;
  }

  result = loadState();                                              // Load state file
  if (strcmp(result, "OK") != 0) {
#ifdef SERIAL_DEBUG
		debugln("..");debugln(result);
#endif
  	return;
  }
  
	WiFi.hostname(deviceID);                                           // Set the hostname on the the network

#ifdef SERIAL_DEBUG
  debugln("..WiFiManager starting...");
#endif
  WiFiManager wifiManager;                                           // Initialise WiFiManager and it will do all the work of managing the WiFi
//  wifiManager.resetSettings();                                     // Wipes out previously saved WiFi settings
  wifiManager.setTimeout(300);                                       // Set timeout (secs) for completing WiFi configuration

//  if(!wifiManager.autoConnect(chipID, WWW_PASSWD)) {                 // Fetch SSID and Password from EEPROM and try to connect
  if(!wifiManager.autoConnect(chipID)) {                             // Fetch SSID and Password from EEPROM and try to connect
#ifdef SERIAL_DEBUG
    debugln("....Unable to connect WiFi so starting own AP");        // If it doesn't connect start an access point and go into
#endif
    delay(5000);                                                     // a blocking loop waiting for configuration or timeout
    ESP.restart();                                                   // Restart and try again
  }

  if (!mDNS.begin(deviceID, WiFi.localIP())) {                       // Start the mDNS responder for <deviceID>.local
#ifdef SERIAL_DEBUG
    debugln("..Error setting up MDNS responder!");
#endif
    return;
  }

  delay(10);

  WiFiInfo();                                                        // Now connected to WiFi so print out info

  // Setup http firmware update page
  httpUpdater.setup(&httpServer, UPDATE_PATH, WWW_USER, WWW_PASSWD);

  // Setup URL handlers
  httpServer.on("/", HTTP_GET, handleRoot);
  httpServer.on("/", HTTP_POST, handleRoot);
  httpServer.on("/saveChanges", HTTP_POST, saveChanges);
  httpServer.onNotFound(handleNotFound);

  httpServer.begin();                                                // Start Web Server
  delay(10);

  mDNS.addService("sonoff", "tcp", 80);                              // Add service availability

  MQTTclient.setServer(mqtt_server, mqtt_port);                      // Initialse MQTT client
  MQTTclient.setCallback(callback);                                  // Callback service for receiving MQTT messages
  lastReconnectAttempt = 0;

  delay(10);

  // Initialise PIN's
	pinMode(LED_PIN, OUTPUT);                                          // LED
	pinMode(RELAY_PIN, OUTPUT);                                        // Relay for power
	pinMode(TOGGLE_PIN, OUTPUT);                                       // Onboard momentary switch
	pinMode(EXT_PIN, OUTPUT);                                          // Local switch (input and output)
  
  delay(10);
  
	// Turn off power initially and initialise buttons
	digitalWrite(LED_PIN, HIGH);
	digitalWrite(RELAY_PIN, LOW);
  digitalWrite(TOGGLE_PIN, HIGH);
  digitalWrite(EXT_PIN, HIGH);
  
  delay(10);
  
	// Initialise from previous state
	if (state == 1) {
		digitalWrite(LED_PIN, LOW);
		digitalWrite(RELAY_PIN, HIGH);
	}	else {
		digitalWrite(LED_PIN, HIGH);
		digitalWrite(RELAY_PIN, LOW);
	}
		
#ifdef SERIAL_DEBUG
  debugln("************ Setup() finished *************\n\n");
#endif

} // setup()

void WiFiInfo() {

#ifdef SERIAL_DEBUG
  debugln("Running WiFiInfo()");
  debug("..Connected to WiFi: "); debugln(WiFi.SSID());
  debug("....mDNS started: "); debug(deviceID); debugln(".local");
  debug("....IP address:   "); debugln(WiFi.localIP().toString());
  debug("....MAC address:  "); debugln(WiFi.macAddress());
  debug("....Signal (dBm): "); debugln((String)WiFi.RSSI());
#endif

  delay(10);

} // WiFiStatus()

void reboot() {

#ifdef SERIAL_DEBUG
  debugln("Running reboot()");
#endif

  delay(2500);
  ESP.restart();

} //reboot()

void relayOn() {                                                     // Turn on relay and LED

#ifdef SERIAL_DEBUG
  debugln("In relayOn()");
#endif

	digitalWrite(LED_PIN, LOW);
	digitalWrite(RELAY_PIN, HIGH);
	MQTTclient.publish(mqtt_outTopic, "ON");
	state = 1;
	saveState();
	
	delay(100);
	
} // relayOn()

void relayOff() {                                                    // Turn off relay and LED

#ifdef SERIAL_DEBUG
  debugln("In relayOff()");

#endif

	digitalWrite(LED_PIN, HIGH);
	digitalWrite(RELAY_PIN, LOW);
	MQTTclient.publish(mqtt_outTopic, "OFF");
	state = 0;
	saveState();
	
	delay(100);
	
} // relayOff()

void loop() {

	//
	// Non-blocking reconnect to MQTT if WiFi is connected
  // This allows other parts of the loop to run whilst no MQTT connection
  // If theres no WiFi that will be handled by WiFiManager in blocking mode
  //
  if ((WiFi.status() == WL_CONNECTED) && !MQTTclient.connected()) {
    now = millis();
    if (now - lastReconnectAttempt > 5000) {                         // Attempt MQTT conncection if we tried over 5 secs ago
      lastReconnectAttempt = now;
      // Attempt to reconnect MQTT
      if (MQTTconnect()) {
        // Client connected
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // MQTT Client connected so check for MQTT activity
    MQTTclient.loop();
    delay(100);
  }

	//
	// Give time to Web Server
	//
  httpServer.handleClient();

	//
	// Handle onboard toggle pin
	//
	if(!digitalRead(TOGGLE_PIN)) {
		if (!digitalRead(RELAY_PIN))                                     // Power On
			relayOn();
		else
			relayOff();	                                                   // Power Off
	}

	//
	// External button one way
	//
	if(!digitalRead(EXT_PIN) && (!btnState)) {
#ifdef SERIAL_DEBUG
		debugln("Ext toggle On");
#endif

		btnState = true;
		if (!digitalRead(RELAY_PIN))                                     // Power On
			relayOn();
		else
			relayOff();
	}

	//
	// External button other way
	//
	if(digitalRead(EXT_PIN) && (btnState)) {
#ifdef SERIAL_DEBUG
		debugln("Ext toggle Off");
#endif

		btnState = false;
		if (digitalRead(RELAY_PIN))                                      // Power Off
			relayOff();
		else
			relayOn();
	}
	
	//
	// Handle reboot
	//
	if (rebootAt != 0) {
	  long millisecs = millis();

  	String systemUpTimeMn;
  	String systemUpTimeHr;
//  String systemUpTimeDy;

  	systemUpTimeMn = int((millisecs / (1000 * 60)) % 60);
  	systemUpTimeHr = int((millisecs / (1000 * 60 * 60)) % 24 );
//  systemUpTimeDy = int((millisecs / (1000 * 60 * 60 * 24)) % 365);

#ifdef SERIAL_DEBUG
//  debug("rebootAt: "); debug((String)rebootAt); debug("............. Min: "); debug(systemUpTimeMn); debug(" Hr: "); debugln(systemUpTimeHr);
#endif

  	if (systemUpTimeHr.toInt() == rebootAt) {
    	MQTTclient.publish("/SH/Rebooted", deviceID);
    	reboot();
  	}
  }
  
} // loop()

void debug(String message) {
	Serial.print(message);
	delay(SERIAL_DELAY);
}

void debugln(String message) {
	Serial.println(message);
	delay(SERIAL_DELAY);
}

void debugln() {
    Serial.println();
}