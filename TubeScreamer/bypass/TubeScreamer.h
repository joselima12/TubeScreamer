#pragma once

#include "RCFilter.h"
#include "Oversampler2x.h"
#include "TSClipping.h"

class TubeScreamer
{
public:
    void prepare(float sampleRate);
    float processSample(float input, float potValue);

    void setGain(float g) { clippingStage.setDrive(g); }
    void setTone(float R, float C);

private:
    RCFilter toneFilter { 1000.0f, 47e-9f };
    Oversampler2x oversampler;
    ClippingStage clippingStage;
};
