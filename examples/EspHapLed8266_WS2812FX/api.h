/*
 * color_helpers.h
 *
 */

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
  server.send(200, FPSTR(TEXT_PLAIN), DeviceData.IsOn ? "1" : "0");
}

void handleSetVal()
{
  if (server.args() != 2)
  {
    server.send(505, FPSTR(TEXT_PLAIN), "Bad args");
    return;
  }
  //to do analyze
  bool isSucess = false;
  if (server.arg("var") == "ch1")
  {

    DeviceData.IsOn = (server.arg("val") == "true");
    Serial.println("Web SetOn");
    set_strip();
    notifyRGB();
    isSucess = true;
  }
  else if (server.arg("var") == "br")
  {
    DeviceData.Brigthness = server.arg("val").toInt();

    notifyRGB();
    set_strip();
    isSucess = true;
  }
  else if (server.arg("var") == "col")
  {
    // Serial.println("Web Set Color");
    uint32_t color = server.arg("val").toInt();
    double Hue;
    double Saturation;
    double Intensity;
    ColorToHSI(color, (float)DeviceData.Brigthness, Hue, Saturation, Intensity);
    DeviceData.Saturation = Saturation;
    DeviceData.Hue = Hue;
    notifyRGB();
    set_strip();
    isSucess = true;
  }
  if (isSucess)
    server.send(200, FPSTR(TEXT_PLAIN), "OK");
  else
    server.send(505, FPSTR(TEXT_PLAIN), "Bad args");
}

#endif /* API_H_ */
