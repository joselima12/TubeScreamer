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