.syntax unified     // in the pas THUMB and ARM instructions had distinct syntaxes, then a unified syntax was introduced - use it
.cpu cortex-m4      // STM32L476RG is cortex-m4

.fpu fpv4-sp-d16    // Hardware FPU

.thumb              // equivalent to .code16, generate instructions encoed as THUMB

/* Easy macro for definig weak symbol vectors */
.macro      vector name
.weak       \name
.thumb_set  \name, DEFAULT_IRQ_handler
    .word    \name
.endm

/*
 * The vector table.
 */
.type vtable, %object
.section .vector_table,"a",%progbits
vtable:
    // Cortex-M4
    .word _estack
    .word RESET_handler
    vector NMI_handler
    vector HARD_FAULT_handler
    vector MEM_MANAGE_handler
    vector BUS_FAULT_handler
    vector USAGE_FAULT_handler
    .word 0
    .word 0
    .word 0
    .word 0
    vector SVC_handler
    vector DEBUG_handler
    .word 0
    vector PEND_SV_handler
    vector SYSTICK_handler
    // 0-15
    vector WWDG_IRQ_handler
    vector PVD_PVM_IRQ_handler
    vector RTC_TAMP_STAMP_IRQ_handler
    vector RTC_WKUP_IRQ_handler
    vector FLASH_IRQ_handler
    vector RCC_IRQ_handler
    vector EXTI0_IRQ_handler
    vector EXTI1_IRQ_handler
    vector EXTI2_IRQ_handler
    vector EXTI3_IRQ_handler
    vector EXTI4_IRQ_handler
    vector DMA1_CH1_IRQ_handler
    vector DMA1_CH2_IRQ_handler
    vector DMA1_CH3_IRQ_handler
    vector DMA1_CH4_IRQ_handler
    vector DMA1_CH5_IRQ_handler
    // 16-31
    vector DMA1_CH6_IRQ_handler
    vector DMA1_CH7_IRQ_handler
    vector ADC1_2_IRQ_handler
    vector CAN1_TX_IRQ_handler
    vector CAN1_RX0_IRQ_handler
    vector CAN1_RX1_IRQ_handler
    vector CAN1_SCE_IRQ_handler
    vector EXTI9_5_IRQ_handler
    vector TIM1_BRK_TIM15_IRQ_handler
    vector TIM1_UP_TIM16_IRQ_handler
    vector TIM1_TRG_COM_TIM17_IRQ_handler
    vector TIM1_CC_IRQ_handler
    vector TIM2_IRQ_handler
    vector TIM3_IRQ_handler
    vector TIM4_IRQ_handler
    vector I2C1_EV_IRQ_handler
    // 32-47
    vector I2C1_ER_IRQ_handler
    vector I2C2_EV_IRQ_handler
    vector I2C2_ER_IRQ_handler
    vector SPI1_IRQ_handler
    vector SPI2_IRQ_handler
    vector USART1_IRQ_handler
    vector USART2_IRQ_handler
    vector USART3_IRQ_handler
    vector EXTI15_10_IRQ_handler
    vector RTC_ALARM_IRQ_handler
    vector DFSDM1_FLT3_IRQ_handler
    vector TIM8_BRK_IRQ_handler
    vector TIM8_UP_IRQ_handler
    vector TIM8_TRG_COM_IRQ_handler
    vector TIM8_CC_IRQ_handler
    vector ADC3_IRQ_handler
    // 48-63
    vector FMC_handler
    vector SDMMC1_handler
    vector TIM5_handler
    vector SPI3_handler
    vector UART4_handler
    vector UART5_handler
    vector TIM6_DACUNDER_handler
    vector TIM7_handler
    vector DMA2_CH1_handler
    vector DMA2_CH2_handler
    vector DMA2_CH3_handler
    vector DMA2_CH4_handler
    vector DMA2_CH5_handler
    vector DFSDM1_FLT0_handler
    vector DFSDM1_FLT1_handler
    vector DFSDM1_FLT2_handler
    // 64-79
    vector COMP_handler
    vector LPTIM1_handler
    vector LPTIM2_handler
    vector OTG_FS_handler
    vector DMA2_CH6_handler
    vector DMA2_CH7_handler
    vector LPUART1_handler
    vector QUADSPI_handler
    vector I2C3_EV_handler
    vector I2C3_ER_handler
    vector SAI1_handler
    vector SAI2_handler
    vector SWPMI1_handler
    vector TSC_handler
    vector LCD_handler
    .word 0 /* Would be AES handler, but wrong chip */
    // 80-82
    vector RNG_handler /* Also HASH handler on chips that support it */
    vector FPU_handler
    vector CRS_handler
.size vtable, .-vtable

/*
 * A 'Default' interrupt handler. This is where interrupts
 * which are not otherwise configured will go.
 * It is an infinite loop, because...well, we weren't
 * expecting the interrupt, so what can we do?
 */
.section .text.DEFAULT_IRQ_handler,"ax",%progbits
.type DEFAULT_IRQ_handler, %function
DEFAULT_IRQ_handler:
    B DEFAULT_IRQ_handler
.size DEFAULT_IRQ_handler, .-DEFAULT_IRQ_handler

.section .text.RESET_handler,"ax",%progbits
.type RESET_handler, %function
RESET_handler:
    // Set the stack pointer to the end of the stack.
    // The '_estack' value is defined in our linker script.
    LDR  r0, =_estack
    MOV  sp, r0
    // Copy data from flash to RAM data init section.
    // R2 will store our progress along the sidata section.
    MOVS r0, #0

    // Load the start/end addresses of the data section,
    // and the start of the data init section.
    LDR  r1, =_sdata
    LDR  r2, =_edata
    LDR  r3, =_sidata
    B    copy_sidata_loop

copy_sidata:
    // Offset the data init section by our copy progress.
    LDR  r4, [r3, r0]
    // Copy the current word into data, and increment.
    STR  r4, [r1, r0]
    ADDS r0, r0, #4
copy_sidata_loop:
    // Unless we've copied the whole data section, copy the
    // next word from sidata->data.
    ADDS r4, r0, r1
    CMP  r4, r2
    BCC  copy_sidata

    // Once we are done copying the data section into RAM,
    // move on to filling the BSS section with 0s.
    MOVS r0, #0
    LDR  r1, =_sbss
    LDR  r2, =_ebss
    B    reset_bss_loop
    // Fill the BSS segment with '0's.
reset_bss:
    // Store a 0 and increment by a word.
    STR  r0, [r1]
    ADDS r1, r1, #4
reset_bss_loop:
    // We'll use R1 to count progress here; if we aren't
    // done, reset the next word and increment.
    CMP  r1, r2
    BCC  reset_bss
    // Turn on the FPU (Cortex-M4 Reference Manual 7.3.1)
    // CPACR is located at address 0xE000ED88
    LDR.W R0, =0xE000ED88
    // Read CPACR
    LDR R1, [R0]
    // Set bits 20-23 to enable CP10 and CP11 coprocessors
    ORR R1, R1, #(0xF << 20)
    // Write back the modified value to the CPACR
    STR R1, [R0]

    // Branch to the 'main' method.
    B    main
.size RESET_handler, .-RESET_handler
