#include"relay_light.h"
#include"constants.h"

void onMqttConnect(bool sessionPresent)
{
    relay.setup_mqtt_subscribe();
};
void mqttOnMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{

};

void RelayLight::setup()
{
    mqtt.onConnect(onMqttConnect);
    mqtt.onMessage(mqttOnMessage);

    mqtt.setServer("mqtt.dealgate.ru", 1883);
    mqtt.setCredentials("", "");
}
void RelayLight::setup_pin()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(SETUP_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN0, OUTPUT);

    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN0, LOW);
};
void RelayLight::setup_mqtt_subscribe()
{
    String topic="relayLight"+ESP.getChipId();
    topic+=MQTT_RELAY;
    mqtt.subscribe(topic.c_str(),0);
};
void RelayLight::loop()
{
    if(digitalRead(SETUP_PIN)==LOW)
    {
        WiFiManager manager;
        digitalWrite(LED_PIN, HIGH);
        manager.autoConnect("relayLight", "1234567");
    }
    if(WiFi.isConnected()==false)
    {
        WiFi.begin();
        return;
    }
    if(mqtt.connected()==false)
    {
        mqtt.connect();
    }
}