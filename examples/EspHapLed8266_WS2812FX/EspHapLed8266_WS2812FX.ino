#include <Arduino.h>

#ifdef ESP32
#include <SPIFFS.h>
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "coredecls.h"
#endif

bool isWebserver_started = false;

#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <WS2812FX.h>    //https://github.com/kitesurfer1404/WS2812FX

#include "color_helpers.h" // Include color converters

#define RGB_LED_COUNT 40 //adopt for your project
#define RGB_LED_PIN 2    //adopt for your project

WS2812FX ws2812fx = WS2812FX(RGB_LED_COUNT, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

const char *HOSTNAME = "WS2812";

//#define ENABLE_WIFI_MANAGER

// If not enable wifi manager set ssid & password
#ifndef ENABLE_WIFI_MANAGER
const char *ssid = "";
const char *password = "";
#endif

extern "C"
{
#include "homeintegration.h"
}
#ifdef ESP8266
#include "homekitintegrationcpp.h"
#endif

#include <hapfilestorage/hapfilestorage.hpp>
#include <hapweb/hap_webserver.hpp>

#include "file_index_html.h"

struct device_data_t
{
  bool IsOn = false;
  uint8_t Brigthness = 0;
  float Hue = 0.0;
  float Saturation = 0.0;
};

device_data_t DeviceData;

homekit_service_t *hapservice = {0};
homekit_characteristic_t *hap_on = {0};
homekit_characteristic_t *hap_br = {0};
homekit_characteristic_t *hap_hue = {0};
homekit_characteristic_t *hap_saturation = {0};

void lamp_callback(homekit_characteristic_t *ch, homekit_value_t value, void *context);

void set_strip()
{
  Serial.println("Setting strip");
  Serial.println("IsOn");
  Serial.println(DeviceData.IsOn);
  Serial.println("Hue");
  Serial.println(DeviceData.Hue);
  Serial.println("Stauration");
  Serial.println(DeviceData.Saturation);
  Serial.println("Brigthness");
  Serial.println(DeviceData.Brigthness);
  
  if (DeviceData.IsOn)
  {
    uint32_t color = HSVColor(DeviceData.Hue, DeviceData.Saturation / 100.0, DeviceData.Brigthness / 100.0);
    ws2812fx.setColor(color);
    ws2812fx.setBrightness(DeviceData.Brigthness);
  }
  else
  {
    ws2812fx.setBrightness(0);
  }
}

#include "api.h" // Include api endpoints

void setup()
{

  
#ifdef ESP8266
  disable_extra4k_at_link_time();
#endif
  Serial.begin(115200);
  delay(10);
  // We start by connecting to a WiFi network
  set_strip();
#ifdef ESP32
  if (!SPIFFS.begin(true))
  {
    Serial.print("SPIFFS Mount failed");
  }
#endif
#ifdef ESP8266
  if (!SPIFFS.begin())
  {
    Serial.print("SPIFFS Mount failed");
  }
#endif

  /// Setting RGB WS2812
  ws2812fx.init();
  ws2812fx.setBrightness(DeviceData.Brigthness);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();

  init_hap_storage("/pair.dat");

  /// We will use for this example only one accessory (possible to use a several on the same esp)
  //Our accessory type is light bulb , apple interface will proper show that
  hap_setbase_accessorytype(homekit_accessory_category_lightbulb);

  /// init base properties
  hap_initbase_accessory_service(HOSTNAME, "Yurik72", "0", "EspHapLed", "1.0");
  //we will add only one light bulb service and keep pointer for nest using

  //adding rgb Accessory
  hapservice = hap_add_rgbstrip_service("RGB", lamp_callback, NULL);
  hap_on = homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_ON);
  hap_br = homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_BRIGHTNESS);
  hap_hue = homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_HUE);
  hap_saturation = homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_SATURATION);

  if (hap_br)
  {
    hap_br->value.int_value = DeviceData.Brigthness; // initial value
  }

// Wifi setup
#ifdef ENABLE_WIFI_MANAGER
  WiFiManager wifiManager;
  //  WiFi.onStationModeDisconnected(onWifiDisconnect);
  //  WiFi.onStationModeConnected(onWifiConnect);
  if (!wifiManager.autoConnect(HOSTNAME, NULL))
  {
    ESP.restart();
    delay(1000);
  }
#else
  WiFi.mode(WIFI_STA);
  WiFi.begin((char *)ssid, (char *)password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
#endif

  Serial.println(PSTR("WiFi connected"));
  Serial.println(PSTR("IP address: "));
  Serial.println(WiFi.localIP());
  hap_init_homekit_server();

  set_indexhml(FPSTR(INDEX_HTML));
  hap_webserver_begin();

#ifdef ESP8266
  if (hap_homekit_is_paired())
  {
#endif
    Serial.println(PSTR("Setting web server"));
    SETUP_FILEHANDLES
    server.on("/get", handleGetVal);
    server.on("/set", handleSetVal);
    server.begin();
    isWebserver_started = true;
#ifdef ESP8266
  }
  else
    Serial.println(PSTR("Web server is NOT SET, waiting for pairing"));
#endif
}

void loop()
{

#ifdef ESP8266
  hap_homekit_loop();
#endif

  if (isWebserver_started)
  {
    server.handleClient();
  }
  ws2812fx.service();
}

void lamp_callback(homekit_characteristic_t *ch, homekit_value_t value, void *context)
{
  Serial.println("lamp_callback");
  bool isSet = false;
  if (ch == hap_on && ch->value.bool_value != DeviceData.IsOn)
  {
    DeviceData.IsOn = ch->value.bool_value;
    isSet = true;
  }
  if (ch == hap_br && ch->value.int_value != DeviceData.Brigthness && DeviceData.IsOn)
  {
    DeviceData.Brigthness = ch->value.int_value;
    isSet = true;
  }
  if (ch == hap_hue && ch->value.float_value != DeviceData.Hue)
  {
    DeviceData.Hue = ch->value.float_value;
    isSet = true;
  }
  if (ch == hap_saturation && ch->value.float_value != DeviceData.Saturation)
  {
    DeviceData.Saturation = ch->value.float_value;
    isSet = true;
  }
  if (isSet)
  {
    set_strip();
  }
}
