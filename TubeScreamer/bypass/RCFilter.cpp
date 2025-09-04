#include "RCFilter.h"

RCFilter::RCFilter(float R_val, float C_val)
    : R_value(R_val), C_value(C_val),
      R(R_value), C(C_value),
      series(&R, &C)
{
}

void RCFilter::prepare(float sampleRate)
{
    fs = sampleRate;
    C.prepare(sampleRate);
    series.propagateImpedance();
}

float RCFilter::processSample(float input)
{
    series.incident(input);
    series.reflected();
    return series.voltage();
}

void RCFilter::setResistor(float newR)
{
    R_value = newR;
    R.setResistanceValue(R_value);
    series.propagateImpedance();
}

void RCFilter::setCapacitor(float newC)
{
    C_value = newC;
    C.setCapacitanceValue(C_value);
    C.prepare(fs);
    series.propagateImpedance();
}
