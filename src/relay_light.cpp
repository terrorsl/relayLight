#include"relay_light.h"
#include"constants.h"
#include<ArduinoJson.h>

void onMqttConnect(bool sessionPresent)
{
    relay.setup_mqtt_subscribe();
};
void mqttOnMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    relay.update_mqtt(topic, payload);
};

void RelayLight::setup()
{
    Serial.begin(115200);

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
    String name="relayLight"+ESP.getChipId();
    String topic=name+MQTT_RELAY+String("/0");
    mqtt.subscribe(topic.c_str(),0);
};
void RelayLight::update_mqtt(const char *topic, const char *payload)
{
    //if(topic==)
    DynamicJsonDocument doc(256);
	deserializeJson(doc, payload);

    Serial.printf("%s-%s\n", topic, payload);

    int index = atol(&topic[strlen(topic)-1]);
    if(index==RELAY_PIN0)
    {
        if(doc["switch"]==true)
        {
            digitalWrite(RELAY_PIN0, HIGH);
        }
        else
        {
            digitalWrite(RELAY_PIN0, LOW);
        }
    }
    //doc["switch"];
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