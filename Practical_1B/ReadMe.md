# EEE3096S 2026 - Practical 1B

Execution timing and cycle-accurate Assembly on the UCT Dev Board.

Board: UCT Dev Board, STM32F051C8, Cortex-M0, 8 MHz HSI. One CPU cycle is 125 ns.

---

## Repository layout

```
Practical_1B/
├── task1/                    Task 1. Golden measure on the PC.
│   ├── Makefile
│   └── golden_measure.c
├── Practical1B/              Tasks 2 and 3. C on the board.
│   └── Core/Src/main.c       <- your work goes here
└── Practical1B-Assembly/     Tasks 4 and 5. Assembly on the board.
    └── Core/Src/
        ├── main.c            <- peripheral setup and task switch
        ├── dsp.s             <- Task 4
        └── lcd.s             <- Task 5
```

Every file you edit is marked. Search for `TODO` inside it. Each TODO is one piece of work.

Leave the `USER CODE BEGIN` and `USER CODE END` markers in place. STM32CubeIDE deletes everything outside them whenever you regenerate from the `.ioc` file.

---

## Before you start

Install STM32CubeIDE v1.19. Connect the board over USB. The onboard ST-Link V2.1 handles flashing and debugging, so no external programmer is needed.

Fill in your names and student numbers at the top of every file you submit.

---

## task1 folder, Task 1

Runs on your PC, not on the board.

```bash
cd task1
make
./golden_measure
```

Produce the ten golden outputs and record the per-call time at two different repetition counts. You need those ten outputs for Task 2, so keep the console output.

---

## Practical1B folder, Tasks 2 and 3

### Opening the project

File > Open Projects from File System, then point at `Practical1B`.

### Where to write code

All of it goes in `Core/Src/main.c`. The skeleton gives you:

- The ten Task 1 inputs, already filled in.
- An empty `golden_outputs` array. Paste your own Task 1 results into it.
- Stubs for `gpio_init`, `timing_timer_init`, `square_le`, `isqrt`, `time_one_call` and `time_n_calls`.
- A self-test loop and a measurement loop, both wired up and waiting for your functions.

Work through the TODOs in order. TODO 1 through 9 get the algorithm running. TODO 10 through 17 build the timing harness.

### Watching your results

Start a debug session with F11. Open Window > Show View > Live Expressions and add:

```
pass_all
single_call_span
long_run_span
mean_us_per_call
```

`pass_all` reads 1 once all ten values match. User LED 1 on PB1 shows the same thing.

### Oscilloscope setup for Task 2

- Probe on PC13, Header P1 pin 2. Ground clip on any board GND pin.
- Probe attenuation 10X. Set the scope channel to match.
- Trigger: Normal, Falling edge, level near 1.5 V.
- The pulse is active low. It drops from 3.3 V to 0 V for the duration of the call.
- Place both cursors on the falling and rising edges. The delta readout has to appear on screen in your screenshot.

If the trace jumps between two widths, the long run measurement is still enabled. Comment out TODO 17 while you capture the single call pulse.

### Task 3, the four builds

Change the level here:

Project > Properties > C/C++ Build > Settings > MCU GCC Compiler > Optimization > Optimization Level

| Setting | Flag |
| --- | --- |
| None | -O0 |
| Optimize | -O1 |
| Optimize more | -O2 |
| Optimize for size | -Os |

Rebuild with Ctrl+B after each change. At every level record the text size, the counter span and the scope pulse width.

Text size from the terminal:

```bash
arm-none-eabi-size Debug/Practical1B.elf
```

Disassembly of your own function at -O2:

```bash
arm-none-eabi-objdump -d Debug/Practical1B.elf > disasm_O2.txt
```

The build also writes `Debug/Practical1B.list`, which holds the same information.

Find one transformation the compiler applied to your square root code. Name it. Point at the source lines it acts on.

If your timing variables vanish from Live Expressions at -O2, they lost their `volatile` qualifier. Put it back.

---

## Practical1B-Assembly folder, Tasks 4 and 5

One project, two tasks. Pick the task with a single line in `Core/Src/main.c`:

```c
#define ACTIVE_TASK   4     /* 4 for the phase delay, 5 for the LCD */
```

Rebuild and flash after changing it.

### Adding lcd.s to the project

`dsp.s` already builds. Add the second file once:

1. Right-click `Core/Src` > New > File. Name it `lcd.s`. Lowercase extension.
2. Paste in the skeleton.
3. Right-click the project > Refresh, then Clean and rebuild.

Check the console for `dsp.o` and `lcd.o`. A missing `.o` means the file sits outside the source folders the build scans.

### Task 4, the phase delay

Peripheral settings in the `.ioc`, needed before anything works:

| Setting | Value | What breaks without it |
| --- | --- | --- |
| ADC IN8 | Enabled | No conversion on PB0 |
| Continuous Conversion Mode | Enabled | The ADC converts once and stops. Flat DAC output. |
| Overrun | Overrun data overwritten | The ADC halts as soon as your loop reads slower than it converts |
| DAC OUT1 | Enabled | Nothing on PA4 |

Bench setup:

1. Remove the POT0 jumper from the 2x2 header. The potentiometer otherwise divides down the signal generator output.
2. Signal generator positive lead to PB0. Ground lead to a board GND pin.
3. Sine, 1 kHz, 3.0 Vpp, DC offset 1.5 V. Set the offset before connecting. The ADC input is unipolar and a wave swinging below 0 V forward-biases the input protection.
4. Scope CH1 on PB0. Scope CH2 on PA4. Same volts per division on both, baselines aligned.

Note on PB0: the pin also drives user LED D1 through a 150 ohm resistor. The LED loads the generator and clips the top of the input wave, so CH1 reads less than 3.0 Vpp. Report the amplitude you actually measure.

Your work sits in `dsp.s`. Fill in the target derivation first, at the top of the file. Every instruction choice follows from that number.

Pass criteria: cursor delta near 125 us between CH1 and CH2, and a stepped DAC waveform with a step count matching your loop period.

### Task 5, the LCD

Bench setup:

1. Scope CH1 on PC15, the 3.3 V side from the STM32.
2. Scope CH2 on PC15_S, the 5 V side of the level shifter, near the LCD header.
3. Both channels 1 V/div or 2 V/div, shared ground reference.
4. Timebase near 500 ns/div. Trigger on CH1, rising edge.

Your work sits in `lcd.s`. Build the driver with no delay in `LCD_Pulse` first. The screen stays blank. Capture what CH2 does. That failing capture is a marked deliverable, so take it before you fix anything.

Then measure the rise from 0 V to 3.5 V, calculate the capacitance, size the pad, and capture the fixed case.

Leave the GPIO output speed on High in the `.ioc`. Dropping it to Low masks the fault.

---

## Building and flashing from the terminal

```bash
cd Practical1B/Debug
make -j4
```

Flash from the IDE with the Run button, or with STM32_Programmer_CLI if you have it installed.

---

## Common problems

| Symptom | Cause |
| --- | --- |
| Nothing happens on any GPIO pin | GPIO clock enabled on the wrong bus. GPIO ports sit on AHB on this device, not APB2. |
| Timer count is exactly double or half the scope reading | Prescaler value. The division factor is PSC + 1. |
| Timing variables disappear at -O2 | Missing `volatile`, or the return value is never consumed. |
| Flat line on PA4 | ADC continuous mode off, or overrun set to data preserved. |
| DAC output is a clean sine with no steps | Your delay loop is too short to be visible. Check the cycle budget. |
| Blank LCD, clean pulse on PC15 | The 5 V side never reaches 3.5 V. That is Task 5, working as intended. |
| `undefined reference to DSP_Loop` | The `.s` file is not in the build. Refresh the project and rebuild. |
| Infinite loop in your Assembly delay | `SUB` instead of `SUBS`. The Cortex-M0 needs the flag setting form. |

---

## Submission

- Push this repository to the same GitHub repo you used for Practical 1A.
- Submit the report PDF to Gradescope before the demonstration session.
- Filename: `EEE3096S_Prac1B_STUDENTNUMBER1_STUDENTNUMBER2.pdf`
- Code goes in the report appendix as selectable text. No screenshots of code.
- No video files and no links to video. Live behaviour is assessed at the bench in the White Lab.

Bring your board, your build environment and your report to the demonstration. Both partners answer questions on any line of your own code.