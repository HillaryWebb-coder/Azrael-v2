/*
 * i2c_scan.c
 *
 *  Created on: 10 Jun 2026
 *      Author: hilla
 */


/*
 * i2c_scan.c
 *
 * Bare-metal I2C bus scanner for STM32F411
 * Uses I2C1: PB8 = SCL, PB9 = SDA (AF4)
 *
 * Results printed via SWO (ITM) or swap out
 * i2c_scan_report() for your UART/display output.
 */

#include "stm32f4xx.h"
#include "stm32f411xe.h"

/* ── Timing ─────────────────────────────────────────────────────────── */
/* Assumes APB1 = 50 MHz, standard-mode 100 kHz                         */
#define I2C_PCLK_MHZ     50u
#define I2C_CCR_VALUE    250u   /* PCLK / (2 * 100kHz) = 250            */
#define I2C_TRISE_VALUE  51u    /* (PCLK_MHz * 1000ns) + 1 = 51         */

/* ── Timeouts ───────────────────────────────────────────────────────── */
#define I2C_TIMEOUT      10000u

/* ── ITM printf via SWO ─────────────────────────────────────────────── */
static void itm_print(const char *s) {
    while (*s) {
        /* Wait for ITM port 0 to be ready */
        while (!(ITM->PORT[0].u32));
        ITM->PORT[0].u8 = (uint8_t)*s++;
    }
}

static void itm_print_hex(uint8_t val) {
    const char hex[] = "0123456789ABCDEF";
    char buf[5] = "0x";
    buf[2] = hex[val >> 4];
    buf[3] = hex[val & 0x0F];
    buf[4] = '\0';
    itm_print(buf);
}

/* ── I2C1 init ──────────────────────────────────────────────────────── */
void i2c1_scan_init(void) {
    /* Clocks */
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR  |= RCC_APB1ENR_I2C1EN;

    /* PB8, PB9 → AF4, open-drain, high-speed */
    GPIOB->MODER  &= ~(GPIO_MODER_MODE8  | GPIO_MODER_MODE9);
    GPIOB->MODER  |=  (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1); /* AF */
    GPIOB->OTYPER |=  (GPIO_OTYPER_OT8   | GPIO_OTYPER_OT9);     /* OD */
    GPIOB->OSPEEDR|=  (GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9);
    GPIOB->PUPDR  &= ~(GPIO_PUPDR_PUPD8  | GPIO_PUPDR_PUPD9);
    GPIOB->PUPDR  |=  (GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0); /* pull-up */

    /* AF4 = I2C1 on PB8/PB9 (high byte of AFR) */
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9);
    GPIOB->AFR[1] |=  ((4UL << 0) | (4UL << 4));   /* AF4 */

    /* Reset I2C1 peripheral */
    I2C1->CR1 |=  I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    /* Configure */
    I2C1->CR2   = I2C_PCLK_MHZ;            /* peripheral clock in MHz  */
    I2C1->CCR   = I2C_CCR_VALUE;           /* standard mode, 100 kHz   */
    I2C1->TRISE = I2C_TRISE_VALUE;

    I2C1->CR1  |= I2C_CR1_PE;              /* enable peripheral        */
}

/* ── Low-level helpers ──────────────────────────────────────────────── */

/* Returns 0 on success, 1 on timeout */
static uint8_t wait_flag(uint32_t reg_mask, uint32_t sr, uint8_t set) {
    uint32_t timeout = I2C_TIMEOUT;
    if (sr == 1) {
        /* SR1 */
        if (set) { while (!(I2C1->SR1 & reg_mask) && --timeout); }
        else      { while ( (I2C1->SR1 & reg_mask) && --timeout); }
    } else {
        /* SR2 */
        if (set) { while (!(I2C1->SR2 & reg_mask) && --timeout); }
        else      { while ( (I2C1->SR2 & reg_mask) && --timeout); }
    }
    return (timeout == 0) ? 1 : 0;
}

/*
 * Attempt to address a device at `addr` (7-bit).
 * Returns 1 if ACK received, 0 if NACK or error.
 */
static uint8_t i2c_probe(uint8_t addr) {
    uint32_t timeout;

    /* Wait until bus is free */
    timeout = I2C_TIMEOUT;
    while ((I2C1->SR2 & I2C_SR2_BUSY) && --timeout);
    if (!timeout) return 0;

    /* Generate START */
    I2C1->CR1 |= I2C_CR1_START;

    /* Wait for SB (start bit generated) */
    if (wait_flag(I2C_SR1_SB, 1, 1)) return 0;

    /* Send address + write bit — clears SB by reading SR1 then writing DR */
    (void)I2C1->SR1;
    I2C1->DR = (addr << 1) | 0x00;   /* write direction */

    /* Wait for ADDR or AF (ACK failure) */
    timeout = I2C_TIMEOUT;
    while (timeout--) {
        uint16_t sr1 = I2C1->SR1;
        if (sr1 & I2C_SR1_ADDR) {
            /* ACK received — clear ADDR by reading SR1 then SR2 */
            (void)I2C1->SR1;
            (void)I2C1->SR2;
            /* Generate STOP */
            I2C1->CR1 |= I2C_CR1_STOP;
            return 1;
        }
        if (sr1 & I2C_SR1_AF) {
            /* NACK — clear AF, generate STOP */
            I2C1->SR1 &= ~I2C_SR1_AF;
            I2C1->CR1 |= I2C_CR1_STOP;
            return 0;
        }
    }

    /* Timeout — force STOP and reset */
    I2C1->CR1 |= I2C_CR1_STOP;
    return 0;
}

/* ── Scanner ────────────────────────────────────────────────────────── */
void i2c_scan(void) {
    uint8_t found = 0;
    uint8_t addr_buf[3];

    itm_print("\r\n== I2C Bus Scan ==\r\n");
    itm_print("Scanning addresses 0x08 to 0x77...\r\n\r\n");

    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        if (i2c_probe(addr)) {
            itm_print("  ACK at ");
            itm_print_hex(addr);
            itm_print("\r\n");
            addr_buf[found] = addr;
            found++;
        }
    }

    if (found == 0) {
        itm_print("  No devices found.\r\n");
    } else {
        itm_print("\r\nTotal devices found: ");
        /* Simple single-digit print — extend if you expect >9 devices */
        char c = '0' + found;
        while (!(ITM->PORT[0].u32));
        ITM->PORT[0].u8 = c;
        itm_print("\r\n");
    }

    itm_print("== Scan complete ==\r\n");
}
