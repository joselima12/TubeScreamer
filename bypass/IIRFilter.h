#ifndef IIRFILTER_H
#define IIRFILTER_H

struct IIRFilter
{
    float alpha;

    float out;
};

void IIRFilter_Init(IIRFilter *filt, float alpha);

float IIRFilter_Update(IIRFilter *filt, float in);

#endif