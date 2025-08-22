/*
 * ClipWDFa is the first part of the WDF implementation of a Tube Screamer clipping section as described in
 * "An Improved and Generalized Diode Clipper Model for Wave Digital Filters" by Werner et al.
 * It models the first stage of the clipping circuit, the input of the op-amp.
*/

#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>

class ClipWDFa
{
public:
    ClipWDFa() = default;

    void prepare(double sampleRate)
    {
        C2.prepare((float)sampleRate);
    }

    void reset()
    {
        C2.reset();
    }

    // takes voltage and returns voltage
    inline float processSample(float x)
    {
        // Set the voltage of the voltage source as the input signal
        Vs.setVoltage(x);

        // Accept the incident wave into the voltage source
        Vs.incident(S3.reflected());

        // The voltage across R5 is the output of this stage
        auto y = chowdsp::wdft::voltage<float>(R5); // y variable is auto type, compiler deduces type

        // Propagate the reflected wave through the series adaptor
        S3.incident(Vs.reflected());

        return y;
    }

private:

    chowdsp::wdf::Resistor<float> Rin{ 1.0f    };
    chowdsp::wdf::Resistor<float> RA { 220.0f  };
    chowdsp::wdf::Resistor<float> R5 { 10000.0f };

    chowdsp::wdf::Capacitor<float> C2{ 1.0e-6f, 48000.0f };

    chowdsp::wdf::WDFSeries<float> S1{ &RA, &R5 };
    chowdsp::wdf::WDFSeries<float> S2{ &C2, &S1 };
    chowdsp::wdf::WDFSeries<float> S3{ &Rin, &S2 };

    // Ideal voltage source that receives the input signal
    chowdsp::wdf::IdealVoltageSource<float> Vs{ &S3 };

    // MISSING Vb WHICH IS 4.5V
};