bool    loadConfig();
bool    saveConfig();

void    handleNotFound();
void    handleRoot();
void    saveChanges();
void    handleWiFiReset();

boolean MQTTconnect();
void    callback(char* topic, byte* payload, unsigned int length);

void    relayToggle();
void    relayOn();
void    relayOff();
void    reboot();
void    debug(String message);
void    debugln(String message);