#ifndef CONSTANTS_FILE
#define CONSTANTS_FILE

#define LED_PIN 2
#define SETUP_PIN 5

#define RELAY_COUNT 1
#define RELAY_PIN0 4

const char *board_name PROGMEM = "relayLight";

#define MQTT_STATE "/state"
#define MQTT_RELAY "/relay"

#endif