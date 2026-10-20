#include "main.h"
#include "global.h"
#include "sensorlessP.h"
#include "sensorless.h"
#include "motor.h"
#include "init.h"

extern System_Type System;

void ATIM_IRQHandler(void)
{
	if(CW_ATIM->ISR_f.UIF)
	{
		ATIM_ClearITPendingBit(ATIM_IT_UIE);
		
		ADC_GetSqr0Result(&SampleData[0]); //母线电压
	  ADC_GetSqr1Result(&SampleData[4]); //母线电流
		
		Motor_CurrentLoopRun();
	}
	
	if(CW_ATIM->ISR_f.CC4IF)
	{
		ATIM_ClearITPendingBit(ATIM_IT_CC4IE);
		
		if(System.OverTurn_Flag == 0)
		{
			System.OverTurn_Flag = 1;
			LEDON;
		}
		else
		{
			System.OverTurn_Flag = 0;
			LEDOFF;
		}
	}
}

void GTIM1_IRQHandler(void)
{
	if(GTIM_GetITStatus(CW_GTIM1, GTIM_IT_UPDATA))
	{			
		GTIM_ClearITPendingBit(CW_GTIM1, GTIM_IT_UPDATA);
		
		Motor_SpeedLoopRun();
	}
}
