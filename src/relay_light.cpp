#include"relay_light.h"
#include"constants.h"
#include<ArduinoJson.h>
#include<LittleFS.h>

#define WIFI_MANAGER_PARAM_COUNT 4
WiFiManagerParameter *params[WIFI_MANAGER_PARAM_COUNT];

void saveWifiManagerParam()
{
    Serial.println("saveWifiManagerParam");
    
    relay.set_mqtt_params(params[0]->getValue(),atol(params[1]->getValue()),
        params[2]->getValue(), params[3]->getValue());
}

void onMqttConnect(bool sessionPresent)
{
    relay.setup_mqtt_subscribe();
};

void mqttOnMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    relay.update_mqtt(topic, payload);
};

RelayLight::RelayLight()
{
};

void RelayLight::setup()
{
    Serial.begin(115200);

    Serial.println("setup pin");
    setup_pin();
#if defined(ESP8266)
    LittleFS.begin();
#else
    LittleFS.begin(true);
#endif

    DynamicJsonDocument doc=loadConfig();
    mqtt_server=doc["mqtt_server"].as<String>();
    uint16_t port = doc["mqtt_port"].as<unsigned short>();
    mqtt.setServer(mqtt_server.c_str(), port);
    mqtt_login = doc["mqtt_login"].as<String>();
    mqtt_password = doc["mqtt_password"].as<String>();
    mqtt.setCredentials(mqtt_login.c_str(), mqtt_password.c_str());

    Serial.println(mqtt_server);
    Serial.println(port);
    Serial.println(mqtt_login);
    Serial.println(mqtt_password);

	/*fs::File file = LittleFS.open("/config.json","r");
    if(file)
    {
        Serial.println("load from config.json");
        DynamicJsonDocument doc(256);
        deserializeJson(doc,file);

        mqtt_server=doc["mqtt_server"].as<String>();
        uint16_t port = doc["mqtt_port"].as<unsigned short>();
        mqtt.setServer(mqtt_server.c_str(), port);
        mqtt_login = doc["mqtt_login"].as<String>();
        mqtt_password = doc["mqtt_password"].as<String>();
        mqtt.setCredentials(mqtt_login.c_str(), mqtt_password.c_str());

        Serial.println(mqtt_server);
        Serial.println(port);
        Serial.println(mqtt_login);
        Serial.println(mqtt_password);
        
        file.close();
    }
    else
    {
        mqtt.setServer("mqtt.dealgate.ru", 1883);
        mqtt.setCredentials("", "");
    }*/

    Serial.println("setup mqtt");
    mqtt.onConnect(onMqttConnect);
    mqtt.onMessage(mqttOnMessage);
#if defined(ESP8266)
    boardName=board_name+String(ESP.getChipId());
#else
    boardName=board_name+String((unsigned long)ESP.getEfuseMac());
#endif
    Serial.println(boardName);
    mqtt.setClientId(boardName.c_str());
    willTopic=boardName+MQTT_WILL;
    mqtt.setWill(willTopic.c_str(),0,true,"disconnected");

    WiFi.persistent(true);
}
void RelayLight::setup_pin()
{
    pinMode(LED_PIN, OUTPUT);
    pinMode(SETUP_PIN, INPUT_PULLUP);
    
    for(int index=0;index<RELAY_COUNT;index++)
    {
        pinMode(pins[index].pin, OUTPUT);
        digitalWrite(pins[index].pin, pins[index].safe_state);
    }

    digitalWrite(LED_PIN, HIGH);
};
void RelayLight::setup_mqtt_subscribe()
{
    Serial.println(boardName);
    for(int index=0;index<RELAY_COUNT;index++)
    {
        String topic=boardName+MQTT_RELAY+String("/")+String(index);
        mqtt.subscribe(topic.c_str(),0);
    }
};
DynamicJsonDocument RelayLight::loadConfig()
{
    DynamicJsonDocument doc(256);
    fs::File file=LittleFS.open("/config.json", "r");
    if(file)
    {
        deserializeJson(doc, file);
        file.close();
    }
    return doc;
};
void RelayLight::saveConfig(DynamicJsonDocument *doc)
{
    fs::File file=LittleFS.open("/config.json", "w");
    if(file)
    {
        serializeJson(*doc, file);
        file.close();
    }
};
void RelayLight::set_mqtt_params(const char *server, unsigned short port, const char *login, const char *password)
{
    mqtt_server=server;
    mqtt_login=login;
    mqtt_password=password;

    DynamicJsonDocument doc=loadConfig();
    doc["mqtt_server"]=mqtt_server;
    doc["mqtt_port"]=port;
    doc["mqtt_login"]=mqtt_login;
    doc["mqtt_password"]=mqtt_password;
    saveConfig(&doc);
};
void RelayLight::update_mqtt(const char *topic, const char *payload)
{
    //if(topic==)
    DynamicJsonDocument doc(256);
	deserializeJson(doc, payload);

    Serial.printf("%s-%s\n", topic, payload);

    unsigned char index = atol(&topic[strlen(topic)-1]);
    if(index >= RELAY_COUNT)
        return;

    digitalWrite(pins[index].pin, doc["switch"].as<unsigned char>());
    String state_topic=boardName+MQTT_RELAY_STATE+"/"+String(index);
    mqtt.publish(state_topic.c_str(),0,false,doc["switch"]);

    /*if(index==RELAY_PIN0)
    {
        if(doc["switch"]==true)
        {
            digitalWrite(RELAY_PIN0, HIGH);
        }
        else
        {
            digitalWrite(RELAY_PIN0, LOW);
        }
        String topic=boardName+MQTT_RELAY_STATE+"/"+String(index);
        mqtt.publish(topic.c_str(),0,false,doc["switch"]);
    }*/
};
void RelayLight::loop()
{
    if(digitalRead(SETUP_PIN)==LOW)
    {
        WiFiManager manager;
        params[0]=new WiFiManagerParameter("mqtt_server","MQTT Server",mqtt_server.c_str(),40);
        params[1]=new WiFiManagerParameter("mqtt_port","MQTT Port","1883",6);
        params[2]=new WiFiManagerParameter("mqtt_login","MQTT Login",mqtt_login.c_str(),40);
        params[3]=new WiFiManagerParameter("mqtt_password","MQTT Password",mqtt_password.c_str(),40);
        for(int index=0;index<WIFI_MANAGER_PARAM_COUNT;index++)
            manager.addParameter(params[index]);
        manager.setSaveConfigCallback(saveWifiManagerParam);
        manager.setTitle("Relay Light");
        digitalWrite(LED_PIN, LOW);
        manager.startConfigPortal(board_name,"12345678");
        /*if(manager.autoConnect(board_name, "12345678"))
        {
            //ESP.reset();
        }*/
        for(int index=0;index<WIFI_MANAGER_PARAM_COUNT;index++)
            delete params[index];
        digitalWrite(LED_PIN, HIGH);
    }
    if(WiFi.isConnected()==false)
    {
        Serial.println("Try connect to WIFI");
        WiFi.begin();
        delay(2000);
        return;
    }
    if(mqtt.connected()==false)
    {
        Serial.println("Try connect to MQTT");
        mqtt.connect();
        delay(500);
    }
}