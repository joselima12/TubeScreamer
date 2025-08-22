#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>

class RCFilter
{
    public:
        RCFilter(float R_val, float C_val);

        void prepare(float sampleRate);
        float processSample(float input);

        void setResistor(float newR);
        void setCapacitor(float newC);

    private:
        float R_value;
        float C_value;
        float fs = 48000.0f;

        chowdsp::wdf::Resistor<float> R;
        chowdsp::wdf::Capacitor<float> C;
        chowdsp::wdf::WDFSeries<float> series;
};
