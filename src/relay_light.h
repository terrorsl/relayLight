#ifndef RELAY_LIGHT_FILE
#define RELAY_LIGHT_FILE

#include<WiFiManager.h>

#include <AsyncMqttClient.h>

class RelayLight
{
public:
    void setup();
    void setup_mqtt_subscribe();

    void update_mqtt(const char *topic, const char *payload);

    void loop();
private:
    void setup_pin();

    AsyncMqttClient mqtt;
    String boardName;
};

extern RelayLight relay;
#endif