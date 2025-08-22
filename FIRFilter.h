#ifndef FIR_FILTER_H
#define FIR_FILTER_H

#include <stdint.h>
#include <vector>

struct FIRFilter
{
    std::vector<float> buf;  // Dynamic array
    uint8_t bufIndex;
    uint8_t bufLength;

    float out;

    // Constructor declaration
    FIRFilter(size_t length);
};

// Update recieves the filter struct, the input and the taps array
float FIRFilter_Update(FIRFilter *fir, float inp, float *inTaps);

#endif