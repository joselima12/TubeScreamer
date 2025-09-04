#include "Oversampler2x.h"

void Oversampler2x::prepare()
{
    y1 = y2 = 0.0f;
}

void Oversampler2x::upsample(float x, float& x1, float& x2)
{
    x1 = x2 = x; // zero-order hold
}

float Oversampler2x::downsample(float y0)
{
    float result = 0.25f * y2 + 0.5f * y1 + 0.25f * y0;
    y2 = y1;
    y1 = y0;
    return result;
}
