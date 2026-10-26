# Stopwatch & Countdown Timer (ATmega32)

A stopwatch and countdown timer built from scratch on an **ATmega32** in bare-metal C, using register-level programming with no libraries. 


## Features

- **Count-up mode:** runs from `00:00:00`, ticking once per second
- **Count-down mode:** set any time up to `99:59:59`
- **Buzzer alert** when the countdown reaches zero
- **Two LEDs** indicate the active mode
- **Reset / Pause / Resume** controls via external interrupts
- **Time adjustment** with dedicated buttons for hours, minutes, and seconds

## How It Works

| Component | Implementation |
|---|---|
| 1-second tick | Timer1 in CTC mode, compare-match interrupt |
| Display | 6 multiplexed 7-segment displays driven through a single 7447 BCD decoder |
| Reset | INT0 |
| Pause | INT1 |
| Resume | INT2 |
| Time setting | 6 buttons (hours, minutes, seconds, up/down) |
| Mode switch | 1 button toggles count-up / count-down |

## Hardware

- ATmega32 microcontroller
- 6 x common-anode 7-segment displays (multiplexed)
- 7447 BCD to 7-segment decoder
- 7 push buttons (6 for time adjustment, 1 for mode)
- 3 push buttons for reset, pause, and resume (INT0 / INT1 / INT2)
- Buzzer and 2 mode LEDs


## Project Structure

```
.
├── src/            # C source files
├── proteus/        # Proteus simulation project
├── docs/           # Schematic, demo GIF
└── README.md
```

## Build & Run

1. Open the `.pdsprj` file in `proteus/` using Proteus.
2. Load the compiled `.hex` file into the ATmega32.
3. Run the simulation.



## Challenges

The hardest part was getting display multiplexing and the interrupts to work together without flicker. The fix was keeping the interrupt service routines short, and doing the refresh timing carefully so the ISRs never block the display loop.


## What I Learned

- Register-level configuration of timers and external interrupts from the datasheet
- Time-multiplexing displays while serving interrupts
- Debugging embedded firmware in simulation

## Author

**Mohammed Yasser Abdelsalam**
[LinkedIn](https://www.linkedin.com/in/mohammedyasserr/)

`#EmbeddedSystems` `#AVR` `#ATmega32` `#Microcontrollers` `#ElectricalEngineering` `#Proteus`
