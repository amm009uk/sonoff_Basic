char*   loadConfig();
char*   saveConfig();
char*   loadState();
char*   saveState();

void    handleNotFound();
void    handleRoot();
void    sendHTMLPage();
void    saveChanges();

boolean MQTTconnect();
void    callback(char* topic, byte* payload, unsigned int length);
