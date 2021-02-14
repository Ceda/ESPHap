/*
 * color_helpers.h
 *
 */

#ifndef COLOR_HELPERS_H_
#define COLOR_HELPERS_H_

#define REDVALUE(x) ((x >> 16) & 0xFF)
#define GREENVALUE(x) ((x >> 8) & 0xFF)
#define BLUEVALUE(x) ((x >> 0) & 0xFF)

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Convert Hue/Saturation/Brightness values to a packed 32-bit RBG color.
// hue must be a float value between 0 and 360
// saturation must be a float value between 0 and 1
// brightness must be a float value between 0 and 1
uint32_t HSVColor(float h, float s, float v)
{
  h = constrain(h, 0.0, 360.0);
  s = constrain(s, 0.0, 1.0);
  v = constrain(v, 0.0, 1.0);

  int i, b, p, q, t;
  float f;

  h /= 60.0; // sector 0 to 5
  i = floor(h);
  f = h - i; // factorial part of h

  b = v * 255;
  p = v * (1 - s) * 255;
  q = v * (1 - s * f) * 255;
  t = v * (1 - s * (1 - f)) * 255;

  switch (i)
  {
  case 0:
    return Color(b, t, p);
  case 1:
    return Color(q, b, p);
  case 2:
    return Color(p, b, t);
  case 3:
    return Color(p, q, b);
  case 4:
    return Color(t, p, b);
  default:
    return Color(b, p, q);
  }
}

void ColorToHSI(uint32_t rgbcolor, uint32_t brightness, double &Hue, double &Saturation, double &Intensity)
{
  uint32_t r = REDVALUE(rgbcolor);
  uint32_t g = GREENVALUE(rgbcolor);
  uint32_t b = BLUEVALUE(rgbcolor);

  if ((r < 0 && g < 0 && b < 0) || (r > 255 || g > 255 || b > 255))
  {
    Hue = Saturation = Intensity = 0;
    return;
  }

  if (g == b)
  {
    if (b < 255)
    {
      b = b + 1;
    }
    else
    {
      b = b - 1;
    }
  }
  uint32_t nImax, nImin, nSum, nDifference;
  nImax = max(r, b);
  nImax = max(nImax, g);
  nImin = min(r, b);
  nImin = min(nImin, g);
  nSum = nImin + nImax;
  nDifference = nImax - nImin;

  Intensity = (float)nSum / 2;

  if (Intensity < 128)
  {
    Saturation = (255 * ((float)nDifference / nSum));
  }
  else
  {
    Saturation = (float)(255 * ((float)nDifference / (510 - nSum)));
  }

  if (Saturation != 0)
  {
    if (nImax == r)
    {
      Hue = (60 * ((float)g - (float)b) / nDifference);
    }
    else if (nImax == g)
    {
      Hue = (60 * ((float)b - (float)r) / nDifference + 120);
    }
    else if (nImax == b)
    {
      Hue = (60 * ((float)r - (float)g) / nDifference + 240);
    }

    if (Hue < 0)
    {
      Hue = (60 * ((float)b - (float)r) / nDifference + 120);
    }
  }
  else
  {
    Hue = -1;
  }
  return;
}

#endif /* COLOR_HELPERS_H_ */
