#include"relay_light.h"
#include"constants.h"
#include<ArduinoJson.h>
#include<LittleFS.h>

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

    Serial.println("setup pin");
    setup_pin();

    /*LittleFS.begin();
	fs::File file = LittleFS.open("config.json","r");
    if(file)
    {
        DynamicJsonDocument doc(256);
        deserializeJson(doc,file);
        
        mqtt.setServer(doc["mqtt_server"].as<String>().c_str(), doc["mqtt_port"].as<unsigned short>());
        mqtt.setCredentials(doc["mqtt_login"].as<String>().c_str(), doc["mqtt_password"].as<String>().c_str());
        
        file.close();
    }*/

    Serial.println("setup mqtt");
    mqtt.onConnect(onMqttConnect);
    mqtt.onMessage(mqttOnMessage);

    mqtt.setServer("mqtt.dealgate.ru", 1883);
    mqtt.setCredentials("", "");
#if defined(ESP8266)
    String name=board_name+String(ESP.getChipId());
#else
    String name=board_name+String(ESP.getEfuseMac());
#endif
    mqtt.setClientId(name.c_str());
    //mqtt.setKeepAlive(15);

    WiFi.persistent(true);
}
void RelayLight::setup_pin()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(SETUP_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN0, OUTPUT);

    digitalWrite(LED_PIN, HIGH);
    digitalWrite(RELAY_PIN0, LOW);
};
void RelayLight::setup_mqtt_subscribe()
{
#if defined(ESP8266)
    String name=board_name+String(ESP.getChipId());
#else
    String name=board_name+String(ESP.getEfuseMac());
#endif
    Serial.println(name);
    for(int index=0;index<RELAY_COUNT;index++)
    {
        String topic=name+MQTT_RELAY+String("/")+String(index);
        mqtt.subscribe(topic.c_str(),0);
    }
    mqtt.publish("test",0,false,"test");
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
};
void RelayLight::loop()
{
    if(digitalRead(SETUP_PIN)==LOW)
    {
        WiFiManager manager;
        digitalWrite(LED_PIN, LOW);
        if(manager.autoConnect(board_name, "12345678"))
        {
            //ESP.reset();
        }
        digitalWrite(LED_PIN, HIGH);
    }
    if(WiFi.isConnected()==false)
    {
        Serial.println("Try connect to WIFI");
        WiFi.begin();
        delay(500);
        return;
    }
    if(mqtt.connected()==false)
    {
        Serial.println("Try connect to MQTT");
        mqtt.connect();
    }
}