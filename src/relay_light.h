#ifndef RELAY_LIGHT_FILE
#define RELAY_LIGHT_FILE

#include<WiFiManager.h>

#include <AsyncMqttClient.h>
#include<ArduinoJson.h>

#include"constants.h"

struct Relay
{
    unsigned char pin;
    unsigned char safe_state;
};

const Relay pins[]={
    {RELAY_PIN0, LOW},
    {RELAY_PIN1, LOW},
    {RELAY_PIN2, LOW},
    {RELAY_PIN3, LOW},
    {RELAY_PIN4, LOW},
    {RELAY_PIN5, LOW},
    {RELAY_PIN6, LOW},
    {RELAY_PIN7, LOW}
};

class RelayLight
{
public:
    RelayLight();

    void setup();
    void setup_mqtt_subscribe();

    void update_mqtt(const char *topic, const char *payload);

    void set_mqtt_params(const char *server, unsigned short port, const char *login, const char *password);

    void loop();
private:
    void setup_pin();

    DynamicJsonDocument loadConfig();
    void saveConfig(DynamicJsonDocument *doc);

    AsyncMqttClient mqtt;
    String boardName;
    String mqtt_server, mqtt_login, mqtt_password, willTopic;
};

extern RelayLight relay;
#endif