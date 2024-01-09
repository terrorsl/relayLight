#ifndef CONSTANTS_FILE
#define CONSTANTS_FILE

#define RELAY_COUNT 1

#if defined(ESP8266)
#define LED_PIN 2
#define SETUP_PIN 5

#define RELAY_PIN0 4
#define RELAY_PIN1 255
#define RELAY_PIN2 255
#define RELAY_PIN3 255
#define RELAY_PIN4 255
#define RELAY_PIN5 255
#define RELAY_PIN6 255
#define RELAY_PIN7 255
#else
#define LED_PIN 2
#define SETUP_PIN 5

#define RELAY_PIN0 4
#define RELAY_PIN1 255
#define RELAY_PIN2 255
#define RELAY_PIN3 255
#define RELAY_PIN4 255
#define RELAY_PIN5 255
#define RELAY_PIN6 255
#define RELAY_PIN7 255
#endif

#define GET_RELAY_PIN(index) RELAY_PIN##index

#define board_name "relayLight"

#define MQTT_INFO "/info"
#define MQTT_WILL "/status"
#define MQTT_RELAY_STATE "/relay/state"
#define MQTT_RELAY "/relay"

#endif