/* Autogenerated vector table for hc32l130f8 */

.syntax unified
.cpu cortex-m0plus
.thumb

.global vtable
.global default_interrupt_handler

.type vtable, %object
.section .vector_table,"a",%progbits
vtable:
  .word _estack
  .word reset_handler
  .word NMI_IRQn_handler
  .word HardFault_IRQn_handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_IRQn_handler
  .word 0
  .word 0
  .word PendSV_IRQn_handler
  .word SysTick_IRQn_handler
  .word PORTA_IRQn_handler
  .word PORTB_IRQn_handler
  .word PORTC_IRQn_handler
  .word PORTD_IRQn_handler
  .word DMAC_IRQn_handler
  .word TIM3_IRQn_handler
  .word UART0_IRQn_handler
  .word UART1_IRQn_handler
  .word LPUART0_IRQn_handler
  .word LPUART1_IRQn_handler
  .word SPI0_IRQn_handler
  .word SPI1_IRQn_handler
  .word I2C0_IRQn_handler
  .word I2C1_IRQn_handler
  .word TIM0_IRQn_handler
  .word TIM1_IRQn_handler
  .word TIM2_IRQn_handler
  .word LPTIM_IRQn_handler
  .word TIM4_IRQn_handler
  .word TIM5_IRQn_handler
  .word TIM6_IRQn_handler
  .word PCA_IRQn_handler
  .word WDT_IRQn_handler
  .word RTC_IRQn_handler
  .word ADC_IRQn_handler
  .word PCNT_IRQn_handler
  .word VC0_IRQn_handler
  .word VC1_IRQn_handler
  .word LVD_IRQn_handler
  .word 0
  .word FLASH_RAM_IRQn_handler
  .word CLK_TRIM_IRQn_handler

  .weak NMI_IRQn_handler
  .thumb_set NMI_IRQn_handler,default_interrupt_handler
  .weak HardFault_IRQn_handler
  .thumb_set HardFault_IRQn_handler,default_interrupt_handler
  .weak SVC_IRQn_handler
  .thumb_set SVC_IRQn_handler,default_interrupt_handler
  .weak PendSV_IRQn_handler
  .thumb_set PendSV_IRQn_handler,default_interrupt_handler
  .weak SysTick_IRQn_handler
  .thumb_set SysTick_IRQn_handler,default_interrupt_handler
  .weak PORTA_IRQn_handler
  .thumb_set PORTA_IRQn_handler,default_interrupt_handler
  .weak PORTB_IRQn_handler
  .thumb_set PORTB_IRQn_handler,default_interrupt_handler
  .weak PORTC_IRQn_handler
  .thumb_set PORTC_IRQn_handler,default_interrupt_handler
  .weak PORTD_IRQn_handler
  .thumb_set PORTD_IRQn_handler,default_interrupt_handler
  .weak DMAC_IRQn_handler
  .thumb_set DMAC_IRQn_handler,default_interrupt_handler
  .weak TIM3_IRQn_handler
  .thumb_set TIM3_IRQn_handler,default_interrupt_handler
  .weak UART0_IRQn_handler
  .thumb_set UART0_IRQn_handler,default_interrupt_handler
  .weak UART1_IRQn_handler
  .thumb_set UART1_IRQn_handler,default_interrupt_handler
  .weak LPUART0_IRQn_handler
  .thumb_set LPUART0_IRQn_handler,default_interrupt_handler
  .weak LPUART1_IRQn_handler
  .thumb_set LPUART1_IRQn_handler,default_interrupt_handler
  .weak SPI0_IRQn_handler
  .thumb_set SPI0_IRQn_handler,default_interrupt_handler
  .weak SPI1_IRQn_handler
  .thumb_set SPI1_IRQn_handler,default_interrupt_handler
  .weak I2C0_IRQn_handler
  .thumb_set I2C0_IRQn_handler,default_interrupt_handler
  .weak I2C1_IRQn_handler
  .thumb_set I2C1_IRQn_handler,default_interrupt_handler
  .weak TIM0_IRQn_handler
  .thumb_set TIM0_IRQn_handler,default_interrupt_handler
  .weak TIM1_IRQn_handler
  .thumb_set TIM1_IRQn_handler,default_interrupt_handler
  .weak TIM2_IRQn_handler
  .thumb_set TIM2_IRQn_handler,default_interrupt_handler
  .weak LPTIM_IRQn_handler
  .thumb_set LPTIM_IRQn_handler,default_interrupt_handler
  .weak TIM4_IRQn_handler
  .thumb_set TIM4_IRQn_handler,default_interrupt_handler
  .weak TIM5_IRQn_handler
  .thumb_set TIM5_IRQn_handler,default_interrupt_handler
  .weak TIM6_IRQn_handler
  .thumb_set TIM6_IRQn_handler,default_interrupt_handler
  .weak PCA_IRQn_handler
  .thumb_set PCA_IRQn_handler,default_interrupt_handler
  .weak WDT_IRQn_handler
  .thumb_set WDT_IRQn_handler,default_interrupt_handler
  .weak RTC_IRQn_handler
  .thumb_set RTC_IRQn_handler,default_interrupt_handler
  .weak ADC_IRQn_handler
  .thumb_set ADC_IRQn_handler,default_interrupt_handler
  .weak PCNT_IRQn_handler
  .thumb_set PCNT_IRQn_handler,default_interrupt_handler
  .weak VC0_IRQn_handler
  .thumb_set VC0_IRQn_handler,default_interrupt_handler
  .weak VC1_IRQn_handler
  .thumb_set VC1_IRQn_handler,default_interrupt_handler
  .weak LVD_IRQn_handler
  .thumb_set LVD_IRQn_handler,default_interrupt_handler
  .weak FLASH_RAM_IRQn_handler
  .thumb_set FLASH_RAM_IRQn_handler,default_interrupt_handler
  .weak CLK_TRIM_IRQn_handler
  .thumb_set CLK_TRIM_IRQn_handler,default_interrupt_handler
.size vtable, .-vtable

.section .text.default_interrupt_handler,"ax",%progbits
default_interrupt_handler:
  default_interrupt_loop:
    B default_interrupt_loop
.size default_interrupt_handler, .-default_interrupt_handler
