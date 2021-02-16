/*
 * color_helpers.h
 *
 */
#include <ArduinoJson.h>

#ifndef API_H_
#define API_H_

void notifyRGB()
{
  if (hap_on && hap_on->value.bool_value != DeviceData.IsOn)
  {
    hap_on->value.bool_value = DeviceData.IsOn;
    homekit_characteristic_notify(hap_on, hap_on->value);
  }
  if (hap_br && hap_br->value.int_value != DeviceData.Brigthness)
  {
    hap_br->value.int_value = DeviceData.Brigthness;
    homekit_characteristic_notify(hap_br, hap_br->value);
  }
  if (hap_hue && hap_hue->value.float_value != DeviceData.Hue)
  {
    hap_hue->value.float_value = DeviceData.Hue;
    homekit_characteristic_notify(hap_hue, hap_hue->value);
  }
  if (hap_saturation && hap_saturation->value.float_value != DeviceData.Saturation)
  {
    hap_saturation->value.float_value = DeviceData.Saturation;
    homekit_characteristic_notify(hap_saturation, hap_saturation->value);
  }
}

void handleGetVal()
{
  DynamicJsonDocument res(1024);

  res["isOn"] = DeviceData.IsOn;
  res["brigthness"] = DeviceData.Brigthness;
  res["speed"] = DeviceData.Speed;
  res["mode"] = DeviceData.Mode;
  res["hue"] = DeviceData.Hue;
  res["saturation"] = DeviceData.Saturation;

  char response[1024];
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleSetVal()
{
  DynamicJsonDocument req(1024);
  deserializeJson(req, server.arg("plain"));

  boolean IsOn = req["isOn"];
  int Brigthness = req["brigthness"];
  int Speed = req["speed"];
  int Mode = req["mode"];
  float Hue = req["hue"];
  float Saturation = req["saturation"];

  DeviceData.IsOn = IsOn;
  DeviceData.Brigthness = Brigthness;
  DeviceData.Speed = Speed;
  DeviceData.Mode = Mode;
  DeviceData.Hue = Hue;
  DeviceData.Saturation = Saturation;

  notifyRGB();
  set_strip();

  DynamicJsonDocument res(1024);

  res["success"] = true;
  res["message"] = "State updated.";

  char response[1024];
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

#endif /* API_H_ */
