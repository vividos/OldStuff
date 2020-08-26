// linmap.h - linear mapping from long and float ranges
// (c) 1999 Michael Fink
// https://github.com/vividos/OldStuff/

#ifndef __linmap_h_
#define __linmap_h_

// Klasse konvertiert linear zwischen einem float-Zahlenbereich
// und einem long-Zahlenbereich
class linear_mapping
{
private:
  long add;
  float sub, mul;
public:
  linear_mapping(float min1 = 0.f, float max1 = 1.f,
    long min2 = 0L, long max2 = 100L)
  {
    add = min2;
    sub = min1;
    mul = (max2 - min2) / (max1 - min1);
  };

  long float2long(float val)
  {
    return (long)((val - sub) * mul + add);
  };

  float long2float(long val)
  {
    float i = ((val -add) / mul + sub);
    return i;
  };
};

#endif
