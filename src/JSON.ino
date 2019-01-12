char* loadConfig() {                                                 // Load JSON conig file from SPIFFS

#ifdef SERIAL_DEBUG
  debugln("Running loadConfig()");
#endif

  File configFile = SPIFFS.open(CONFIGFILE, "r");
  if (!configFile) {
    return (char*)"Failed to open config file";
  }

  const size_t bufferSize = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 220;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  std::unique_ptr<char[]> buf(new char[bufferSize]);                 // Allocate a buffer to store contents of the file

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), bufferSize);

  JsonObject& root = jsonBuffer.parseObject(buf.get());
  if (!root.success()) {
    return (char*)"Failed to parse config file correctly (root section)";
  }

  const char* t_deviceID       = root["deviceID"];     strcpy(deviceID, t_deviceID);
  int         t_rebootAt       = root["rebootAt"];     rebootAt    = t_rebootAt;

  JsonObject& MQTT             = root["MQTT"];
  if (!MQTT.success()) {
    return (char*)"Failed to parse config file correctly (MQTT section)";
  }

  const char* t_MQTT_server      = MQTT["Server"];      strcpy(mqtt_server,      t_MQTT_server);
  int         t_MQTT_Port        = MQTT["Port"];        mqtt_port =              t_MQTT_Port;
  const char* t_MQTT_user        = MQTT["User"];        strcpy(mqtt_user,        t_MQTT_user);
  const char* t_MQTT_password    = MQTT["Password"];    strcpy(mqtt_password,    t_MQTT_password);
  const char* t_MQTT_inTopic     = MQTT["inTopic"];     strcpy(mqtt_inTopic,     t_MQTT_inTopic);
  const char* t_MQTT_outTopic    = MQTT["outTopic"];    strcpy(mqtt_outTopic,    t_MQTT_outTopic);

#ifdef SERIAL_DEBUG
  debugln("..Config file start");
  debug("....deviceID:         "); debugln(deviceID);
  debug("....Reboot At:        "); debugln((String)rebootAt);
  debug("....MQTT Server:      "); debugln(mqtt_server);
  debug("....MQTT Port:        "); debugln((String)mqtt_port);
  debug("....MQTT User:        "); debugln((String)mqtt_user);
  debug("....MQTT Password:    "); debugln((String)mqtt_password);
  debug("....MQTT inTopic:     "); debugln(mqtt_inTopic);
  debug("....MQTT outTopic:    "); debugln(mqtt_outTopic);
  debugln("..Config file end");
#endif

  delay(50);
  return (char*)"OK";

} // loadConfig()

char* saveConfig() {                                                 // Save JSON config file to SPIFFS

#ifdef SERIAL_DEBUG
  debugln("Running saveConfig()");
#endif

  File configFile = SPIFFS.open(CONFIGFILE, "w");
  if (!configFile) {
    return (char*)"Failed to open config file for writing";
  }

  const size_t bufferSize = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root     = jsonBuffer.createObject();
  root["deviceID"]     = deviceID;
  root["rebootAt"]     = rebootAt;

  JsonObject& MQTT     = root.createNestedObject("MQTT");
  MQTT["Server"]       = mqtt_server;
  MQTT["Port"]         = mqtt_port;
  MQTT["User"]         = mqtt_user;
  MQTT["Password"]     = mqtt_password;
  MQTT["inTopic"]      = mqtt_inTopic;
  MQTT["outTopic"]     = mqtt_outTopic;

  root.printTo(configFile);
  delay(50);

#ifdef SERIAL_DEBUG
  debugln("..Config changes saved");
#endif

  return (char*)"OK";

} // saveConfig()

char* loadState() {                                                  // Load JSON state file from SPIFFS

#ifdef SERIAL_DEBUG
  debugln("Running loadState()");
#endif

  File stateFile = SPIFFS.open(STATEFILE, "r");
  if (!stateFile) {
    return (char*)"Failed to open state file";
  }

  const size_t bufferSize = JSON_OBJECT_SIZE(1) + 10;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  std::unique_ptr<char[]> buf(new char[bufferSize]);                 // Allocate a buffer to store contents of the file

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  stateFile.readBytes(buf.get(), bufferSize);

  JsonObject& root = jsonBuffer.parseObject(buf.get());
  if (!root.success()) {
    return (char*)"Failed to parse state file correctly";
  }
  
	int         t_state    = root["state"];     state    = t_state;

#ifdef SERIAL_DEBUG
	debugln("..Initial state: " + String(state));
#endif

  delay(50);
  return (char*)"OK";
    
} // loadState()


char* saveState() {                                      // Save JSON state file to SPIFFS

#ifdef SERIAL_DEBUG
  debugln("Running saveState()");
#endif

  File stateFile = SPIFFS.open(STATEFILE, "w");
  if (!stateFile) {
    return (char*)"Failed to open state file for writing";
  }

  const size_t bufferSize = JSON_OBJECT_SIZE(1);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root  = jsonBuffer.createObject();
  root["state"]     = state;
  
	root.printTo(stateFile);

  delay(50);
  return (char*)"OK";

} // saveState()