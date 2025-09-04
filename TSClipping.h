#pragma once

#include "ClipWDFa.h"
#include "ClipWDFb.h"
#include "ClipWDFc.h"

class ClippingStage
{
public:
    ClippingStage();
    void setDrive(float drive);
    void reset();
    void prepare(float sampleRate);
    float processSample(float x, float potValue) noexcept;

private:

    ClipWDFa clipWDFa;
    ClipWDFb clipWDFb;
    ClipWDFc clipWDFc;

    const float rPot = 500000.0f; // Max pot resistance in ohms
};
