# Embedded Piscine (ATmega328P / AVR in C)

Goal: Learn embedded systems by programming the **ATmega328P** microcontroller in **C**, progressing from toolchain basics to peripherals and sensor interfacing.


# Module00 — Toolchain Basics / Minimal Project

- AVR build pipeline: `C → BIN → HEX → flash`
- Makefile essentials: targets, variables, compile/link flags
- Minimal embedded program structure: init + infinite loop
- Understanding build outputs (`.bin`, `.hex`) vs source (`.c`)

## Exercises & Knowledge Points
- **ex00**: Minimal buildable AVR C project, basic Makefile usage
- **ex01**: Slightly larger program structure (init/loop), basic C organization for embedded
- **ex02**: More control flow and clean structuring; preparing for register-level coding style
- **ex03**: Handling/understanding generated artifacts (`main.hex`, `main.bin`) and what gets flashed
- **ex04**: Consolidation: build + structure + simple embedded control logic



# Module01 — GPIO (Digital I/O) and Bitwise Register Control

- AVR GPIO registers: `DDRx` (direction), `PORTx` (output / pull-up), `PINx` (read)
- Bitwise operations for register manipulation: masks, set/clear/toggle bits
- Input pull-ups and basic button logic
- Debouncing basics (software delay / simple state logic)

## Exercises & Knowledge Points
- **ex00**: Configure pin as output; LED on/off and simple timing behavior
- **ex01**: Configure pin as input; read button state and control output accordingly
- **ex02**: Multi-pin control; mask-based operations for multiple bits
- **ex03**: More complex GPIO logic (state switching / mode logic), better structure for scaling



# Module02 — Timing Concepts and Timer-Oriented Thinking

- Time in MCUs: busy-wait delays vs hardware timers
- Core timer concepts: prescaler, counter, compare match, overflow
- Periodic tasks (blinking, sampling, scheduling) without scattered hard delays
- Basic “tick” / counter-driven structure

## Exercises & Knowledge Points
- **ex00**: Introduce periodic behavior with more controlled timing structure
- **ex01**: Multi-rate timing or improved time abstraction (counters instead of raw delays)
- **ex02**: Compare/overflow style logic or equivalent periodic scheduling patterns
- **ex03**: Integrate timing with program structure (preparing for PWM/serial timeouts)


# Module03 — Interrupts and Event-Driven Structure

- Interrupt model: sources, ISR, global enable
- External events handling (e.g., button interrupt concepts)
- Shared data with ISR: `volatile`, race awareness, keep ISR short
- Polling vs interrupts trade-offs: responsiveness, clarity, power

## Exercises & Knowledge Points
- **ex00**: Basic interrupt setup pattern (init + ISR + main loop coordination)
- **ex01**: Use interrupts for external events; state toggling/counters via ISR
- **ex02**: More complex interrupt-driven logic; safer sharing of state
- **ex03**: Combine interrupts with timing/state machines for robust behavior



# Module04 — Analog / ADC-Oriented Skills (verify details in Module04.pdf)

- ADC basics: reference voltage, resolution, channels, sampling considerations
- Converting analog signals to numeric values and using thresholds/mapping
- Simple filtering/averaging and noise awareness

## Exercises & Knowledge Points
- **ex00**: Peripheral initialization + single-channel sampling + basic decision logic
- **ex01**: Multi-sample/multi-channel or more advanced mapping/filtering and output behavior



# Module05 — Serial Communication / Debugging (verify details in Module05.pdf)

- UART fundamentals: baud rate, frame format (e.g., 8N1), TX/RX
- Serial as a debugging tool: logs, observability, validation
- Receive buffering: polling vs interrupt-based reception (ring buffer idea)
- Simple command parsing and safe input handling

## Exercises & Knowledge Points
- **ex00**: UART init + transmit; basic “print/debug output” workflow
- **ex01**: UART receive; echo or basic input processing
- **ex02**: Line/buffer handling; simple parsing patterns
- **ex03**: More complex command protocol and error handling
- **ex04**: Integrate serial commands with peripheral control; modular code structure



# Module06 — I2C + Sensor Driver (AHT20) and Integration

- I2C fundamentals: addressing, read/write sequences, ACK/NACK, bus timing
- Driver workflow from datasheet: init → trigger → wait → read → convert
- Robustness: retries, timeouts, error codes, clear driver APIs
- Datasheet reading skills (timing diagrams, conversion formulas)

## Exercises & Knowledge Points
- **ex00**: I2C master init + basic transactions; validation via produced HEX/BIN artifacts
- **ex01**: Communicate with AHT20: send commands, read raw measurement bytes, respect timing
- **ex02**: Convert raw data to temperature/humidity using AHT20 datasheet; add robustness
