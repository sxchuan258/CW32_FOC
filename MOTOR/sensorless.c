#include "main.h"
#include "global.h"
#include "sensorlessP.h"
#include "sensorless.h"
#include "motor.h"
#include "init.h"

extern System_Type System;
unsigned short Ibus_AdcSample1;
unsigned short Ibus_AdcSample2;

void ADC_IRQHandler(void)
{
	static unsigned char i = 1;
	/* USER CODE BEGIN */
  if (CW_ADC->ISR_f.EOS)
  {
    ADC_ClearITPendingAll();//清除AD所有中断标志
		
		if(i == 1)
		{
			ADC_GetSqr1Result(&Ibus_AdcSample1);
			i = 2;
		}
		else if(i == 2)
		{
			ADC_GetSqr0Result(&SampleData[0]);
			ADC_GetSqr1Result(&Ibus_AdcSample2);
			i = 1;
		}

		//ADC_ITConfig(ADC_IT_EOS, DISABLE);
	}
}

void ATIM_IRQHandler(void)
{
	if(CW_ATIM->ISR_f.UIF)
	{
		ATIM_ClearITPendingBit(ATIM_IT_UIE);
		
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
		
//		ADC_GetSqr0Result(&SampleData[0]); //母线电压
//	  ADC_GetSqr1Result(&Ibus_AdcSample2); //母线电流
		
		Motor_CurrentLoopRun();
		
//		ADC_ClearITPendingAll();
//		ADC_ITConfig(ADC_IT_EOS, ENABLE);
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
