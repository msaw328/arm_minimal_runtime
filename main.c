#include "stm32l476/registers.h"

#define F_CPU 4000000UL

#define TIM7_IRQn (55)

#define LED_PIN (5)
#define USART_RX_PIN (3)
#define USART_TX_PIN (2)

char* uart_msg = "Hello from STM32!\r\n";
uint32_t uart_msg_len = 19;

void init_hw() {
    // First enable GPIOA peripheral
    RCC->AHB2ENR |= _BV(RCC_AHB2ENR_GPIOAEN);

    // Enable DMA1
    RCC->AHB1ENR |= _BV(RCC_AHB1ENR_DMA1EN);

    // Also TIM7 peripheral and USART2 (the one connected to Nucleo USB)
    RCC->APB1ENR1 |= _BV(RCC_APB1ENR1_TIM7EN) | _BV(RCC_APB1ENR1_USART2EN);

    // Good information on GPIO setup is in Reference manual 8.4, especially table 39.
    // Many of those operations are not needed due to default values of those registers,
    // but for the sake of explicity

    // MODER has 2 bits per GPIO, hence for example LED_PIN * 2
    // First set bits to 0, to make sure they are cleared
    GPIOA->MODER &= ~(0x3 << (LED_PIN * 2) | 0x3 << (USART_RX_PIN * 2) | 0x3 << (USART_TX_PIN * 2));

    // Then set them to general purpose output and alternate function for usart pins
    GPIOA->MODER |= (GPIOx_MODE_GPOUT << (LED_PIN * 2) | GPIOx_MODE_AF << (USART_RX_PIN * 2) | GPIOx_MODE_AF << (USART_TX_PIN * 2));

    // Set led and usart lines to push pull
    GPIOA->OTYPER &= ~((~GPIOx_OTYPE_PP) << LED_PIN | (~GPIOx_OTYPE_PP) << USART_RX_PIN | (~GPIOx_OTYPE_PP) << USART_TX_PIN);

    // Set speed to low, again clear bits first cause its 2 bits per pin
    GPIOA->OSPEEDR &= ~(0x3 << (LED_PIN * 2) | 0x3 << (USART_RX_PIN * 2) | 0x3 << (USART_TX_PIN * 2));
    GPIOA->OSPEEDR |= (GPIOx_OSPEED_LOW << (LED_PIN * 2) | GPIOx_OSPEED_LOW << (USART_RX_PIN * 2) | GPIOx_OSPEED_LOW << (USART_TX_PIN * 2));

    // Set pullups/pulldowns to no explicit choice for LED, and pullups for
    GPIOA->PUPDR &= ~(0x3 << (LED_PIN * 2) | 0x3 << (USART_RX_PIN * 2) | 0x3 << (USART_TX_PIN * 2));
    GPIOA->PUPDR |= (GPIOx_OPUPD_NONE << (LED_PIN * 2) | GPIOx_OPUPD_PU << (USART_RX_PIN * 2) | GPIOx_OPUPD_PU << (USART_TX_PIN * 2));

    // Set alternative functions to uart pins
    GPIOA->AFRL |= (0x7 << (USART_RX_PIN * 4) | 0x7 << (USART_TX_PIN * 4));

    // Set top value to 40000, so with 4MHz system clock it would results in 100Hz
    TIM7->ARR = 40000;

    // Set prescaler to 50 so it fires twice every second
    // fCK_PSC / (PSC[15:0] + 1)
    TIM7->PSC = 49;

    // Enable settings
    //TIM7->EGR |= _BV(TIMx_6_7_EGR_UG); // seems to not be needed? works without it

    // USART2 (the one connected to nucleo usb)
    // Set up baud rate according to 40.5.4 in ref manual (Table 249 for some examples)
    // 2.4 Kbps (with some error due to rounding)
    USART2->BRR = F_CPU / 2400;

    // enable receiver, transmitter and uart itself
    USART2->CR1 |= _BV(USART_CR1_RE) | _BV(USART_CR1_TE) | _BV(USART_CR1_UE);

    // Configure DMA1 transfers - 0101 (0x5) on channel 4 (bitshift by 4 bits, 3 times)
    DMA1->CSELR |= 0x5 << (4 * 3);
    // Channels are indexed from 1, but array indexing is from 0, so 4 - 1
    DMA1->channel[4 - 1].CPAR = (uint32_t) &(USART2->TDR); // Move to UART transmit register
    DMA1->channel[4 - 1].CMAR = (uint32_t) uart_msg; // From the string
    DMA1->channel[4 - 1].CNDTR = uart_msg_len; // this number of data elements (bytes)

    // Memory increment, circular, dir = 1 (memory to peripheral), enable
    // data size = 8 bits on both ends
    DMA1->channel[4 - 1].CCR |= _BV(DMA_CCRx_PL1) | _BV(DMA_CCRx_PL0) | _BV(DMA_CCRx_MINC) | _BV(DMA_CCRx_CIRC) | _BV(DMA_CCRx_DIR) | _BV(DMA_CCRx_EN);
    DMA1->channel[3 - 1].CPAR = (uint32_t) &(DMA1->CSELR);
    // Enable interrupt in NVIC too
    NVIC_EnableIRQ(TIM7_IRQn);

    // Enable interrupt in peripheral and also DMA request
    TIM7->DIER |= _BV(TIMx_6_7_DIER_UIE) | _BV(TIMx_6_7_DIER_UDE);

    // Finally enable clock
    TIM7->CR1 |= _BV(TIMx_6_7_CR1_CEN);
}

// Interrupt handler for TIM6 overflow event
// https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html for attribute
// It seems this attribute is not mandatory for arm7m, since it says its ignored - may be worth using in case of bugs though
void TIM7_handler()  {
    // flip output of the LED
    GPIOA->ODR ^= (1 << LED_PIN);

    // Clear interrupt flag
    TIM7->SR &= ~(_BV(TIMx_6_7_SR_UIF));
}

int main(void) {
    init_hw();

    while (1) {
        // Do nothing, should perhaps even sleep
    }
}
