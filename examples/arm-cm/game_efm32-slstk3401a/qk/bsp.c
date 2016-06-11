/*****************************************************************************
* Product: "Fly 'n' Shoot" game example, EFM32-SLSTK3401A board, QK kernel
* Last updated for version 5.6.5
* Last updated on  2016-06-02
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* http://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpn.h"
#include "game.h"
#include "bsp.h"

#include "em_device.h"  /* the device specific header (SiLabs) */
#include "em_chip.h"    /* Chip errata (SiLabs) */
#include "em_cmu.h"     /* Clock Management Unit (SiLabs) */
#include "em_gpio.h"    /* GPIO (SiLabs) */
#include "em_usart.h"   /* USART (SiLabs) */
#include "display_ls013b7dh03.h" /* LS013b7DH03 display (SiLabs/QL) */
/* add other drivers if necessary... */

Q_DEFINE_THIS_FILE

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
* DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
*/
enum KernelUnawareISRs { /* see NOTE00 */
    /* ... */
    MAX_KERNEL_UNAWARE_CMSIS_PRI  /* keep always last */
};
/* "kernel-unaware" interrupts can't overlap "kernel-aware" interrupts */
Q_ASSERT_COMPILE(MAX_KERNEL_UNAWARE_CMSIS_PRI <= QF_AWARE_ISR_CMSIS_PRI);

enum KernelAwareISRs {
    GPIO_EVEN_PRIO = QF_AWARE_ISR_CMSIS_PRI, /* see NOTE00 */
    SYSTICK_PRIO,
    /* ... */
    MAX_KERNEL_AWARE_CMSIS_PRI /* keep always last */
};
/* "kernel-aware" interrupts should not overlap the PendSV priority */
Q_ASSERT_COMPILE(MAX_KERNEL_AWARE_CMSIS_PRI <= (0xFF >>(8-__NVIC_PRIO_BITS)));

/* ISRs defined in this BSP ------------------------------------------------*/
void SysTick_Handler(void);
void GPIO_EVEN_IRQHandler(void);
void USART0_RX_IRQHandler(void);

/* Local-scope objects -----------------------------------------------------*/
#define LED_PORT    gpioPortF
#define LED0_PIN    4
#define LED1_PIN    5

#define PB_PORT     gpioPortF
#define PB0_PIN     6
#define PB1_PIN     7

/* LCD geometry and frame buffer */
static uint32_t l_fb[BSP_SCREEN_HEIGHT + 1][BSP_SCREEN_WIDTH / 32U];

/* the walls buffer */
static uint32_t l_walls[GAME_TUNNEL_HEIGHT + 1][BSP_SCREEN_WIDTH / 32U];

static unsigned l_rnd;  /* random seed */
static void paintBits(uint8_t x, uint8_t y, uint8_t const *bits, uint8_t h);
static void paintBitsClear(uint8_t x, uint8_t y,
                           uint8_t const *bits, uint8_t h);

/* ISRs used in the application ==========================================*/
void SysTick_Handler(void) {
    QK_ISR_ENTRY();  /* inform QK about entering an ISR */

    QF_tickXISR(0U); /* process time events for rate 0 */

    /* post TIME_TICK events to all interested active objects... */
    QACTIVE_POST_ISR((QMActive *)&AO_Tunnel,  TIME_TICK_SIG, 0);
    QACTIVE_POST_ISR((QMActive *)&AO_Ship,    TIME_TICK_SIG, 0);
    QACTIVE_POST_ISR((QMActive *)&AO_Missile, TIME_TICK_SIG, 0);

    {
        /* state of the button debouncing, see below */
        static struct ButtonsDebouncing {
            uint32_t depressed;
            uint32_t previous;
        } buttons = { ~0U, ~0U };
        uint32_t current;
        uint32_t tmp;

       /* Perform the debouncing of buttons. The algorithm for debouncing
       * adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
       * and Michael Barr, page 71.
       */
       current = ~GPIO->P[PB_PORT].DIN; /* read PB0 and BP1 */
       tmp = buttons.depressed; /* save the debounced depressed buttons */
       buttons.depressed |= (buttons.previous & current); /* set depressed */
       buttons.depressed &= (buttons.previous | current); /* clear released */
       buttons.previous   = current; /* update the history */
       tmp ^= buttons.depressed;     /* changed debounced depressed */
       if ((tmp & (1U << PB0_PIN)) != 0U) {  /* debounced PB0 state changed? */
           if ((buttons.depressed & (1U << PB0_PIN)) != 0U) {/*PB0 depressed?*/
               QACTIVE_POST_ISR((QMActive *)&AO_Ship,   PLAYER_TRIGGER_SIG, 0U);
               QACTIVE_POST_ISR((QMActive *)&AO_Tunnel, PLAYER_TRIGGER_SIG, 0U);
           }
       }
    }

    QK_ISR_EXIT();  /* inform QK about exiting an ISR */
}
/*..........................................................................*/
void GPIO_EVEN_IRQHandler(void) {
    QK_ISR_ENTRY(); /* inform QK about entering an ISR */
    QACTIVE_POST_ISR((QMActive *)&AO_Tunnel, TAKE_OFF_SIG, 0U);
    QK_ISR_EXIT();  /* inform QK about exiting an ISR */
}

/* BSP functions ===========================================================*/
void BSP_init(void) {
    /* Chip errata */
    CHIP_Init();

    /* NOTE: SystemInit() already called from startup_TM4C123GH6PM.s
    *  but SystemCoreClock needs to be updated
    */
    SystemCoreClockUpdate();

    /* configure the FPU usage by choosing one of the options... */
#if 1
    /* OPTION 1:
    * Use the automatic FPU state preservation and the FPU lazy stacking.
    *
    * NOTE:
    * Use the following setting when FPU is used in more than one task or
    * in any ISRs. This setting is the safest and recommended, but requires
    * extra stack space and CPU cycles.
    */
    FPU->FPCCR |= (1U << FPU_FPCCR_ASPEN_Pos) | (1U << FPU_FPCCR_LSPEN_Pos);
#else
    /* OPTION 2:
    * Do NOT to use the automatic FPU state preservation and
    * do NOT to use the FPU lazy stacking.
    *
    * NOTE:
    * Use the following setting when FPU is used in ONE task only and not
    * in any ISR. This setting is very efficient, but if more than one task
    * (or ISR) start using the FPU, this can lead to corruption of the
    * FPU registers. This option should be used with CAUTION.
    */
    FPU->FPCCR &= ~((1U << FPU_FPCCR_ASPEN_Pos) | (1U << FPU_FPCCR_LSPEN_Pos));
#endif

    /* enable clock for to the peripherals used by this application... */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO,  true);
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO,  true);

    /* configure the LEDs */
    GPIO_PinModeSet(LED_PORT, LED0_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(LED_PORT, LED1_PIN, gpioModePushPull, 0);
    GPIO_PinOutClear(LED_PORT, LED0_PIN);
    GPIO_PinOutClear(LED_PORT, LED1_PIN);

    /* configure the Buttons */
    GPIO_PinModeSet(PB_PORT, PB0_PIN, gpioModeInputPull, 1);
    GPIO_PinModeSet(PB_PORT, PB1_PIN, gpioModeInputPull, 1);

    /* Initialize the DISPLAY driver. */
    if (!Display_init()) {
        Q_ERROR();
    }
}

/*..........................................................................*/
void BSP_updateScreen(void) {
    GPIO->P[LED_PORT].DOUT |=  (1U << LED1_PIN);
    Display_sendPA(&l_fb[0][0], 0, LS013B7DH03_HEIGHT);
    GPIO->P[LED_PORT].DOUT &= ~(1U << LED1_PIN);
}
/*..........................................................................*/
void BSP_clearFB() {
    uint_fast8_t y;
    for (y = 0U; y < BSP_SCREEN_HEIGHT; ++y) {
        l_fb[y][0] = 0U;
        l_fb[y][1] = 0U;
        l_fb[y][2] = 0U;
        l_fb[y][3] = 0U;
    }
}
/*..........................................................................*/
void BSP_clearWalls() {
    uint_fast8_t y;
    for (y = 0U; y < GAME_TUNNEL_HEIGHT; ++y) {
        l_walls[y][0] = 0U;
        l_walls[y][1] = 0U;
        l_walls[y][2] = 0U;
        l_walls[y][3] = 0U;
    }
}
/*..........................................................................*/
bool BSP_isThrottle(void) { /* is the throttle button depressed? */
    return (GPIO->P[PB_PORT].DIN & (1U << PB1_PIN)) == 0U;
}
/*..........................................................................*/
void BSP_paintString(uint8_t x, uint8_t y, char const *str) {
    static uint8_t const font5x7[95][7] = {
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U }, /*   */
        { 0x04U, 0x04U, 0x04U, 0x04U, 0x00U, 0x00U, 0x04U }, /* ! */
        { 0x0AU, 0x0AU, 0x0AU, 0x00U, 0x00U, 0x00U, 0x00U }, /* " */
        { 0x0AU, 0x0AU, 0x1FU, 0x0AU, 0x1FU, 0x0AU, 0x0AU }, /* # */
        { 0x04U, 0x1EU, 0x05U, 0x0EU, 0x14U, 0x0FU, 0x04U }, /* $ */
        { 0x03U, 0x13U, 0x08U, 0x04U, 0x02U, 0x19U, 0x18U }, /* % */
        { 0x06U, 0x09U, 0x05U, 0x02U, 0x15U, 0x09U, 0x16U }, /* & */
        { 0x06U, 0x04U, 0x02U, 0x00U, 0x00U, 0x00U, 0x00U }, /* ' */
        { 0x08U, 0x04U, 0x02U, 0x02U, 0x02U, 0x04U, 0x08U }, /* ( */
        { 0x02U, 0x04U, 0x08U, 0x08U, 0x08U, 0x04U, 0x02U }, /* ) */
        { 0x00U, 0x04U, 0x15U, 0x0EU, 0x15U, 0x04U, 0x00U }, /* * */
        { 0x00U, 0x04U, 0x04U, 0x1FU, 0x04U, 0x04U, 0x00U }, /* + */
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x06U, 0x04U, 0x02U }, /* , */
        { 0x00U, 0x00U, 0x00U, 0x1FU, 0x00U, 0x00U, 0x00U }, /* - */
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x06U, 0x06U }, /* . */
        { 0x00U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U, 0x00U }, /* / */
        { 0x0EU, 0x11U, 0x19U, 0x15U, 0x13U, 0x11U, 0x0EU }, /* 0 */
        { 0x04U, 0x06U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU }, /* 1 */
        { 0x0EU, 0x11U, 0x10U, 0x08U, 0x04U, 0x02U, 0x1FU }, /* 2 */
        { 0x1FU, 0x08U, 0x04U, 0x08U, 0x10U, 0x11U, 0x0EU }, /* 3 */
        { 0x08U, 0x0CU, 0x0AU, 0x09U, 0x1FU, 0x08U, 0x08U }, /* 4 */
        { 0x1FU, 0x01U, 0x0FU, 0x10U, 0x10U, 0x11U, 0x0EU }, /* 5 */
        { 0x0CU, 0x02U, 0x01U, 0x0FU, 0x11U, 0x11U, 0x0EU }, /* 6 */
        { 0x1FU, 0x10U, 0x08U, 0x04U, 0x02U, 0x02U, 0x02U }, /* 7 */
        { 0x0EU, 0x11U, 0x11U, 0x0EU, 0x11U, 0x11U, 0x0EU }, /* 8 */
        { 0x0EU, 0x11U, 0x11U, 0x1EU, 0x10U, 0x08U, 0x06U }, /* 9 */
        { 0x00U, 0x06U, 0x06U, 0x00U, 0x06U, 0x06U, 0x00U }, /* : */
        { 0x00U, 0x06U, 0x06U, 0x00U, 0x06U, 0x04U, 0x02U }, /* ; */
        { 0x08U, 0x04U, 0x02U, 0x01U, 0x02U, 0x04U, 0x08U }, /* < */
        { 0x00U, 0x00U, 0x1FU, 0x00U, 0x1FU, 0x00U, 0x00U }, /* = */
        { 0x02U, 0x04U, 0x08U, 0x10U, 0x08U, 0x04U, 0x02U }, /* > */
        { 0x0EU, 0x11U, 0x10U, 0x08U, 0x04U, 0x00U, 0x04U }, /* ? */
        { 0x0EU, 0x11U, 0x10U, 0x16U, 0x15U, 0x15U, 0x0EU }, /* @ */
        { 0x0EU, 0x11U, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U }, /* A */
        { 0x0FU, 0x11U, 0x11U, 0x0FU, 0x11U, 0x11U, 0x0FU }, /* B */
        { 0x0EU, 0x11U, 0x01U, 0x01U, 0x01U, 0x11U, 0x0EU }, /* C */
        { 0x07U, 0x09U, 0x11U, 0x11U, 0x11U, 0x09U, 0x07U }, /* D */
        { 0x1FU, 0x01U, 0x01U, 0x0FU, 0x01U, 0x01U, 0x1FU }, /* E */
        { 0x1FU, 0x01U, 0x01U, 0x0FU, 0x01U, 0x01U, 0x01U }, /* F */
        { 0x0EU, 0x11U, 0x01U, 0x1DU, 0x11U, 0x11U, 0x1EU }, /* G */
        { 0x11U, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U, 0x11U }, /* H */
        { 0x0EU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU }, /* I */
        { 0x1CU, 0x08U, 0x08U, 0x08U, 0x08U, 0x09U, 0x06U }, /* J */
        { 0x11U, 0x09U, 0x05U, 0x03U, 0x05U, 0x09U, 0x11U }, /* K */
        { 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x1FU }, /* L */
        { 0x11U, 0x1BU, 0x15U, 0x15U, 0x11U, 0x11U, 0x11U }, /* M */
        { 0x11U, 0x11U, 0x13U, 0x15U, 0x19U, 0x11U, 0x11U }, /* N */
        { 0x0EU, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU }, /* O */
        { 0x0FU, 0x11U, 0x11U, 0x0FU, 0x01U, 0x01U, 0x01U }, /* P */
        { 0x0EU, 0x11U, 0x11U, 0x11U, 0x15U, 0x09U, 0x16U }, /* Q */
        { 0x0FU, 0x11U, 0x11U, 0x0FU, 0x05U, 0x09U, 0x11U }, /* R */
        { 0x1EU, 0x01U, 0x01U, 0x0EU, 0x10U, 0x10U, 0x0FU }, /* S */
        { 0x1FU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U }, /* T */
        { 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU }, /* U */
        { 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U }, /* V */
        { 0x11U, 0x11U, 0x11U, 0x15U, 0x15U, 0x15U, 0x0AU }, /* W */
        { 0x11U, 0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U, 0x11U }, /* X */
        { 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U, 0x04U, 0x04U }, /* Y */
        { 0x1FU, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U, 0x1FU }, /* Z */
        { 0x0EU, 0x02U, 0x02U, 0x02U, 0x02U, 0x02U, 0x0EU }, /* [ */
        { 0x00U, 0x01U, 0x02U, 0x04U, 0x08U, 0x10U, 0x00U }, /* \ */
        { 0x0EU, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x0EU }, /* ] */
        { 0x04U, 0x0AU, 0x11U, 0x00U, 0x00U, 0x00U, 0x00U }, /* ^ */
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x1FU }, /* _ */
        { 0x02U, 0x04U, 0x08U, 0x00U, 0x00U, 0x00U, 0x00U }, /* ` */
        { 0x00U, 0x00U, 0x0EU, 0x10U, 0x1EU, 0x11U, 0x1EU }, /* a */
        { 0x01U, 0x01U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x0FU }, /* b */
        { 0x00U, 0x00U, 0x0EU, 0x01U, 0x01U, 0x11U, 0x0EU }, /* c */
        { 0x10U, 0x10U, 0x16U, 0x19U, 0x11U, 0x11U, 0x1EU }, /* d */
        { 0x00U, 0x00U, 0x0EU, 0x11U, 0x1FU, 0x01U, 0x0EU }, /* e */
        { 0x0CU, 0x12U, 0x02U, 0x07U, 0x02U, 0x02U, 0x02U }, /* f */
        { 0x00U, 0x1EU, 0x11U, 0x11U, 0x1EU, 0x10U, 0x0EU }, /* g */
        { 0x01U, 0x01U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x11U }, /* h */
        { 0x04U, 0x00U, 0x06U, 0x04U, 0x04U, 0x04U, 0x0EU }, /* i */
        { 0x08U, 0x00U, 0x0CU, 0x08U, 0x08U, 0x09U, 0x06U }, /* j */
        { 0x01U, 0x01U, 0x09U, 0x05U, 0x03U, 0x05U, 0x09U }, /* k */
        { 0x06U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU }, /* l */
        { 0x00U, 0x00U, 0x0BU, 0x15U, 0x15U, 0x11U, 0x11U }, /* m */
        { 0x00U, 0x00U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x11U }, /* n */
        { 0x00U, 0x00U, 0x0EU, 0x11U, 0x11U, 0x11U, 0x0EU }, /* o */
        { 0x00U, 0x00U, 0x0FU, 0x11U, 0x0FU, 0x01U, 0x01U }, /* p */
        { 0x00U, 0x00U, 0x16U, 0x19U, 0x1EU, 0x10U, 0x10U }, /* q */
        { 0x00U, 0x00U, 0x0DU, 0x13U, 0x01U, 0x01U, 0x01U }, /* r */
        { 0x00U, 0x00U, 0x0EU, 0x01U, 0x0EU, 0x10U, 0x0FU }, /* s */
        { 0x02U, 0x02U, 0x07U, 0x02U, 0x02U, 0x12U, 0x0CU }, /* t */
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x11U, 0x19U, 0x16U }, /* u */
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U }, /* v */
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x15U, 0x15U, 0x0AU }, /* w */
        { 0x00U, 0x00U, 0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U }, /* x */
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x1EU, 0x10U, 0x0EU }, /* y */
        { 0x00U, 0x00U, 0x1FU, 0x08U, 0x04U, 0x02U, 0x1FU }, /* z */
        { 0x08U, 0x04U, 0x04U, 0x02U, 0x04U, 0x04U, 0x08U }, /* { */
        { 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U }, /* | */
        { 0x02U, 0x04U, 0x04U, 0x08U, 0x04U, 0x04U, 0x02U }, /* } */
        { 0x02U, 0x15U, 0x08U, 0x00U, 0x00U, 0x00U, 0x00U }, /* ~ */
    };

    for (; *str != '\0'; ++str, x += 6) {
        uint8_t const *ch = &font5x7[*str - ' '][0];
        paintBitsClear(x, y, ch, 7);
    }
}

/*==========================================================================*/
typedef struct { /* the auxiliary structure to hold const bitmaps */
    uint8_t const *bits; /* the bits in the bitmap */
    uint8_t height;      /* the height of the bitmap */
} Bitmap;

/* bitmap of the Ship:
*
*     x....
*     xxx..
*     xxxxx
*/
static uint8_t const ship_bits[] = {
    0x01U, 0x07U, 0x1FU
};

/* bitmap of the Missile:
*
*     xxxx
*/
static uint8_t const missile_bits[] = {
    0x0FU
};

/* bitmap of the Mine type-1:
*
*     .x.
*     xxx
*     .x.
*/
static uint8_t const mine1_bits[] = {
    0x02U, 0x07U, 0x02U
};

/* bitmap of the Mine type-2:
*
*     x..x
*     .xx.
*     .xx.
*     x..x
*/
static uint8_t const mine2_bits[] = {
    0x09U, 0x06U, 0x06U, 0x09U
};

/* Mine type-2 is nastier than Mine type-1. The type-2 mine can
* hit the Ship with any of its "tentacles". However, it can be
* destroyed by the Missile only by hitting its center, defined as
* the following bitmap:
*
*     ....
*     .xx.
*     .xx.
*/
static uint8_t const mine2_missile_bits[] = {
    0x00U, 0x06U, 0x06U
};

/*
* The bitmap of the explosion stage 0:
*
*     .......
*     ...x...
*     ..x.x..
*     ...x...
*/
static uint8_t const explosion0_bits[] = {
    0x00U, 0x08U, 0x14U, 0x08U
};

/*
* The bitmap of the explosion stage 1:
*
*     .......
*     ..x.x..
*     ...x...
*     ..x.x..
*/
static uint8_t const explosion1_bits[] = {
    0x00U, 0x14U, 0x08U, 0x14U
};

/*
* The bitmap of the explosion stage 2:
*
*     .x...x.
*     ..x.x..
*     ...x...
*     ..x.x..
*     .x...x.
*/
static uint8_t const explosion2_bits[] = {
    0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U
};

/*
* The bitmap of the explosion stage 3:
*
*     x..x..x
*     .x.x.x.
*     ..x.x..
*     xx.x.xx
*     ..x.x..
*     .x.x.x.
*     x..x..x
*/
static uint8_t const explosion3_bits[] = {
    0x49, 0x2A, 0x14, 0x6B, 0x14, 0x2A, 0x49
};

static Bitmap const l_bitmap[MAX_BMP] = {
    { ship_bits,       Q_DIM(ship_bits) },
    { missile_bits,    Q_DIM(missile_bits) },
    { mine1_bits,      Q_DIM(mine1_bits) },
    { mine2_bits,      Q_DIM(mine2_bits) },
    { mine2_missile_bits, Q_DIM(mine2_missile_bits) },
    { explosion0_bits, Q_DIM(explosion0_bits) },
    { explosion1_bits, Q_DIM(explosion1_bits) },
    { explosion2_bits, Q_DIM(explosion2_bits) },
    { explosion3_bits, Q_DIM(explosion3_bits) }
};

/*..........................................................................*/
void BSP_paintBitmap(uint8_t x, uint8_t y, uint8_t bmp_id) {
    Bitmap const *bmp = &l_bitmap[bmp_id];
    paintBits(x, y, bmp->bits, bmp->height);
}
/*..........................................................................*/
void BSP_advanceWalls(uint8_t top, uint8_t bottom) {
    uint_fast8_t y;
    for (y = 0U; y < GAME_TUNNEL_HEIGHT; ++y) {
        /* shift the walls one pixel to the left */
        l_walls[y][0] = (l_walls[y][0] >> 1) | (l_walls[y][1] << 31);
        l_walls[y][1] = (l_walls[y][1] >> 1) | (l_walls[y][2] << 31);
        l_walls[y][2] = (l_walls[y][2] >> 1) | (l_walls[y][3] << 31);
        l_walls[y][3] = (l_walls[y][3] >> 1);

        /* add new column of walls at the end */
        if (y <= top) {
            l_walls[y][3] |= (1U << 31);
        }
        if (y >= (GAME_TUNNEL_HEIGHT - bottom)) {
            l_walls[y][3] |= (1U << 31);
        }

        /* copy the walls to the frame buffer */
        l_fb[y][0] = l_walls[y][0];
        l_fb[y][1] = l_walls[y][1];
        l_fb[y][2] = l_walls[y][2];
        l_fb[y][3] = l_walls[y][3];
    }
}
/*..........................................................................*/
bool BSP_doBitmapsOverlap(uint8_t bmp_id1, uint8_t x1, uint8_t y1,
                          uint8_t bmp_id2, uint8_t x2, uint8_t y2)
{
    uint8_t y;
    uint8_t y0;
    uint8_t h;
    uint32_t bits1;
    uint32_t bits2;
    Bitmap const *bmp1;
    Bitmap const *bmp2;

    Q_REQUIRE((bmp_id1 < Q_DIM(l_bitmap)) && (bmp_id2 < Q_DIM(l_bitmap)));

    /* are the bitmaps close enough in x? */
    if (x1 >= x2) {
        if (x1 > x2 + 8U) {
            return false;
        }
        x1 -= x2;
        x2 = 0U;
    }
    else {
        if (x2 > x1 + 8U) {
            return false;
        }
        x2 -= x1;
        x1 = 0U;
    }

    bmp1 = &l_bitmap[bmp_id1];
    bmp2 = &l_bitmap[bmp_id2];
    if ((y1 <= y2) && (y1 + bmp1->height > y2)) {
        y0 = y2 - y1;
        h = y1 + bmp1->height - y2;
        if (h > bmp2->height) {
            h = bmp2->height;
        }
        for (y = 0; y < h; ++y) { /* scan over the overlapping rows */
            bits1 = ((uint32_t)bmp1->bits[y + y0] << x1);
            bits2 = ((uint32_t)bmp2->bits[y] << x2);
            if ((bits1 & bits2) != 0U) { /* do the bits overlap? */
                return true; /* yes! */
            }
        }
    }
    else {
        if ((y1 > y2) && (y2 + bmp2->height > y1)) {
            y0 = y1 - y2;
            h = y2 + bmp2->height - y1;
            if (h > bmp1->height) {
                h = bmp1->height;
            }
            for (y = 0; y < h; ++y) {  /* scan over the overlapping rows */
                bits1 = ((uint32_t)bmp1->bits[y] << x1);
                bits2 = ((uint32_t)bmp2->bits[y + y0] << x2);
                if ((bits1 & bits2) != 0U) { /* do the bits overlap? */
                                       return true; /* yes! */
                }
            }
        }
    }
    return false; /* the bitmaps do not overlap */
}
/*..........................................................................*/
bool BSP_isWallHit(uint8_t bmp_id, uint8_t x, uint8_t y) {
    Bitmap const *bmp = &l_bitmap[bmp_id];
    uint32_t shft = (x & 0x1FU);
    uint32_t *walls = &l_walls[y][x >> 5];
    for (y = 0; y < bmp->height; ++y, walls += (BSP_SCREEN_WIDTH >> 5)) {
        if (*walls & ((uint32_t)bmp->bits[y] << shft)) {
            return true;
        }
        if (shft > 24U) {
            if (*(walls + 1) & ((uint32_t)bmp->bits[y] >> (32U - shft))) {
                return true;
            }
        }
    }
    return false;
}

/*..........................................................................*/
void BSP_updateScore(uint16_t score) {
    char str[5];
    uint16_t s = score;

    if (score == 0U) {
        BSP_paintString(1U, BSP_SCREEN_HEIGHT - 8U, "SCORE:");
    }

    /* update the SCORE area on the screeen */
    str[4] = '\0';
    str[3] = (s % 10U) + '0'; s /= 10U;
    str[2] = (s % 10U) + '0'; s /= 10U;
    str[1] = (s % 10U) + '0'; s /= 10U;
    str[0] = (s % 10U) + '0';
    BSP_paintString(6U*6U, BSP_SCREEN_HEIGHT - 8U, str);
}
/*..........................................................................*/
void BSP_displayOn(void) {
    Display_enable(true);
}
/*..........................................................................*/
void BSP_displayOff(void) {
    Display_enable(false);
}
/*..........................................................................*/
uint32_t BSP_random(void) {  /* a very cheap pseudo-random-number generator */
    /* "Super-Duper" Linear Congruential Generator (LCG)
    * LCG(2^32, 3*7*11*13*23, 0, seed)
    */
    l_rnd = l_rnd * (3U*7U*11U*13U*23U);
    return l_rnd >> 8;
}
/*..........................................................................*/
void BSP_randomSeed(uint32_t seed) {
    l_rnd = seed;
}

/*--------------------------------------------------------------------------*/
/*..........................................................................*/
static void paintBits(uint8_t x, uint8_t y, uint8_t const *bits, uint8_t h) {
    uint32_t *fb = &l_fb[y][x >> 5];
    uint32_t shft = (x & 0x1FU);
    for (y = 0; y < h; ++y, fb += (BSP_SCREEN_WIDTH >> 5)) {
        *fb |= ((uint32_t)bits[y] << shft);
        if (shft > 24U) {
            *(fb + 1) |= ((uint32_t)bits[y] >> (32U - shft));
        }
    }
}
/*..........................................................................*/
static void paintBitsClear(uint8_t x, uint8_t y,
                           uint8_t const *bits, uint8_t h)
{
    uint32_t *fb = &l_fb[y][x >> 5];
    uint32_t shft = (x & 0x1FU);
    uint32_t mask1 = ~((uint32_t)0xFFU << shft);
    uint32_t mask2;
    if (shft > 24U) {
        mask2 = ~(0xFFU >> (32U - shft));
    }
    for (y = 0; y < h; ++y, fb += (BSP_SCREEN_WIDTH >> 5)) {
        *fb = ((*fb & mask1) | ((uint32_t)bits[y] << shft));
        if (shft > 24U) {
            *(fb + 1) = ((*(fb + 1) & mask2)
                | ((uint32_t)bits[y] >> (32U - shft)));
        }
    }
}

/* QF callbacks ============================================================*/
void QF_onStartup(void) {
    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* assing all priority bits for preemption-prio. and none to sub-prio. */
    NVIC_SetPriorityGrouping(0U);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   SYSTICK_PRIO);
    NVIC_SetPriority(GPIO_EVEN_IRQn, GPIO_EVEN_PRIO);
    /* ... */

    /* enable IRQs... */
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}
/*..........................................................................*/
void QF_stop(void) {
}
/*..........................................................................*/
void QK_onIdle(void) {
    /* toggle the User LED on and then off, see NOTE01 */
    QF_INT_DISABLE();
    GPIO->P[LED_PORT].DOUT |=  (1U << LED1_PIN);
    GPIO->P[LED_PORT].DOUT &= ~(1U << LED1_PIN);
    QF_INT_ENABLE();

#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M3 MCU.
    */
    __WFI(); /* Wait-For-Interrupt */
#endif
}

/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR module, int loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)loc;

#ifndef NDEBUG
    /* light up both LEDs */
    GPIO->P[LED_PORT].DOUT |= ((1U << LED0_PIN) | (1U << LED1_PIN));
    /* for debugging, hang on in an endless loop until PB1 is pressed... */
    while ((GPIO->P[PB_PORT].DIN & (1U << PB1_PIN)) != 0) {
    }
#endif

    NVIC_SystemReset();
}

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QK_ISR_ENTRY/QK_ISR_ENTRY
* macros or any other QF/QK  services. These ISRs are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF/QK services. In particular they
* can NOT call the macros QK_ISR_ENTRY/QK_ISR_ENTRY. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The User LED is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/
