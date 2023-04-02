/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "ESP8266WiFi.h" // Only needed for Arduino 1.6.5 and earlier
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "SPI.h"
#include "Wire.h"
#include "AsyncMqttClient.h"
#include "Ticker.h"
#include "ArduinoJson.h"

const String SSID = "Adfnet";
const String WIFIPW = "33394151923058759658";

#define DISPLAY_I2C_ADDRESS 0x3C
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define MQTT_HOST IPAddress(192, 168, 52, 185)
#define MQTT_PORT 1883
#define WIFI_SSID "Adfnet"
#define WIFI_PASSWORD "33394151923058759658"
#define CMD_ONE_COLUMN_SCROLL_H_LEFT 0x2D

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire, -1);

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;
String oledMessage;
int fontSize = 4;

void connectToWifi()
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent)
{
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe("adfhome/pcdata", 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
  mqttClient.publish("$SYS/pc_monitor", 0, true, "Monitoring PC");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected())
  {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    display.println(error.f_str());
  }
  if (!error)
  {
    oledMessage = "";
    for (size_t i = 0; i <= 3; i++)
    {
      String name = doc[i]["name"].as<String>();
      String value = doc[i]["value"].as<String>();
      oledMessage += name + ":" + value + "   ";
    }
  }
}

void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void initDisplay()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(fontSize);
  display.setTextWrap(false);
  display.setTextColor(SSD1306_WHITE);
  display.setFont(_GFXFONT_H_);
  display.clearDisplay();
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  connectToWifi();
  initDisplay();
  pinMode(LED_BUILTIN, OUTPUT);
}

void showOLEDMessage(String message)
{
  // Set the minimum x Position by fontsize *6 (ADAFruit Documentation) and multiply by message size
  int minX = (fontSize * -6) * strlen(message.c_str());
  // Set current x to the display width (in my case 128)
  int x = display.width();
  display.setCursor(0, 0);
  while (x > minX)
  {
    display.clearDisplay();
    display.setCursor(x, 0);
    display.print(message);
    x = x - 2;
    display.display();
  }

  // displays content in buffer
}

void loop()
{
  showOLEDMessage(oledMessage);
}
