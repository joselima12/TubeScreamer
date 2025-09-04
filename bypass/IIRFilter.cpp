#include "IIRFilter.h"

void IIRFilter_Init(IIRFilter *filt, float alpha){

    /* Check if alpha value is within boundaries and store to struct */
    if(alpha < 0.0f){
        filt->alpha = 0.0f;
    }
    else if (alpha > 1.0f){
        filt->alpha = 1.0f;
    }
    else{
        filt->alpha = alpha;
    }

    /* Reset Output */
    filt->out = 0.0f;
}

float IIRFilter_Update(IIRFilter *filt, float in){

    /* Compute output */
    filt->out = (1.0f - filt->alpha) * in + filt->alpha * filt->out;

    /* Return filtered output */
    return filt->out;
}