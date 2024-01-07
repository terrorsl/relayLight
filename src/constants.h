#ifndef CONSTANTS_FILE
#define CONSTANTS_FILE

#if defined(ESP8266)
#define LED_PIN 2
#define SETUP_PIN 5

#define RELAY_COUNT 1
#define RELAY_PIN0 4
#else
#define LED_PIN 2
#define SETUP_PIN 5

#define RELAY_COUNT 1
#define RELAY_PIN0 4
#endif

#define board_name "relayLight"

#define MQTT_STATE "/state"
#define MQTT_RELAY "/relay"

#endif