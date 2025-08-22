#include "TSClipping.h"

ClippingStage::ClippingStage()
{
}

void ClippingStage::setDrive(float potValue)
{
    clipWDFc.setPotResitanceValue(potValue); // Set the pot resistance value based on drive
}
 
void ClippingStage::reset()
{
    clipWDFa.reset();
    clipWDFb.reset();
    clipWDFc.reset();
}

void ClippingStage::prepare(float sampleRate)
{
    //fs = (float)sampleRate;

    //p1Smoothed.setCurrentAndTargetValue(10.0f);
    //p1Smoothed.reset(sampleRate, 0.05);

    clipWDFa.prepare(sampleRate);
    clipWDFb.prepare(sampleRate);
    clipWDFc.prepare(sampleRate);
}

float ClippingStage::processSample(float x, float potValue) noexcept
{
    setDrive(potValue); 

    const float clipWDFaOut = clipWDFa.processSample(x);
    const float clipWDFbOut = clipWDFb.processSample(clipWDFaOut);
    return clipWDFc.processSample(clipWDFbOut);
}

/* float ClippingStage::audioTaperPotSim(float in)
{
    jassert(in >= 0.0f && in <= 10.0f);

    if (in >= 0 && in <= 5.0f)
        return in / 5.0f;
    else 
        return  (9.0f / 5.0f) * in - 8.0f;
}
 */