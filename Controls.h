#pragma once
/**
 * Controls.h - Daisy Seed controls helper
 *
 * Features:
 *  - Smoothed 0..1 pot reads via libDaisy::AnalogControl
 *  - Debounced SPST (toggle) and momentary footswitch handling via libDaisy::Switch
 *  - Edge detection helpers for momentary actions
 *
 * Usage:
 *   - Compute a control update rate: ctrl_hz = hw.AudioSampleRate() / hw.AudioBlockSize();
 *   - Call Controls::Init(...) once, then Controls::Update() once per audio block.
 *   - Read pots with Pot(i), toggles with TogglePressed(i), footswitch edges with FootRising(i), etc.
 *
 * Wiring (typical):
 *   - Pots: ends to 3V3_A and AGND; wiper to Ai (A0..A11).
 *   - Switches/footswitches: one side to GND, the other to a GPIO; use internal PULLUP (default).
 */

#include <array>
#include <cmath>
#include <cstdint>
#include "daisy_seed.h"
#include "hid/ctrl.h"    // AnalogControl
#include "hid/switch.h"  // Switch

namespace controls {

using namespace daisy;
using namespace daisy::seed;

template <size_t NPots, size_t NToggles, size_t NFoots, size_t NLeds>
class Controls
{
  public:
    Controls() = default;

    /** Initialize all controls.
     *
     * @param hw                 DaisySeed instance (already Init()'d)
     * @param pot_pins           array of ADC-capable pins for pots (A0..A11)
     * @param toggle_pins        array of GPIO pins for SPST latching switches
     * @param foot_pins          array of GPIO pins for momentary footswitches
     * @param leds_pins          array of GPIO pins for LEDs
     * @param effect_on          initial state of effect (true=on), default false
     * @param ctrl_update_hz     rate in Hz that Update() will be called (usually AudioSampleRate/BlockSize)
     * @param pot_flip           invert 0..1 reading (hardware orientation), default false
     * @param pot_invert         multiply by -1 (rarely needed), default false
     * @param pot_slew_seconds   smoothing time constant (default 2 ms)
     */
    void Init(DaisySeed& hw,
              const Pin (&pot_pins)[NPots],
              const Pin (&toggle_pins)[NToggles],
              const Pin (&foot_pins)[NFoots],
              const Pin (&leds_pins)[NLeds],
              float      ctrl_update_hz,
              bool       pot_flip         = false,
              bool       pot_invert       = false,
              bool       effect_on        = false,
              float      pot_slew_seconds = 0.002f)
    {
        hw_            = &hw;
        ctrl_update_hz_ = ctrl_update_hz;
        effect_on_     = effect_on;

        // --- ADC + Pots ---
        if (NPots > 0)
        {
            for(size_t i = 0; i < NPots; ++i)
                adc_cfg_[i].InitSingle(pot_pins[i]);

            hw_->adc.Init(adc_cfg_.data(), NPots);
            hw_->adc.Start();

            for(size_t i = 0; i < NPots; ++i)
            {
                pots_[i].Init(hw_->adc.GetPtr(i), ctrl_update_hz_, pot_flip, pot_invert, pot_slew_seconds);
                pot_values_[i] = 0.f;
            }
        }

        // --- SPST Toggles (latching) ---
        if (NToggles > 0)
        {
            for(size_t i = 0; i < NToggles; ++i)
            {
                toggles_[i].Init(toggle_pins[i]); // defaults: momentary, inverted, pull-up
            }
        }

        // --- Momentary Footswitches ---
        if (NFoots > 0)
        {
            for(size_t i = 0; i < NFoots; ++i)
            {
                foots_[i].Init(foot_pins[i]);     // defaults: momentary, inverted, pull-up
            }
        }

        // --- LEDs ---
        if (NLeds > 0)
        {
            for(size_t i = 0; i < NLeds; ++i)
            {
                leds_[i].Init(leds_pins[i],
                        GPIO::Mode::OUTPUT,
                        GPIO::Pull::NOPULL,
                        GPIO::Speed::LOW);
                leds_[i].Write(false); // start OFF
            }
        }
    }

    /** Call once per audio block (or at ctrl_update_hz). */
    void Update()
    {
        if (NPots > 0)
        {
            for(size_t i = 0; i < NPots; ++i)
                pot_values_[i] = pots_[i].Process(); // smoothed, normalized 0..1
        }

        if (NToggles > 0)
        {
            for(size_t i = 0; i < NToggles; ++i)
                toggles_[i].Debounce();
        }

        if (NFoots > 0)
        {
            for(size_t i = 0; i < NFoots; ++i)
                foots_[i].Debounce();
        }
    }

    // ---------- Pots ----------
    float Pot(size_t i) const
    {
        return (i < NPots) ? pot_values_[i] : 0.f;
    }

    /** Map pot i to [min_v, max_v] with a logarithmic (exponential) response.
    Requires min_v > 0 and max_v > 0. Falls back to linear if not valid. */
    float PotMapped(size_t i, float min_v, float max_v) const
    {
        float x = Pot(i);
        if(x < 0.f) x = 0.f; else if(x > 1.f) x = 1.f;

        if(min_v > 0.f && max_v > 0.f)
        {
            // exponential interpolation: y = lo * (hi/lo)^x
            const float lo = (min_v < max_v) ? min_v : max_v;
            const float hi = (min_v < max_v) ? max_v : min_v;
            const float y  = lo * std::pow(hi / lo, x);

            // keep original direction (handles min_v > max_v)
            return (min_v <= max_v) ? y : (min_v + max_v - y);
        }

        // Fallback to linear if bounds aren’t both positive
        return min_v + (max_v - min_v) * x;
    }

    /** Access raw 16-bit ADC (0..65535). */
    uint16_t PotRaw(size_t i) const
    {
        return (i < NPots) ? pots_[i].GetRawValue() : 0;
    }

    // ---------- SPST Toggles (latching) ----------
    bool TogglePressed(size_t i) const
    {
        return (i < NToggles) ? toggles_[i].Pressed() : false;
    }
    bool ToggleRising(size_t i) const
    {
        return (i < NToggles) ? toggles_[i].RisingEdge() : false;
    }
    bool ToggleFalling(size_t i) const
    {
        return (i < NToggles) ? toggles_[i].FallingEdge() : false;
    }
    float ToggleHeldMs(size_t i) const
    {
        return (i < NToggles) ? toggles_[i].TimeHeldMs() : 0.f;
    }

    // ---------- Momentary Footswitches ----------
    bool FootPressed(size_t i) const
    {
        return (i < NFoots) ? foots_[i].Pressed() : false;
    }
    bool FootRising(size_t i) const
    {
        return (i < NFoots) ? foots_[i].RisingEdge() : false;
    }
    bool FootFalling(size_t i) const
    {
        return (i < NFoots) ? foots_[i].FallingEdge() : false;
    }
    float FootHeldMs(size_t i) const
    {
        return (i < NFoots) ? foots_[i].TimeHeldMs() : 0.f;
    }

    // ------------------- LEDs --------------------

    /** Set LED i ON/OFF. */
    void LedWrite(size_t i, bool on)
    {
        if(i < NLeds)
            leds_[i].Write(on);
    }

    /** Toggle LED i and return the new state. */
    bool LedToggle(size_t i)
    {
        if(i < NLeds)
        {
            bool new_state = !leds_[i].Read();
            leds_[i].Write(new_state);
            return new_state;
        }
        return false;
    }

    /** Read current LED state. */
    bool LedState(size_t i) const
    {
        return (i < NLeds) ? leds_[i].Read() : false;
    }

    // --------------- Effect Toggle ----------------

    void EffectToggle()
    {
        effect_on_ = !effect_on_;
    }
    
    bool EffectOn() const
    {
        return effect_on_;
    }


    /** If your block size changes at runtime, call this to keep smoothing stable. */
    void SetControlUpdateRate(float hz)
    {
        ctrl_update_hz_ = hz;
        if (NPots > 0)
            for(size_t i = 0; i < NPots; ++i)
                pots_[i].SetSampleRate(ctrl_update_hz_);
        if (NToggles > 0)
            for(size_t i = 0; i < NToggles; ++i)
                toggles_[i].SetUpdateRate(ctrl_update_hz_); // kept for API compat
        if (NFoots > 0)
            for(size_t i = 0; i < NFoots; ++i)
                foots_[i].SetUpdateRate(ctrl_update_hz_);   // kept for API compat
    }

  private:
    DaisySeed* hw_            = nullptr;
    float      ctrl_update_hz_ = 1000.0f;
    bool       effect_on_     = false;

    // Pots
    std::array<AdcChannelConfig, NPots> adc_cfg_{};
    std::array<AnalogControl, NPots>    pots_{};
    std::array<float, NPots>            pot_values_{};

    // Switches
    std::array<Switch, NToggles> toggles_{};
    std::array<Switch, NFoots>   foots_{};

    // LEDs
    std::array<GPIO, NLeds> leds_{};
};

} // namespace controls
