void handleNotFound() {

#ifdef SERIAL_DEBUG
  debugln("Running handleNotFound()");
#endif
  httpServer.send(404, "text/plain", "404: Not found");

} // handleNotFound()

void handleRoot() {

#ifdef SERIAL_DEBUG
  debugln("Running handleRoot()");
#endif
  if(!httpServer.authenticate(WWW_USER, WWW_PASSWD)) {               // Check user + password
    return httpServer.requestAuthentication();                       // Request user + password
  }
#ifdef SERIAL_DEBUG
  debugln("..HTTP status 200: Good URL and successfully authenticated");
#endif

  sendHTMLPage();
  delay(10);                                                         // Build the HTML (array) and send to browser

} // handleRoot()

void sendHTMLPage() {

#ifdef SERIAL_DEBUG
  debugln("Running sendHTMLPage()");
#endif

  INDEX_HTML  = "<!DOCTYPE html>\n";
  INDEX_HTML += "<html lang=\"en\">\n";
	INDEX_HTML += "<head>\n";
	INDEX_HTML += "<title>Configure Settings</title>\n";
	INDEX_HTML += "<meta http-equiv='Content-Type' content='text/html;charset=UTF-8'/>\n";

	INDEX_HTML += "</head>\n";
	INDEX_HTML += "<body> \n";

	INDEX_HTML += "<table border=\"0\" width=\"400\" align=\"center\">\n";
	INDEX_HTML += "<tr>\n";
	INDEX_HTML += "<td>\n";
	
	INDEX_HTML += "<h2>sonoff Configuration - ";
	INDEX_HTML += "<label>version</label>";
	INDEX_HTML += version;
	INDEX_HTML += "</h2>\n";

  INDEX_HTML += "<form action='/saveChanges' method='POST' target=\"_self\">\n";

/**********************************************************************************************/

	INDEX_HTML += "<fieldset>\n";
	INDEX_HTML += "<legend><h4>&nbsp;Device Details&nbsp;</h4></legend>\n";
	
	INDEX_HTML += "<table>\n";
	
	INDEX_HTML += "<tr>\n";
  INDEX_HTML += "<td><label>Device ID</label></td>\n";
  INDEX_HTML += "<td><input type=\"text\" name=\"deviceID\" size=\"30\" title=\"A Unique ID to help you find device with mDNS\" value=\"";
  INDEX_HTML += deviceID;
  INDEX_HTML += "\"></td>\n";
	INDEX_HTML += "</tr>\n";
	
	INDEX_HTML += "<tr>\n";  
  INDEX_HTML += "<td><label>Reboot Interval</label></td>\n";
  INDEX_HTML += "<td><input type=\"text\" name=\"rebootAt\" size=\"30\" title=\"Reboot device after N Hours or 0 to disable\" value=\"";
  INDEX_HTML += rebootAt;
  INDEX_HTML += "\"></td>\n";
  INDEX_HTML += "</tr>\n";
	
	INDEX_HTML += "</table>\n";
	
	INDEX_HTML += "</fieldset>\n";

/**********************************************************************************************/
	
	INDEX_HTML += "<fieldset>\n";
	INDEX_HTML += "<legend><h4>&nbsp;MQTT Broker Details&nbsp;</h4></legend>\n";
	
	INDEX_HTML += "<table>\n";	
	
	INDEX_HTML += "<tr>\n";
	INDEX_HTML += "<td><label>Server&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>\n";
  INDEX_HTML += "<td><input type=\"text\" name=\"mqtt_server\" size=\"30\" title=\"Broker address\" value=\"";
  INDEX_HTML += mqtt_server;
  INDEX_HTML += "\"></td>\n";
	INDEX_HTML += "</tr>\n";
  
	INDEX_HTML += "<tr>\n";
  INDEX_HTML += "<td><label>Server Port&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>\n";
  INDEX_HTML += "<td><input type=\"text\" name=\"mqtt_port\" size=\"30\" title=\"Broker port typically 1883\" value=\"";
  INDEX_HTML += mqtt_port;
  INDEX_HTML += "\"></td>\n";
  INDEX_HTML += "</tr>\n";
 
	INDEX_HTML += "<tr>\n";
	INDEX_HTML += "<td><label>User&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>\n";
  INDEX_HTML += "<td><input type=\"text\" name=\"mqtt_user\" size=\"30\" title=\"Broker username\" value=\"";
  INDEX_HTML += mqtt_user;
  INDEX_HTML += "\"></td>\n";
  INDEX_HTML += "</tr>\n";

	INDEX_HTML += "<tr>\n";
  INDEX_HTML += "<td><label>Password&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>\n";
  INDEX_HTML += "<td><input type=\"password\" name=\"mqtt_password\" size=\"30\" title=\"Broker password\" value=\"";
  INDEX_HTML += mqtt_password;
  INDEX_HTML += "\"></td>\n";
	INDEX_HTML += "</tr>\n";
	
	INDEX_HTML += "</table>\n";
	
	INDEX_HTML += "</fieldset>\n";

/**********************************************************************************************/

	INDEX_HTML += "<fieldset>\n";
	INDEX_HTML += "<legend><h4>&nbsp;Message Details&nbsp;</h4></legend>\n";

	INDEX_HTML += "<table>\n";	
	
	INDEX_HTML += "<tr>\n";
  INDEX_HTML += "<td><label>Inbound&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>\n";
  INDEX_HTML += "<td><input type=\"text\" name=\"mqtt_inTopic\" size=\"30\" title=\"Eg. cmnd/Power/LivingRoom\" value=\"";
  INDEX_HTML += mqtt_inTopic;
  INDEX_HTML += "\"/></td>\n";
  INDEX_HTML += "</tr>\n";

	INDEX_HTML += "<tr>\n";
  INDEX_HTML += "<td><label>Outbound&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>\n";
  INDEX_HTML += "<td><input type=\"text\" name=\"mqtt_outTopic6\" size=\"30\" title=\"Eg. stat/Power/LivingRoom\" value=\"";
  INDEX_HTML += mqtt_outTopic;
  INDEX_HTML += "\"/></td>\n";
	INDEX_HTML += "</tr>\n";
	
	INDEX_HTML += "</table>\n";

	INDEX_HTML += "</fieldset>\n";
	
/**********************************************************************************************/

  INDEX_HTML += "<br>\n";

	INDEX_HTML += "<table>\n";	
	INDEX_HTML += "<tr>\n";
  INDEX_HTML += "<td><input type=\"submit\" value=\"Save Changes\" class=\"btn-save\"></td>\n";
	INDEX_HTML += "<td><input type=button onClick=\"parent.location='/firmware'\" value='Firmware Update' class=\"btn-update\"></td>\n";
	INDEX_HTML += "</tr>\n";
	INDEX_HTML += "</table>\n";
	
	INDEX_HTML += "</form>\n";

	INDEX_HTML += "</td>\n";
	INDEX_HTML += "</tr>\n";
	INDEX_HTML += "</table>\n";

	INDEX_HTML += "</body>\n";
  INDEX_HTML += "</html>";

  httpServer.send(200, "text/html", INDEX_HTML);                     // Send HTTP status 200 (Ok) and HTML to the browser
  delay(100);

#ifdef SERIAL_DEBUG
  debugln("..Web Page loaded");
#endif

} // sendHTMLPage()

void saveChanges() {

#ifdef SERIAL_DEBUG
  debugln("Running saveChanges()");
#endif

  if (httpServer.args() !=  8) {
#ifdef SERIAL_DEBUG
    debugln("Wrong number args received from HTTP POST");
#endif
  	return;
  }
#ifdef SERIAL_DEBUG
  debug("..No. of arguments from web page save: "); debugln((String)httpServer.args());
#endif
   
//  for ( uint8_t i = 0; i < httpServer.args(); i++ ) {
//    debugln(httpServer.arg(i));
//  }

  strcpy(deviceID, httpServer.arg(0).c_str());
  rebootAt    = httpServer.arg(1).toInt();
  strcpy(mqtt_server, httpServer.arg(2).c_str());
  mqtt_port = httpServer.arg(3).toInt();
  strcpy(mqtt_user, httpServer.arg(4).c_str());
  strcpy(mqtt_password, httpServer.arg(5).c_str());
  strcpy(mqtt_inTopic, httpServer.arg(6).c_str());
  strcpy(mqtt_outTopic, httpServer.arg(7).c_str());

	delay(50);
  saveConfig();
  httpServer.send(200,"text/html", "<META http-equiv=\"refresh\" content=\"15;URL=/\">Changes saved! Rebooting please wait this page will refresh...");
//  delay(50);
  reboot();
  
} // saveChanges()