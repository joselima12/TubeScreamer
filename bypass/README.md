main.cpp
│
├── hw.Init()
├── hw.StartAudio()
└── Audio Callback Loop
     │
     └─► TubeScreamer::processSample(in_sample)
             ▲
             │
             └── TubeScreamer Class (TubeScreamer.h/.cpp)
                  │
                  ├── prepare(sampleRate)
                  └── processSample(input)
                          │
                          ▼
              ┌─────────────────────────────┐
              │     TubeScreamer Class      │
              └─────────────────────────────┘
                          │
          ┌───────────────┼────────────────┐
          │                                │
          ▼                                ▼
       Upsample                        process through
     input signal                     TSClipper (diode + op-amp)
     (2x or more)                     (TSClipper.h/.cpp)
          │                                │
          ▼                                ▼
       FIR LPF                        Nonlinear Clip
     Interpolation                         │
     (if used)                             │
          │                                │
          └────────────┬───────────────────┘
                       ▼
             ▼ Tone shaping filter
               RCFilter.h/.cpp
             ▼ (passive EQ-like LPF)
                       │
                       ▼
              ▼ Downsample output
                (anti-alias LPF optional)
                       │
                       ▼
               return final sample
