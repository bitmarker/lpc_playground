/*
 * @brief FreeRTOS LPC43xx M0 core tick function using RITimer
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "chip.h"
#include "FreeRTOS.h"

/** @defgroup FreeRTOS_43XX_M0_TICK LPC43xx M0 core tick function using RITimer
 * @ingroup RTOS_FreeRTOS
 * @note	This driver adds support for the RITimer for the FreeRTOS tick on the LPC43xx.
 * The M0 core in LPC43XX series of controllers does not have internal systick timer,
 * so the RITIMER is used to generate the RTOS ticks.
 * @{
 */

 #if (defined(CHIP_LPC43XX) && defined(CORE_M0) && (configUSE_CUSTOM_TICK == 1))

 /*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define portNVIC_PENDSVSET          0x10000000
#define portNVIC_INT_CTRL           ((volatile unsigned long *) 0xe000ed04)
#define portMIN_INTERRUPT_PRIORITY  (255UL)
#define portNVIC_SYSTICK_PRI        portMIN_INTERRUPT_PRIORITY

#define RITENCLR               (1 << 1)
#define RITINT                 (1 << 0)

/* Timer reload value for next tick */
static uint32_t reload_val;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* FreeRTOS tick update function */
extern void vTaskIncrementTick(void);

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Tick interrupt handler routine
 * @return	Nothing
 * @note	This function handles the tick interrupts that are generated by RITIMER.
 */
void xPortSysTickHandler(void)
{
	unsigned long ulDummy;

	/* TODO: check if WWDT interrupt and redirect */
	Chip_RIT_ClearInt(LPC_RITIMER);
	Chip_RIT_SetCOMPVAL(LPC_RITIMER, Chip_RIT_GetCounter(LPC_RITIMER) + reload_val);/* Reload value */

#if configUSE_PREEMPTION == 1
	/* If using preemption, also force a context switch. */
	*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
#endif

	ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		vTaskIncrementTick();
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR(ulDummy);
}

/**
 * @brief	Initialize tick interrupts
 * @return	Nothing
 * @note	This function initializes the tick interrupts, and must
 * be called before starting the freeRTOS scheduler.
 */
void prvSetupTimerInterrupt(void)
{
	/* Clear any pending interrupt */
	Chip_RIT_ClearInt(LPC_RITIMER);

	/* Calculate reload value */
	reload_val = (configCPU_CLOCK_HZ / configTICK_RATE_HZ);
	Chip_RIT_SetCOMPVAL(LPC_RITIMER, Chip_RIT_GetCounter(LPC_RITIMER) + reload_val);/* Start tick */

	/* Set the priority and enable the interrupt */
	NVIC_SetPriority((IRQn_Type) RITIMER_IRQn, portNVIC_SYSTICK_PRI);
	NVIC_EnableIRQ((IRQn_Type) RITIMER_IRQn);
}

#endif

/**
 * @}
 */
