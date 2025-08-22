#include "TubeScreamer.h"

void TubeScreamer::prepare(float sampleRate)
{
    toneFilter.prepare(sampleRate * 2.0f); // Prepare the tone filter for oversampling
    oversampler.prepare();
    clippingStage.prepare(sampleRate); // Prepare the clipping stage
}

void TubeScreamer::setTone(float R, float C)
{
    toneFilter.setResistor(R);
    toneFilter.setCapacitor(C);
}

float TubeScreamer::processSample(float input, float potValue)
{
    float x1, x2;
    oversampler.upsample(input, x1, x2);

    float y1 = toneFilter.processSample(clippingStage.processSample(x1, potValue));
    float y2 = toneFilter.processSample(clippingStage.processSample(x2, potValue));

    return oversampler.downsample(y2);
}
