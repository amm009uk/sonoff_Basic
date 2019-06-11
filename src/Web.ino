void handleNotFound() {

  #ifdef SERIAL_DEBUG
	debugln(F("Running handleNotFound()"));
  #endif
  httpServer.send(404, "text/plain", "404: Not found");

} // handleNotFound()


void handleRoot() {                                                  // Send the main page

  #ifdef SERIAL_DEBUG
  debugln(F("Running handleRoot()"));
  #endif
  
  if(!httpServer.authenticate(WWW_USER, WWW_PASSWD)) {               // Check user + password
    return httpServer.requestAuthentication();                       // Request user + password
  }
  
  #ifdef SERIAL_DEBUG
	debugln(F("handleRoot(Reading index.html)"));
	debugln(F("..Variables"));
 	debug(F("....Version:        ")); debugln(version);
  debug(F("....deviceID:       ")); debugln(deviceID);
  debug(F("....MQTT Server:    ")); debugln(mqtt_server);
  debug(F("....MQTT Port:      ")); debugln(String(mqtt_port));
  debug(F("....MQTT User:      ")); debugln(mqtt_user);
  debug(F("....MQTT Password:  ")); debugln(mqtt_password);
  debug(F("....MQTT inTopic:   ")); debugln(mqtt_inTopic);
  debug(F("....MQTT outTopic:  ")); debugln(mqtt_outTopic);
  #endif

  String page;
  page += FPSTR(mainPage);
  
  if (MQTTclient.connected())
		page.replace("{{_MQTT}}", "Yes");
	else
		page.replace("{{_MQTT}}", "No");

	page.replace("{{_version}}",       version);
	page.replace("{{_deviceID}}",      deviceID);
	page.replace("{{_mqtt_server}}",   mqtt_server);
	page.replace("{{_mqtt_port}}",     String(mqtt_port));
	page.replace("{{_mqtt_user}}",     mqtt_user);
	page.replace("{{_mqtt_password}}", mqtt_password);
	page.replace("{{_mqtt_inTopic}}",  mqtt_inTopic);
	page.replace("{{_mqtt_outTopic}}", mqtt_outTopic);

	httpServer.send(200, "text/html", page);
	
  delay(50);

  #ifdef SERIAL_DEBUG
	debugln(F("HTML loaded"));
  #endif
	                           
} // handleRoot()


void handlesaveChanges() {

	#ifdef SERIAL_DEBUG
  debugln(F("Saving changes..."));
	#endif
	
  if (httpServer.args() !=  8) {
    #ifdef SERIAL_DEBUG
    debug(F("Wrong number of args received from HTTP POST: ")); debugln(String(httpServer.args()));
	  for ( uint8_t i = 0; i < httpServer.args(); i++ ) {
  	  debug((String) i); debug("  ");  debugln(httpServer.arg(i).c_str());
  	}
    #endif
  	return;
  }

  strcpy(deviceID, httpServer.arg(0).c_str());
  strcpy(mqtt_server, httpServer.arg(1).c_str());
  mqtt_port = httpServer.arg(2).toInt();
  strcpy(mqtt_user, httpServer.arg(3).c_str());
  strcpy(mqtt_password, httpServer.arg(4).c_str());
  strcpy(mqtt_inTopic, httpServer.arg(5).c_str());
  strcpy(mqtt_outTopic, httpServer.arg(6).c_str());

  saveConfig();
  httpServer.send(200,"text/html", "<META http-equiv=\"refresh\" content=\"15;URL=/\">Changes saved! Rebooting please wait this page will refresh...");
  delay(100);
  reboot();
  
} // handlesaveChanges()


void handleWiFiReset() {                                             // Wipe WiFi settings

	httpServer.send(200, "text/plain", "WiFi Settings erased and device is rebooting \n\n*** You must reconfigure its Access Point ***");
  delay(100);
  
  wifiManager.resetSettings();                                       // Wipes out previously saved WiFi settings
	
	reboot();
	
} // handleWiFiReset()
