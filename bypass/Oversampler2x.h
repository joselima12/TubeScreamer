#pragma once

class Oversampler2x
{
public:
    void prepare();
    void upsample(float x, float& x1, float& x2);
    float downsample(float y0);

private:
    float y1 = 0.0f;
    float y2 = 0.0f;
};