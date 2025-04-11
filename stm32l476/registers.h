#ifndef _STM32L476_REGISTERS_H_
#define _STM32L476_REGISTERS_H_

#include <stdint.h>

// bit value macro
#define _BV(N) (1 << (N))

// ARM7m reference manual B3.4.3
struct _NVIC_t_ {
    uint32_t ISER[16];
    // dont care about other stuff, maybe priority register could be useful in the future
};

// ARM7m reference manual B3.2
#define NVIC ((volatile struct _NVIC_t_*) 0xE000E100)

// Copied directly from https://github.com/ARM-software/CMSIS_4/blob/f2cad4345783c948ed4a7f5cdb02cdc0856366f1/CMSIS/Include/core_cm4.h#L1626C17-L1629C2
// The arithmetic is easy but no reason to reinvent the wheel
void NVIC_EnableIRQ(int32_t IRQn) {
    NVIC->ISER[(((uint32_t)(int32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
};

struct _RCC_t {
    uint8_t _dont_care[0x08];

    uint32_t CFGR; // clock configuration register - Reference manual 6.4.3 - offset 0x08, pins defined below
    #define RCC_CFGR_HPRE0      4 // AHB prescaler bit 0
    #define RCC_CFGR_HPRE1      5 // AHB prescaler bit 0
    #define RCC_CFGR_HPRE2      6 // AHB prescaler bit 0
    #define RCC_CFGR_HPRE3      7 // AHB prescaler bit 0

    uint8_t _dont_care_2[0x3C];

    uint32_t AHB1ENR; // Reference manual 6.4.16 - offset 0x48, pins below
    #define RCC_AHB1ENR_CRCEN   12 // enable CRC clock
    #define RCC_AHB1ENR_DMA1EN  0  // enable DMA1
    #define RCC_AHB1ENR_DMA2EN  1  // enable DMA2

    uint32_t AHB2ENR; // Reference manual 6.4.17 - offset 0x4C, pins defined below
    #define RCC_AHB2ENR_GPIOAEN 0

    uint8_t _dont_care_3[0x08];

    uint32_t APB1ENR1; // Reference manual 6.4.19 - offset 0x58, pins defined below
    #define RCC_APB1ENR1_TIM7EN     5  // enable TIMer 7
    #define RCC_APB1ENR1_USART2EN   17 // enable USART2
};

struct _GPIOx_t {
    uint32_t MODER; // mode of operation per pin - Reference manual 8.5.1 - offset 0x0, values for bit pairs defined below
    #define GPIOx_MODE_INPUT    0 // Input
    #define GPIOx_MODE_GPOUT    1 // General Purpose Output
    #define GPIOx_MODE_AF       2 // Alternate Function (GPIO off)
    #define GPIOx_MODE_ANALOG   3 // Analog (reset state, default for most pins)

    uint32_t OTYPER; // output type - Reference manual 8.5.2 - offset 0x4, values for bits defined below
    #define GPIOx_OTYPE_PP      0 // Push-pull (default for most)
    #define GPIOx_OTYPE_OD      1 // Open-drain

    uint32_t OSPEEDR; // output speed - Reference manual 8.5.3 - offset 0x8, values for bit pairs defined below
    #define GPIOx_OSPEED_LOW    0 // default for most pins
    #define GPIOx_OSPEED_MEDIUM 1
    #define GPIOx_OSPEED_HIGH   2
    #define GPIOx_OSPEED_VHIGH  3

    uint32_t PUPDR; // explicit pull - Reference manual 8.5.4 - offset 0xC, values for bit pairs defined below
    #define GPIOx_OPUPD_NONE    0 // no explicit pull up or pull down (default for most)
    #define GPIOx_OPUPD_PU      1 // explicit Pull up
    #define GPIOx_OPUPD_PD      2 // explicit Pull down

    uint32_t IDR; // input data register (readonly) - Reference manual 8.5.5 - offset 0x10
    uint32_t ODR; // output data register (read-write) - Reference manual 8.5.6 - offset 0x14
    // theres also BSRR and some other stuff i think but its not really needed for now?

    uint8_t _dont_care[0x08];

    uint32_t AFRL; // Alternative function register Low - Reference manual 8.5.9 - offset 0x20
    uint32_t AFRH; // Alternative function register High - Reference manual 8.5.10 - offset 0x24

};

// Structure only for TIM6 and TIM7! Reference manual 33.4
// some of these registers are displayed as 16-bit in their manual entries, but in
// 33.4.9 it is shown that they are indeed 32 bits, and their top half of bits just is not used
struct _TIMx_6_7_t_ {
    uint32_t CR1; // control register 1 - Reference manual 33.4.1 - offset 0x0, bits defined below
    #define TIMx_6_7_CR1_CEN    0 // Counter enable, 1 for on 0 for off (default off)
    #define TIMx_6_7_CR1_UDIS   1 // Update disable, 1 for on (disabled update event) , 0 for off (event enabled)
    #define TIMx_6_7_CR1_URS    2 // Update requst source - decides what generates an udpate event (Ref manual)
    #define TIMx_6_7_CR1_OPM    3 // One pulse mode enable

    uint8_t _dont_care[0x08];

    uint32_t DIER; // DMA/interrupt enable register - Reference manual 33.4.3 - offset 0x0C, bits defined below
    #define TIMx_6_7_DIER_UIE   0 // Interrupt enable
    #define TIMx_6_7_DIER_UDE   8 // DMA request enable

    uint32_t SR; // Status register - Reference manual 33.4.4 - offset 0x10, bits defined below
    #define TIMx_6_7_SR_UIF     0 // Update interrupt flag - set to 1 by hardware on update event, has to be cleared in software

    uint32_t EGR; // Event generation register - Reference manual 33.4.5 - offset 0x14, bits defined below
    #define TIMx_6_7_EGR_UG     0 // Update generator - resets the timer and updates ARR and CNT values (?)

    uint8_t _dont_care_2[0x10];

    uint32_t PSC; // Prescaler register (16 bit) - Reference manual 33.4.7 - offset 0x28
    // From the manual, counter clock frequency is equal to: fCK_PSC / (PSC[15:0] + 1)
    // For example, with sysclk equal 4MHz, setting PSC to 39999 will result in frequency of
    // 4MHz / 40000 = 100 Hz

    uint32_t ARR; // Auto reload register (16 bit) - Reference manual 33.4.8 - offset 0x2C
    // The value in the register specifies the top value for the counter to triggeri nterrupt on
};

// Manual 15, 15.4 for registers
struct _CRC_t_ {
    uint32_t DR; // Data register - Reference manual 15.4.1 - offset 0x00
    // Used for writing new data to modify CRC state, on reading returns current CRC value (might need to NOT it)
    
    uint32_t IDR; // Independent data register - Reference manual 15.4.2 - offset 0x04
    // Can be used to store any data in it, for no reason whatsoever it seems, only lowest 8 bits are usable though

    uint32_t CR; // Control regiser - Reference manual 15.4.3 - offset 0x08
    #define CRC_CR_REV_OUT      7 // Reverse of output data at bit level
    #define CRC_CR_REV_IN1      6 // Reverse of input data at bit level bit 1
    #define CRC_CR_REV_IN0      5 // Reverse of input data at bit level bit 0
    // Possible settings for input: 00 no reverse, 01 byte level, 10 half-word level, 11 word level
    #define CRC_CR_POLYSIZE1    4 // Polynomial size bit 1
    #define CRC_CR_POLYSIZE0    3 // Polynomial size bit 0
    // Possible settings are: 00 for 32 bit polynomial (default), 01 for 16 bit polynomial, 10 for 8 bit, 11 for 7 bit polynomial
    #define CRC_CR_RESET        0 // Reset bit, set in software and automatically cleared by hardware, resets the CRC state and initializes the state to CRC_INIT register contents

    uint32_t INIT; // Initial state for CRC - Reference manual 15.4.4 - offset 0x0C (default is 0xFFFFFFFF! not zeros!)

    uint32_t POL; // Polynomial - Reference manual 15.4.5 - offset 0x10 (default is 0x04C11DB7, which i think is the ethernet polynomial? ISO3039 defined it i think but was withdrawn and replaced with newer iso)
};

struct _USART_t_ {
    uint32_t CR1; // Control register1 - Reference manual 40.8.1 - offset 0x00
    #define USART_CR1_M1        28 // Word length bit 1
    #define USART_CR1_M0        12 // Word length bit 0
    // 00 - 1 start, 8 data bits (default), 01 - 1 start, 9 data bits, 10 - 1 star, 7 data bits
    #define USART_CR1_OVER8     15 // OVersampling - 0 = 16 (default), 1 = 8
    #define USART_CR1_PCE       10 // Parity control enable, (default = disabled)
    #define USART_CR1_PS        9  // Parity selection (default = 0 = even), 1 = odd
    // Some interrupt enables here...
    #define USART_CR1_TE        3  // Transmitter enable
    #define USART_CR1_RE        2  // Receiver enable
    #define USART_CR1_UE        0  // USART enable

    // Theres CR2 and CR3 here at offset 0x04 but only thing that is interesting in it as of now
    // would be stop bits amount, which is 1 by default so thats ok
    // Theres also DMA enable for transmit and receive, but i think they are for when UART requests
    // a DMA transfer, meanwhile in our case its a timer, so there is no need to set them

    uint8_t _dont_care1[0x08];

    uint32_t BRR; // Baud rate selection - Reference manual 40.8.4 - offset 0x0C
    // Rules for BRR calculation from baud rate are in reference manual 40.5.4

    // Here a bunch more hardware controlling bits and registers
    // Theres ISR somewhere here, which among other things contains TXE - transmitter register empty
    // which is used for polling for when another byte can be transmitted i guess
    // not used for DMA though

    uint8_t _dont_care2[0x14];

    uint32_t RDR; // Receive data register - Reference manual 40.8.10 - offset 0x24
    uint32_t TDR; // Transmit data register - Reference manual 40.8.11 - offset 0x28
    // In both registers only lowest 8 bits are usable (RDR - readonly too)
};

struct _DMA_channel_t_ {
    uint32_t CCR; // Channel configuration register - REference manual 11.6.3
    #define DMA_CCRx_MEM2MEM    14 // For memory to memory transfers
    #define DMA_CCRx_PL1        13 // Priority level bit 1
    #define DMA_CCRx_PL0        12 // Priority level bit 0
    // Priority levels are in order 00, 01, 10, 11 for Low (default), medium, high, very high
    #define DMA_CCRx_MSIZE1     11 // Memory size bit 1
    #define DMA_CCRx_MSIZE0     10 // Memory size bit 0
    // 00 - 8 bits (default) 01 - 16 bits, 10 - 32 bits
    #define DMA_CCRx_PSIZE1      9 // Peripheral size bit 1
    #define DMA_CCRx_PSIZE0      8 // Peripheral size bit 0
    // 00 - 8 bits (default) 01 - 16 bits, 10 - 32 bits
    #define DMA_CCRx_MINC       7 // Memory increment enable (default 0)
    #define DMA_CCRx_PINC       6 // Peripheral increment enable (default 0)
    #define DMA_CCRx_CIRC       5 // circular mode enable (default 0)
    #define DMA_CCRx_DIR        4 // direction bit (Default 0)
    // Rather complicated due to mem/mem mode and peri/peri mode (look in manual)
    // but for peripheral/mem modes 0 = read from peripheral, 1 = read from memory
    // Niche cases explained in CPAR and CMAR register definitions
    #define DMA_CCRx_EN         0 // Channel enable (default 0)

    uint32_t CNDTR; // number of data to transfer register - Reference manual 11.6.4
    // Contains number of data to transmit (not bytes!) and is reloaded automatically in case of CIRC mode

    uint32_t CPAR; // peripheral address register - Reference manual 11.6.5
    uint32_t CMAR; // memory address register - Reference manual 11.6.6

    uint32_t _reserved;
};

struct _DMA_t_ {
    uint8_t _dont_care[8]; // ISR and IFCR registers

    struct _DMA_channel_t_ channel[7]; // 7 channels
    uint32_t _reserved[5];
    uint32_t CSELR; // Channel selection - Reference manual 11.6.7 - Offset 0xA8 (default 0x0)
    // 4 bits per channel
};

// Below addresses from Datasheet 5. - Memory Map, table 19.
#define RCC ((volatile struct _RCC_t*) 0x40021000)
#define GPIOA ((volatile struct _GPIOx_t*) 0x48000000)
#define TIM7 ((volatile struct _TIMx_6_7_t_*) 0x40001400)
#define TIM6 ((volatile struct _TIMx_6_7_t_*) 0x40001000)
#define CRC ((volatile struct _CRC_t_*) 0x40023000)
#define USART2 ((volatile struct _USART_t_*) 0x40004400)
#define DMA1 ((volatile struct _DMA_t_*) 0x40020000)

#endif
