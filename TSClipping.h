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
    //float audioTaperPotSim(float in);
    //float fs = 44100.0f;

    ClipWDFa clipWDFa;
    ClipWDFb clipWDFb;
    ClipWDFc clipWDFc;

    const float rPot = 500000.0f;

    //juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> p1Smoothed;

    //dsp::Oversampling<float> oversampling{ 2, 1, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };

    //JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClippingStage)
};
