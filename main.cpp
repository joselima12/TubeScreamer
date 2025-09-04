/* Included Libraries, Header files and namespaces */

#include <string.h>
#include <math.h>
#include "daisy_seed.h"
#include "daisysp.h"
#include "daisy_core.h"
#include "TubeScreamer.h"
#include "Controls.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

using controls::Controls;

DaisySeed hw;
TubeScreamer ts;

// 6 pots on A(1-6); 4 SPST toggle on D(7-10); 2 momentary footswitches on D25 & D26
constexpr size_t kNPots = 6, kNToggles = 4, kNFoots = 2, kNLeds = 2;
Controls <kNPots, kNToggles, kNFoots, kNLeds> ui;

float sampleRate;


void AudioCallback(AudioHandle::InterleavingInputBuffer in, 
                   AudioHandle::InterleavingOutputBuffer out, 
                   size_t size)
{

     // Update controls once per block
    ui.Update();

    // Toggle bypass via SPST
    bool on = ui.EffectOn();

    // Toggle bypass via momentary footswitch
    if(ui.FootRising(0))
    {
        ui.EffectToggle();
        on = ui.EffectOn();
    }    
    // Read pots for gain and tone
    float gain = ui.PotMapped(0, 0.0f, 500000.0f); // Map pot 0 to gain range
    float rTone = ui.PotMapped(2, 1000.0f, 20000.0f); // Map pot 2 to tone resistor range
    ts.setTone(rTone, 47e-9f);

    float preGain = ui.PotMapped(3, 0.0f, 1.0f); // Map pot 3 to pre-gain range
    float postGain = ui.PotMapped(5, 0.0f, 2.0f); // Map pot 5 to post-gain range

    for (size_t i = 0; i < size; i += 2)
    {
        if(on)
        {
            float x = in[i+1] * preGain;    // Apply pre-gain to the input signal
            x = ts.processSample(x, gain);  // Process the audio signal through the Tube Screamer
            out[i + 1] = x * postGain;      // Apply post-gain to the output signal
            ui.LedWrite(0, on);             // Light LED when not bypassed
        }
        else
        {
            float x = in[i+1];              // Bypass - just pass input to output
            out[i + 1] = x;
            ui.LedWrite(0, on);             // Don't light LED when bypassed
        }
        /* Pre and Post-Gain is only applied when the effect is on as Post-Gain is effectively the Volume*/
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    sampleRate = hw.AudioSampleRate();
    ts.prepare(sampleRate);

    ts.setGain( 10.0f );
    ts.setTone( 10000.0f, 0.22e-6f );

    const float ctrl_hz = hw.AudioSampleRate() / static_cast<float>(hw.AudioBlockSize());

    // Define pins
    const Pin pot_pins[kNPots]          = { A1, A2, A3, A4, A5, A6 };
    const Pin toggle_pins[kNToggles]    = { D7, D8, D9, D10 };
    const Pin foot_pins[kNFoots]        = { D25, D26 };
    const Pin led_pins[]                = { A7, A8 };

    ui.Init(hw, pot_pins, toggle_pins, foot_pins, led_pins, ctrl_hz);
    
    hw.StartAudio(AudioCallback);

    while (1){

        // Toggle DaisySeed LED on/off every second to show aliveness
        hw.SetLed(true);
        HAL_Delay(500);
        hw.SetLed(false);
        HAL_Delay(500);

        hw.DelayMs(1); // Optional: limit control polling rate
    }
}
