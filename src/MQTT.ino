boolean MQTTconnect() {

  #ifdef SERIAL_DEBUG
	debug(F("Attempting MQTT connection to ")); debug(mqtt_server); debug(F(":")); debug(String(mqtt_port)); debug(F(" with ")); debug(mqtt_user); debug(F("/")); debugln(mqtt_password);
  #endif

  if (MQTTclient.connect(deviceID, mqtt_user, mqtt_password)) {
  	if(!digitalRead(RELAY_PIN)) {
			MQTTclient.publish(mqtt_outTopic, "OFF");
		} else {
			MQTTclient.publish(mqtt_outTopic, "ON");
		}						
  	MQTTclient.subscribe(mqtt_inTopic);
  	MQTTclient.subscribe(IP_REQUEST);

    #ifdef SERIAL_DEBUG
    debugln(F("..MQTT connected"));
    #endif

  } else {
    #ifdef SERIAL_DEBUG
    debugln(F("..MQTT connection failed"));
    #endif
  }

  delay(10);
  return MQTTclient.connected();

} // MQTTconnect()

void callback(char* topic, byte* payload, unsigned int length) {

  #ifdef SERIAL_DEBUG
  debug(F("In MQTT Callback() message arrived ")); debugln(topic);
  #endif

  //
  // Block to process IP_REQUEST messages
  //
  if (strcmp(topic, IP_REQUEST)==0) {                                // Check if message is for IP request

  	String replyMessage = IP_REPLY;                                  // Build the MQTT reply messsage name
  	replyMessage.concat(deviceID);                                   // ...

  	String Msg = WiFi.localIP().toString();                          // Build MQTT message payload contents

    #ifdef SERIAL_DEBUG
		debug(F("MQTT Publish ")); debug(replyMessage.c_str()); debug(F(" with payload ")); debugln(Msg.c_str());
    #endif

		MQTTclient.publish(replyMessage.c_str(), Msg.c_str());		       // Publish message to Broker
		
		delay(10);
		return;

  }

  //
  // Block to process regular messages
  //
	String msgContents;
  if (strcmp(topic, mqtt_inTopic)==0) {                              // Check if message is to Power toggle
    for (int i = 0; (i < (int)length); i++) {
			char receivedChar = (char)payload[i];
			msgContents.concat(receivedChar);
    }
    
    #ifdef SERIAL_DEBUG
		debug(F("..Payload: ")); debugln(msgContents.c_str());
    #endif

		if (msgContents == "ON")
			relayOn();
		
		if (msgContents == "OFF")
			relayOff();
			
		delay(10);
 	  return;
  } 

  //
  // Not interested in this message
  //
  #ifdef SERIAL_DEBUG
	debugln(F("..Message ignored"));
  #endif

} // callback()