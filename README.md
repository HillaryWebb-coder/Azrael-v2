# Azrael v2

> A bare-metal handheld game console built from scratch on the STM32F411CEU6 — custom PCB, no HAL, direct register access throughout.

---

## Overview

Azrael v2 is a self-designed embedded systems project: a handheld gaming console built entirely from first principles. Every driver — SPI, I2C, GPIO, RTC, DMA — is written directly against the STM32F411 reference manual with no HAL abstraction layer. The hardware is a custom PCB manufactured via JLCPCB.

The project exists to demonstrate real embedded engineering: not glued-together library calls, but deliberate, register-level control of the hardware.

---

## Hardware

| Component | Part | Interface | Notes |
|---|---|---|---|
| MCU | STM32F411CEU6 (Cortex-M4, 100MHz) | — | Black pill form factor |
| Display | ST7735 1.8" TFT LCD (128×160) | SPI1 + DMA | DMA-driven framebuffer blits |
| Input | Wii Nunchuck (joystick + buttons) | I2C1 | Decoded in dedicated task |
| Temp/Humidity | AHT10 / SHT40 | I2C1 | Shared bus with Nunchuck |
| RTC | STM32F411 internal RTC | — | LSE crystal, VBAT-backed |
| PCB | Custom 2-layer | — | Designed in KiCad, manufactured by JLCPCB |

---

## Firmware Architecture

The firmware runs on FreeRTOS with a task-based architecture. All inter-task communication uses queues, mutexes, and event groups — no shared globals accessed without synchronisation.

### Task Structure

```
┌──────────────────────┐      writes      ┌─────────────────────────┐
│    vNunchuckTask     │ ───────────────▶ │   g_nun_data (global)   │
│                      │                  │   protected by mutex     │
│  - Polls Nunchuck    │                  └─────────────────────────┘
│    over I2C          │                               │
│  - Decodes joystick  │                    xSemaphoreTake/Give      │
│    + button state    │                               │
│  - Sets event bits   │──────────┐                    ▼
│    on button press   │          │       ┌─────────────────────────┐
└──────────────────────┘          │       │    Screen Tasks read    │
                                  │       │    g_nun_data under     │
                                  │       │    mutex protection     │
                    sets bits     │       └─────────────────────────┘
                                  ▼
                       ┌─────────────────────────────────────────┐
                       │            xSystemFlags                  │
                       │           (EventGroupHandle_t)           │
                       │                                          │
                       │  GAME_SCREEN        SET_TIME_SCREEN      │
                       │  SETTINGS_SCREEN    SET_DATE_SCREEN      │
                       │  OK_PRESSED         EXIT_PRESSED         │
                       │  C_PRESSED          Z_PRESSED            │
                       └─────────────────────────────────────────┘
                            │              │              │
                   waits on │     waits on │     waits on │
                            ▼              ▼              ▼
               ┌──────────────┐  ┌───────────────┐  ┌──────────────────┐
               │  vGameTask   │  │ vSettingsTask │  │ vSetDateTimeTask │
               │              │  │               │  │                  │
               │ Game logic,  │  │ Menu nav,     │  │ Joystick-driven  │
               │ sprite draw, │  │ screen routing│  │ field-by-field   │
               │ sensor read  │  │               │  │ RTC entry        │
               └──────┬───────┘  └──────┬────────┘  └────────┬─────────┘
                      │                 │                     │
                      └─────────────────┴─────────────────────┘
                                        │
                              xQueueSend(xDisplayQueue, ...)
                                        │
                                        ▼
                          ┌─────────────────────────┐
                          │      xDisplayQueue       │
                          │   (QueueHandle_t)        │
                          │                          │
                          │  carries displayData_t   │
                          │  union-tagged payloads   │
                          └─────────────────────────┘
                                        │
                                        ▼
                          ┌─────────────────────────┐
                          │      vDisplayTask        │
                          │                          │
                          │  Dispatches on           │
                          │  data_type field:        │
                          │                          │
                          │  RECT   → fill rect      │
                          │  TEXT   → render string  │
                          │  IMAGE  → blit sprite    │
                          │  CIRCLE → draw circle    │
                          └─────────────────────────┘
                                        │
                                        ▼
                          ┌─────────────────────────┐
                          │   ST7735 over SPI1+DMA   │
                          └─────────────────────────┘
```

### Display Queue

The display subsystem uses a **union-based queue** — a single `displayData_t` type carries tagged payloads across all four draw primitives. Only the display task ever writes to the hardware, enforcing a clean single-writer architecture.

```c
typedef enum { RECT, TEXT, IMAGE, CIRCLE } displayDataType_t;

typedef struct {
    displayDataType_t data_type;
    int16_t x, y;
    union {
        struct { uint16_t color; uint8_t w; uint8_t h; }        rect;
        struct { uint16_t color; char str[30]; FontDef *font;
                 uint16_t bgColor; }                             text;
        struct { uint8_t w; uint8_t h; const uint16_t *image; } image;
        struct { uint8_t r; uint16_t color; }                   circle;
    };
} displayData_t;
```

### Screen Navigation

Navigation between screens is managed entirely through event group bits in `xSystemFlags`. Each screen task blocks on its own bit with `xEventGroupWaitBits`. When the user navigates away, the current task clears its own bit and sets the next screen's bit — no task ever directly calls another. This keeps all screen logic fully decoupled.

```
SETTINGS_SCREEN bit set
        │
        ▼
vSettingsTask unblocks → user selects "Set Time"
        │
        ▼
Clear SETTINGS_SCREEN, Set SET_TIME_SCREEN
        │
        ▼
vSetDateTimeTask unblocks → joystick entry
        │
        ▼
On EXIT: Clear SET_TIME_SCREEN, Set SETTINGS_SCREEN
```

Current screens: Game, Settings, Set Time, Set Date.

---

## Drivers

All drivers are written directly against the STM32F411 reference manual. No HAL, no LL library. CMSIS device headers are used only for register address definitions.

### SPI + DMA

SPI1 drives the ST7735 display. DMA transfers handle framebuffer writes to avoid blocking the CPU during pixel data transmission. Key implementation details:

- `BIDIMODE` bit correctly cleared for full-duplex data line operation
- DMA stream configured with correct `CHSEL`, `MINC`, `DIR`, `TCIE` bits
- Memory-to-peripheral transfer with `PSIZE`/`MSIZE` matching SPI data register width
- CS and DC lines toggled manually via GPIO for command/data distinction

### I2C

I2C1 is shared between the Wii Nunchuck and AHT10/SHT40 sensor. The STM32F4 I2C peripheral has several non-obvious timing requirements implemented correctly here:

- `ADDR` flag cleared by reading `SR1` then `SR2` in sequence — reading `SR1` alone does not clear it
- `ACK` bit set before clearing `ADDR` for multi-byte reads
- `STOP` condition generated correctly without corrupting the bus state
- Nunchuck requires a specific initialisation handshake sequence before data reads are valid

### RTC

Internal RTC driven by 32.768kHz LSE crystal:

- Write protection disabled via two-key sequence (`0xCA` then `0x53` to `WPR`)
- Init mode entered by setting `INIT` bit, polling `INITF` before writing
- All time/date values BCD-encoded for `TR`/`DR` registers
- Weekday field populated using Sakamoto's algorithm — not BCD, raw 1–7
- `RTC_ISR_INITS` checked on every boot — time is only written if the backup domain has not been previously initialised, preserving time across warm resets
- VBAT pin connected to coin cell for persistence through full power loss

### GPIO

All GPIO configuration via direct register writes: `MODER` (mode), `OSPEEDR` (speed), `PUPDR` (pull), `AFR` (alternate function). No `HAL_GPIO_Init` structs.

---

## RTC Date/Time Entry

The Set Time and Set Date screens allow the user to configure the RTC directly from the device using the Nunchuck joystick. Entry is field-by-field:

- Joystick Y-axis increments or decrements the current field
- OK press advances to the next field
- Values are clamped (hours 0–23, minutes/seconds 0–59, etc.)
- On final field confirmation, values are written to the RTC via `RTC_SetTime()` / `RTC_SetDate()`
- Weekday is calculated automatically — user does not set it manually

---

## Build

| | |
|---|---|
| Toolchain | `arm-none-eabi-gcc` |
| IDE | STM32CubeIDE (project files included) |
| RTOS | FreeRTOS 10.x |
| HAL | None |
| CMSIS | Device headers only (register definitions) |
| Flash | ST-LINK v2 / OpenOCD |

---

## Project Structure

```
Azrael-v2/
├── Src/                    # Application — tasks, drivers, game logic
│   ├── main.c
│   ├── display.c           # Display task + ST7735 driver
│   ├── nunchuck.c          # I2C Nunchuck driver + task
│   ├── rtc.c               # RTC init, set, get
│   ├── spi.c               # SPI1 + DMA driver
│   ├── i2c.c               # I2C1 driver
│   ├── gpio.c              # GPIO configuration
│   └── tasks/              # Screen tasks
├── Inc/                    # Headers, structs, driver APIs, FreeRTOS config
├── Drivers/                # CMSIS STM32F411 device headers
├── Startup/                # Startup assembly + vector table
├── STM32F411CEUX_FLASH.ld  # Linker script (flash execution)
├── STM32F411CEUX_RAM.ld    # Linker script (RAM execution)
└── README.md
```

---

## Problems Encountered

This section documents the real debugging process. These are the problems that don't appear in tutorials.

### SPI — BIDIMODE Snow Screen

**Problem:** The ST7735 displayed random noise (snow) on power-on instead of initialising cleanly. SPI transfers appeared to complete without error.

**Cause:** `BIDIMODE` was inadvertently set in `SPI_CR1`, switching the data line to bidirectional mode. In this mode the peripheral shares MOSI/MISO on a single line and the output enable behaves differently — the display received malformed data during the init sequence.

**Fix:** Explicitly clear `BIDIMODE` in `SPI_CR1` during SPI initialisation. The init sequence timing also required `vTaskDelay` gaps between reset pulse, power-on, and the first command — the ST7735 datasheet minimum timings are not optional.

---

### SPI — DMA Address Bug

**Problem:** DMA transfers to the display would corrupt data or transfer incorrect bytes. The first few pixels were correct, then garbage.

**Cause:** The DMA source address was being passed as a stack pointer to a local buffer that went out of scope before the transfer completed. DMA is asynchronous — the CPU continues executing while the transfer runs, and the local buffer was overwritten.

**Fix:** DMA source buffers must be either static, global, or heap-allocated and kept alive for the full duration of the transfer. Added a transfer-complete flag polled before allowing the next queued draw operation.

---

### I2C — ADDR Flag Never Clearing

**Problem:** The I2C peripheral would hang after sending a slave address. The `ADDR` flag in `SR1` would set but never clear, causing the state machine to stall indefinitely.

**Cause:** The `ADDR` flag is cleared by a specific two-register read sequence: `SR1` must be read, then `SR2`. Reading only `SR1` is not sufficient. This is documented in the STM32F4 reference manual but easy to miss — it is not how most people intuitively expect a flag-clear to work.

**Fix:** Always read both `SR1` and `SR2` in sequence after address transmission, even if the `SR2` value is not needed. For multi-byte reads, the `ACK` bit must also be set before the `SR2` read to ensure the peripheral acknowledges the incoming bytes correctly.

---

### FreeRTOS — Hard Fault on First Context Switch

**Problem:** The firmware hard-faulted immediately on the first FreeRTOS context switch. The scheduler started, then crashed before any task code ran.

**Cause:** `configKERNEL_INTERRUPT_PRIORITY` and `configMAX_SYSCALL_INTERRUPT_PRIORITY` were not set correctly for the Cortex-M4 NVIC. FreeRTOS requires SVC, PendSV, and SysTick to be assigned specific priorities, and any interrupt that calls FreeRTOS API functions must have a priority numerically greater than (lower urgency than) `configMAX_SYSCALL_INTERRUPT_PRIORITY`. Mismatched priorities cause the NVIC to fire at the wrong level during a context switch.

**Fix:** Set `configKERNEL_INTERRUPT_PRIORITY` to the lowest priority (`0xFF` on Cortex-M4 with 4-bit priority). Set `configMAX_SYSCALL_INTERRUPT_PRIORITY` to a mid-level value. Verify all peripheral interrupt priorities are set above (numerically greater than) the syscall priority. Ensure `SVC_Handler`, `PendSV_Handler`, and `SysTick_Handler` macro names match the STM32F411 vector table exactly.

---

### FreeRTOS — Stack Overflow in Display Task

**Problem:** The display task would silently corrupt memory after running for several seconds, producing garbled output or a hard fault.

**Cause:** The display task stack was undersized for the union-based `displayData_t` struct being passed on the stack inside the task. The `str[30]` field inside the text union, combined with FreeRTOS internal overhead and the font pointer dereferences, exceeded the allocated stack.

**Fix:** Increased the display task stack size. Enabled `configCHECK_FOR_STACK_OVERFLOW` during development — FreeRTOS calls a hook function when a stack overflow is detected, which makes the problem visible immediately rather than manifesting as silent corruption elsewhere.

---

### RTC — Time Reset on Every Boot

**Problem:** The RTC time was reset to 00:00:00 on every power cycle even though the LSE crystal was running.

**Cause:** The firmware was unconditionally writing to `RTC_TR` and `RTC_DR` on every boot, overwriting whatever the RTC had preserved in the backup domain.

**Fix:** Check `RTC_ISR_INITS` before writing. This bit is set by hardware once the RTC has been initialised and the backup domain has valid data. If it is set, skip the write entirely. Time now persists across warm resets. Full power-loss persistence requires VBAT to be connected to a coin cell — if VBAT is floating or tied to VDD, the backup domain loses power when the main supply is removed.

---

### Nunchuck — Garbage Data on First Read

**Problem:** The first read from the Nunchuck after power-on returned random values. Subsequent reads were correct.

**Cause:** The Nunchuck requires a specific I2C initialisation handshake before it enters normal reporting mode. The standard sequence writes `0x55` to register `0xF0` and `0x00` to register `0xFB`. Without this, the device transmits encrypted data by default (a legacy anti-clone measure from Nintendo).

**Fix:** Implement the two-register handshake sequence in the Nunchuck init function and call it once before the polling loop begins. Add a short delay after initialisation before the first read.

---

## Why No HAL

HAL trades transparency for convenience. On a project this size, that trade is wrong — every abstraction is a layer that obscures what the hardware is actually doing. Writing directly to registers means every peripheral behaviour is understood, debuggable, and intentional.

More importantly: the problems listed above are the education. Every one of them required reading the reference manual, understanding the actual hardware behaviour, and writing a deliberate fix. HAL would have hidden most of them behind opaque error codes or silent workarounds, and the understanding would never have been built.

The goal was never just a working console. The goal was to understand every layer of it.

---

## Status

Active development. v2 hardware is functional. v3 planning underway — proposed additions include WiFi via lwIP stack, audio output, and expanded I/O.

---

## Author

Hillary Webb — self-taught embedded systems engineer, Manchester UK.
Building toward embedded freelance contracting and R&D.

[@hillarywbb](https://instagram.com/hillarywbb)