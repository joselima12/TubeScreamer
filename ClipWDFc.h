/*
 * ClipWDFc is the third and final part of the WDF implementation of a Tube Screamer clipping section as described in
 * "An Improved and Generalized Diode Clipper Model for Wave Digital Filters" by Werner et al.
 * It models the final stage of the clipping circuit, the voltage across the diode pair which clip the signal.
*/

#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>

class ClipWDFc
{
public:
    ClipWDFc() = default;

    void prepare(double sampleRate)
    {
        C4.prepare((float)sampleRate);
    }

    void reset()
    {
        C4.reset();
    }

    void setPotResitanceValue(float newPotR)
    {
        constexpr float R6 = (float)5e3;
        Is.setResistanceValue(R6 + newPotR);
    }

    void switchDiodePair(float Is, float Vt)
    {
        dp.setDiodeParameters(Is, Vt, 2.0f); // 2 diodes
    }

    // Takes current and returns voltage
    inline float processSample(float x)
    {
        Is.setCurrent(x);

        dp.incident(P1.reflected());
        auto y = chowdsp::wdft::voltage<float>(C4);
        P1.incident(dp.reflected());

        return y;
    }

private:

    // Current source is the output of ClipWDFb
    chowdsp::wdf::ResistiveCurrentSource<float> Is;

    chowdsp::wdf::Capacitor<float> C4{ 51.0e-11f };  // changed from 51pF for lower freq cutoff

    chowdsp::wdf::WDFParallel<float> P1{ &Is, &C4 };

    // 1N914 diode pair at 25C and VR = 20V
    chowdsp::wdf::DiodePair<float> dp{ &P1, 25e-9f };
   
};