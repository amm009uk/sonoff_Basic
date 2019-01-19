boolean MQTTconnect() {

#ifdef SERIAL_DEBUG
	rdebugAln("Attempting MQTT connection to %s::%d with %s/%s", mqtt_server, mqtt_port, mqtt_user, mqtt_password);
#endif

  if (MQTTclient.connect(deviceID, mqtt_user, mqtt_password)) {   
  	MQTTclient.subscribe(mqtt_inTopic);
  	MQTTclient.subscribe(IP_REQUEST);
#ifdef SERIAL_DEBUG
    rdebugAln("..MQTT connected");
#endif
  } else {
#ifdef SERIAL_DEBUG
    rdebugAln("..MQTT connection failed");
#endif
  }

  delay(10);
  return MQTTclient.connected();
  
} // MQTTconnect()

void callback(char* topic, byte* payload, unsigned int length) {

#ifdef SERIAL_DEBUG
  rdebugAln("In MQTT Callback() message arrived [%s]", topic);
#endif

  if (strcmp(topic, IP_REQUEST)==0) {                                // Check if we want this message

  	String replyMessage = IP_REPLY;                                  // Build the MQTT reply messsage name
  	replyMessage.concat(deviceID);                                   // ...

  	String Msg = WiFi.localIP().toString();                          // Build MQTT message payload contents

#ifdef SERIAL_DEBUG
		rdebugAln("MQTT Publish %s with payload %s", replyMessage.c_str(), Msg.c_str());
#endif

		MQTTclient.publish(replyMessage.c_str(), Msg.c_str());		       // Publish message to Broker
		return;

  }
  
	String msgContents;
	
  if (strcmp(topic, mqtt_inTopic)==0) {                              // Check if we want this message if so get the payload
    for (int i = 0; (i < (int)length); i++) {
			char receivedChar = (char)payload[i];
			msgContents.concat(receivedChar);			
    } 
  } else {                                                           // Not interested in this message
  		rdebugAln("..Message ignored");
  	return;
	}

#ifdef SERIAL_DEBUG
	rdebugAln("..Payload: %s", msgContents.c_str());
#endif

	//
	// Take appropriate action depending on message contents
	//

	if (msgContents == "ON") {
		relayOn();
	}
	
	if (msgContents == "OFF") {
		relayOff();
	}

  delay(10);

} // callback()