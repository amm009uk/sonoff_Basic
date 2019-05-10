bool loadConfig() {                                                  // Load JSON conig file from SPIFFS

  File configFile = SPIFFS.open(CONFIGFILE, "r");
  if (!configFile) {
    #ifdef SERIAL_DEBUG
    debugln(F("No config file found will use default values"));
    #endif
		strcpy(deviceID,      NEW_deviceID);
		strcpy(mqtt_server,	  NEW_mqtt_server);
		mqtt_port =           NEW_mqtt_port;
		strcpy(mqtt_user,	    NEW_mqtt_user);
		strcpy(mqtt_password, NEW_mqtt_password);
		strcpy(mqtt_inTopic,  NEW_mqtt_inTopic);
		strcpy(mqtt_outTopic, NEW_mqtt_outTopic);
		
		return(true);
  }

	// Calculate buffer size to hold file contents
  const size_t bufferSize = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 220;
  // Allocate buffer to store contents of file
  std::unique_ptr<char[]> buf(new char[bufferSize]);                 

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), bufferSize);

	DynamicJsonDocument doc(bufferSize);
	DeserializationError error = deserializeJson(doc, buf.get());
	if (error) {
		debug("Error in Parsing JSON: "); debugln(error.c_str());
		configFile.close();
  	return false;
	}

	const char* t_deviceID       = doc["deviceID"];     strcpy(deviceID, t_deviceID);
	state                        = doc["state"];

	JsonObject MQTT = doc["MQTT"];

	const char* t_MQTT_server    = MQTT["Server"];      strcpy(mqtt_server,      t_MQTT_server);
	mqtt_port                    = MQTT["Port"]; 
	const char* t_MQTT_user      = MQTT["User"];        strcpy(mqtt_user,        t_MQTT_user);
	const char* t_MQTT_password  = MQTT["Password"];    strcpy(mqtt_password,    t_MQTT_password);
	const char* t_MQTT_inTopic   = MQTT["inTopic"];     strcpy(mqtt_inTopic,     t_MQTT_inTopic);
	const char* t_MQTT_outTopic  = MQTT["outTopic"];    strcpy(mqtt_outTopic,    t_MQTT_outTopic);
  
	configFile.close();

  #ifdef SERIAL_DEBUG
  debugln(F("Config file loaded"));
  #endif

  delay(50);
  return (true);

} // loadConfig()


bool saveConfig() {                                                  // Save JSON config file to SPIFFS

  SPIFFS.remove(CONFIGFILE);
  
  File configFile = SPIFFS.open(CONFIGFILE, "w");
  if (!configFile) {
  #ifdef SERIAL_DEBUG
    debugln(F("Failed to open config file for writing"));
  #endif
    return (false);
  }

  #ifdef SERIAL_DEBUG
  debugln(F("..Contents to save"));
  debug(F("....deviceID:       ")); debugln(deviceID);
  debug(F("....State:          ")); debugln(String(state));
  debug(F("....MQTT Server:    ")); debugln(mqtt_server);
  debug(F("....MQTT Port:      ")); debugln(String(mqtt_port));
  debug(F("....MQTT User:      ")); debugln(mqtt_user);
  debug(F("....MQTT Password:  ")); debugln(mqtt_password);
  debug(F("....MQTT inTopic:   ")); debugln(mqtt_inTopic);
  debug(F("....MQTT outTopic:  ")); debugln(mqtt_outTopic);
  #endif
  
// 	const size_t bufferSize = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6);
//  DynamicJsonDocument doc(bufferSize);
   DynamicJsonDocument doc(512);
  
  doc["deviceID"]  = deviceID;
	doc["state"]     = state;

	JsonObject MQTT = doc.createNestedObject("MQTT");
	
	MQTT["Server"]   = mqtt_server;
	MQTT["Port"]     = mqtt_port;
	MQTT["User"]     = mqtt_user;
	MQTT["Password"] = mqtt_password;
	MQTT["inTopic"]  = mqtt_inTopic;
	MQTT["outTopic"] = mqtt_outTopic;

  if (serializeJson(doc, configFile) == 0) {
  #ifdef SERIAL_DEBUG
    debugln(F("Failed to write config file"));
    delay(500);
  #endif
    return (false);
  }
  
  configFile.close();

  #ifdef SERIAL_DEBUG
  debugln(F("Config changes saved"));
  #endif

  delay(50);
  return (true);

} // saveConfig()
