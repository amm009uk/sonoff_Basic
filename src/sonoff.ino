#include "User.h"                                                    // Our custom settings

/*----------------------------------------- Global variables -----------------------------------------*/
const static  String version = "1.8";                                // Version control no

WiFiClient                  WiFiClient;                              // The WiFi client object
WiFiManager                 wifiManager;         		                 // WiFiManager will do all the work of managing WiFi
PubSubClient                MQTTclient(WiFiClient);                  // MQTT client object
MDNSResponder mDNS;                                                  // Multi-Cast DNS object
ESP8266WebServer            httpServer(80);                          // WebServer on port 80

char deviceID[20]         = "                   ";                   // User specified name stored in configuration file

/* MQTT Settings */
char mqtt_server[30]      = "                            ";
int  mqtt_port;
char mqtt_user[10]        = "         ";
char mqtt_password[10]    = "         ";
char mqtt_inTopic[30]     = "                            ";
char mqtt_outTopic[30]    = "                            ";
long lastReconnectAttempt = 0;
long now;                                                            // Hold current millisecs device has been running

int state                 = 0;                                       // Initial state of power
boolean btnState          = false;                                   // Hold current state of relay

/* ------------------------------------------ Start of code ------------------------------------------*/

void setup() {

  #ifdef SERIAL_DEBUG
  Serial.begin(115200);
	delay(5000);
  debugln(F("************ Setup() started **************"));
  #endif

  if (!SPIFFS.begin()) {                                             // Start filesystem
  #ifdef SERIAL_DEBUG
    debugln(F("File system failed to mount"));
  #endif
    return;
  }

  #ifdef WIPE_SPIFFS
	if (SPIFFS.format()) {
		debugln(F("File System Formated"));
	}	else {
		debugln(F("File System Formatting Error"));
	}
  #endif

  if (!loadConfig()) {                                               // Load configuration file
  	return;
  }

  #ifdef SERIAL_DEBUG
  debugln(F("..Configuration start"));
  debug(F("....deviceID:       ")); debugln(deviceID);
  debug(F("....State:          ")); debugln(String(state));
  debug(F("....MQTT Server:    ")); debugln(mqtt_server);
  debug(F("....MQTT Port:      ")); debugln(String(mqtt_port));
  debug(F("....MQTT User:      ")); debugln(mqtt_user);
  debug(F("....MQTT Password:  ")); debugln(mqtt_password);
  debug(F("....MQTT inTopic:   ")); debugln(mqtt_inTopic);
  debug(F("....MQTT outTopic:  ")); debugln(mqtt_outTopic);

  debugln(F("..Configuration end"));
  #endif

	WiFi.hostname(deviceID);                                           // Set the hostname on network

  #ifdef SERIAL_DEBUG
	wifiManager.setDebugOutput(true);
  #else
	wifiManager.setDebugOutput(false);
  #endif

	wifiManager.setConfigPortalBlocking(false);                        // Config Portal will not block loop() from running
  if(!wifiManager.autoConnect(deviceID, WWW_PASSWD)) {               // Fetch WiFi values (from EEPROM) and try to connect
  }

  if (!mDNS.begin(deviceID, WiFi.localIP())) {                       // Start the mDNS responder for <deviceID>.local
  #ifdef SERIAL_DEBUG
    debugln(F("..Error setting up MDNS responder!"));
  #endif
    return;
  }

  mDNS.addService("sonoff", "tcp", 80);                              // Add service availability

  httpServer.onNotFound(handleNotFound);
	httpServer.on("/",             handleRoot);
	httpServer.on("/saveChanges",  handlesaveChanges);
	httpServer.on("/resetwifi",    handleWiFiReset);

  httpServer.begin();                                                // Start Web Server
  delay(10);

  MQTTclient.setServer(mqtt_server, mqtt_port);                      // Initialse MQTT client
  MQTTclient.setCallback(callback);                                  // Callback service for receiving MQTT messages

  // Initialise PIN's
	pinMode(LED_PIN, OUTPUT);                                          // LED
	pinMode(RELAY_PIN, OUTPUT);                                        // Relay for power
	pinMode(TOGGLE_PIN, INPUT);                                        // Onboard momentary switch
	pinMode(EXT_PIN, OUTPUT);                                          // Local switch (input and output)

  digitalWrite(EXT_PIN, HIGH);                                       // Not sure why I have to do this but it won't work otherwise!

  delay(10);

	// Initialise from previously saved state
	if (state == 1) {
		digitalWrite(LED_PIN, LOW);
		digitalWrite(RELAY_PIN, HIGH);
	}	else {
		digitalWrite(LED_PIN, HIGH);
		digitalWrite(RELAY_PIN, LOW);
	}

  // Require both of these statements to disable rogue AP
	WiFi.softAPdisconnect (true);
	WiFi.mode(WIFI_STA);

  delay(10);

  MQTTconnect();                                                     // Initial MQTT connection

  #ifdef SERIAL_DEBUG
  debugln(F("************ Setup() finished *************"));
  #endif

} // setup()


void relayToggle() {                                                 // Toggle relay and LED

	if(!digitalRead(RELAY_PIN)) {
  #ifdef SERIAL_DEBUG
	  debugln(F("Power On"));
  #endif
		digitalWrite(LED_PIN, LOW);
		digitalWrite(RELAY_PIN, HIGH);
		MQTTclient.publish(mqtt_outTopic, "ON");
		state = 1;

	} else {
  #ifdef SERIAL_DEBUG
		  debugln(F("Power Off"));
  #endif
			digitalWrite(LED_PIN, HIGH);
			digitalWrite(RELAY_PIN, LOW);
			MQTTclient.publish(mqtt_outTopic, "OFF");
			state = 0;
	}

	saveConfig();

} // relayToggle()


void relayOn() {                                                     // Toggle relay and LED

  #ifdef SERIAL_DEBUG
	debugln(F("Power On"));
  #endif
  
  digitalWrite(LED_PIN, LOW);
	digitalWrite(RELAY_PIN, HIGH);
	MQTTclient.publish(mqtt_outTopic, "ON");

	state = 1;
	saveConfig();

} // relayOn()


void relayOff() {                                                    // Toggle relay and LED

  #ifdef SERIAL_DEBUG
	debugln(F("Power Off"));
  #endif
  
  digitalWrite(LED_PIN, HIGH);
	digitalWrite(RELAY_PIN, LOW);
	MQTTclient.publish(mqtt_outTopic, "OFF");

	state = 0;
	saveConfig();

} // relayOff()


void reboot() {
//  ESP.reset();
  ESP.restart();
  delay(1000);
} //reboot()


void loop() {

	//
	// Non-blocking connect to MQTT if WiFi is connected.
  // This allows other parts of the loop to run whilst no MQTT connection
  // If theres no WiFi that will be handled by WiFiManager
  //
  if ((WiFi.status() == WL_CONNECTED) && !MQTTclient.connected()) {
    now = millis();
    if ( (now - lastReconnectAttempt) > MQTT_DELAY_RECONNECT) {      // Attempt MQTT conncection if we tried over 5 secs ago
      lastReconnectAttempt = now;
      // Attempt to reconnect MQTT
      if (MQTTconnect()) {
        // Client connected
        lastReconnectAttempt = 0;
      }
    }
  }

  if (MQTTclient.connected()) {
    MQTTclient.loop();                                               // If MQTT Client connected check for MQTT activity
    delay(10);
  }

	wifiManager.process();                                             // WiFi Manager Configuration Portal

	httpServer.handleClient();                                         // Give time to Web Server
	delay (10);

	//
	// Onboard toggle button
	//
	if(!digitalRead(TOGGLE_PIN))
		relayToggle();

	//
	// External button one way
	//
	if(!digitalRead(EXT_PIN) && (!btnState)) {
		btnState = true;
		relayToggle();
	}

	//
	// External button other way
	//
	if(digitalRead(EXT_PIN) && (btnState)) {
		btnState = false;
		relayToggle();
	}

	yield();

} // loop()


void debug(String message) {
	Serial.print(message);
}


void debugln(String message) {
	Serial.println(message);
}
