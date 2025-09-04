/*
 * ClipWDFb is the second part of the WDF implementation of a Tube Screamer clipping section as described in
 * "An Improved and Generalized Diode Clipper Model for Wave Digital Filters" by Werner et al.
 * It models the second stage of the clipping circuit, the current entering the feedback loop.
*/

//Basically a high-pass RC filter.

#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>

class ClipWDFb
{
public:
    ClipWDFb() = default;

    void prepare(double sampleRate)
    {
        C3.prepare((float)sampleRate);
    }

    void reset()
    {
        C3.reset();
    }

    // Takes voltage and returns current
    inline float processSample(float x)
    {
        // Input is the output voltage from ClipWDFa
        Vs.setVoltage(x);

        Vs.incident(S1.reflected());

        // The current through R4 is the output of this stage
        // Note: This is the current entering the feedback loop
        auto y = chowdsp::wdft::current<float>(R4);

        S1.incident(Vs.reflected());

        return y;
    }

private:
    chowdsp::wdf::Resistor<float> R4{ 4700.0f };

    chowdsp::wdf::Capacitor<float> C3{ 47.0e-9f, 48000.0f };

    chowdsp::wdf::WDFSeries<float> S1{ &C3, &R4 };

    // Output voltage from ClipWDFa
    chowdsp::wdf::IdealVoltageSource<float> Vs{ &S1 };
};