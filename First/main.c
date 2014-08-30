#include "chip.h"
#include <stdio.h>

/* System oscillator rate and clock rate on the CLKIN pin */
const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

#define TICKRATE_HZ1 (10) /* 10 ticks per second */
#define TICKRATE_HZ2 (1) /* 11 ticks per second */
  
volatile int i;


  
void TIMER32_0_IRQHandler(void)
{
  if (Chip_TIMER_MatchPending(LPC_TIMER32_0, 1))
  {
    Chip_TIMER_ClearMatch(LPC_TIMER32_0, 1);
    //Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, false);
  }
}

void SysTick_Handler(void)
{
        
  //Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, true);
}

int main()
{
  uint32_t timerFreq;
  
  SystemCoreClockUpdate();
  
  
  Chip_GPIO_Init(LPC_GPIO);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 1, 10);
  Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, false);
  
  
  Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 7);
  
  
  
  
  // Enable and setup SysTick Timer at a periodic rate
  SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

  // Enable timer 1 clock
  Chip_TIMER_Init(LPC_TIMER32_0);
  
  // Timer rate is system clock rate
  timerFreq = Chip_Clock_GetSystemClockRate();

  // Timer setup for match and interrupt at TICKRATE_HZ
  Chip_TIMER_Reset(LPC_TIMER32_0);
  Chip_TIMER_MatchEnableInt(LPC_TIMER32_0, 1);
  Chip_TIMER_SetMatch(LPC_TIMER32_0, 1, (timerFreq / TICKRATE_HZ2));
  Chip_TIMER_ResetOnMatchEnable(LPC_TIMER32_0, 1);
  Chip_TIMER_Enable(LPC_TIMER32_0);

  // Enable timer interrupt
  NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
  NVIC_EnableIRQ(TIMER_32_0_IRQn);
  
  
  i = 0;

  Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_7, (IOCON_FUNC0 | IOCON_MODE_PULLUP | IOCON_DIGMODE_EN));
  Chip_GPIO_SetupPinInt(LPC_GPIO, 0, 7, GPIO_INT_FALLING_EDGE);
  Chip_GPIO_EnableInt(LPC_GPIO, 0, (1 << 7));
  NVIC_EnableIRQ(EINT0_IRQn);
  
  
  while(1)
  {

  }
    
  return 0;
}

void PIOINT0_IRQHandler(void)
{
  //Chip_GPIO_SetPinState(LPC_GPIO, 1, 10, i);

  /* Clear interrupt */
  Chip_GPIO_ClearInts(LPC_GPIO, 0, (1 << 7));
}
