#include "FIRFilter.h"


/* Constructor function */
FIRFilter::FIRFilter(size_t length) : buf(length, 0.0f), bufIndex(0), bufLength(length), out(0.0f) {}


float FIRFilter_Update(FIRFilter *fir, float inp, float *inTaps){

    /* Store latest sample in buffer */
    fir->buf[fir->bufIndex] = inp;

    /* Increment new buffer index and wrap around if necessary */
    fir->bufIndex++;

    if(fir->bufIndex == fir->bufLength){
        fir->bufIndex = 0;
    }

    /* Compute new output sample (via convolution) */
    fir->out = 0.0f;

    uint8_t sumIndex = fir->bufIndex;

    for (uint8_t n = 0; n < fir->bufLength; n++)
    {
        /* Decrement index and wrap if necessary */
        if (sumIndex > 0){
            sumIndex--;
        }
        else{
            sumIndex = fir->bufLength - 1;
        }

        /* Multiply impulse response with shifted input sample and add to output */
        fir->out += inTaps[n] * fir->buf[sumIndex];
        
    } 

    /* Return filtered output */
    return fir->out;
    
}