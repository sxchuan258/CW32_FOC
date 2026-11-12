#include "main.h"
#include "global.h"
#include "sensorlessP.h"
#include "sensorless.h"
#include "motor.h"
#include "init.h"
#include "CurSen.h"

extern Motor_Type  	 Motor;
extern System_Type   System;
extern sType_ActPwms gActPwmsOut;
unsigned short Ibus_AdcSample1;
unsigned short Ibus_AdcSample2;

void ADC_IRQHandler(void)
{
	/* USER CODE BEGIN */
  if (CW_ADC->ISR_f.EOS)
  {
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
		
		if(CW_GTIM1->CNT_f.CNT < gActPwmsOut.ADC_Trig2Cnt)
		{
			ADC_GetSqr1Result(&Ibus_AdcSample1);
		}
		else
		{
			ADC_GetSqr0Result(&SampleData[0]);
			ADC_GetSqr1Result(&Ibus_AdcSample2);
		}
		
		ADC_ClearITPendingAll();//清除AD所有中断标志
		//ADC_ITConfig(ADC_IT_EOS, DISABLE);
	}
}

void ATIM_IRQHandler(void)
{
	if(CW_ATIM->ISR_f.UIF)
	{
		ATIM_ClearITPendingBit(ATIM_IT_UIE);
		GTIM_ClearITPendingBit(CW_GTIM1, GTIM_IT_CC1);
		GTIM_ClearITPendingBit(CW_GTIM1, GTIM_IT_CC2);
		
//		ADC_GetSqr0Result(&SampleData[0]); //母线电压
//	  ADC_GetSqr1Result(&Ibus_AdcSample2); //母线电流
		Motor.PhaseCurr = CurSen_CurrentReconstruction(Motor.PhaseCurr);
		Motor_CurrentLoopRun();
		
//		ADC_ClearITPendingAll();
//		ADC_ITConfig(ADC_IT_EOS, ENABLE);
	}
}

void BTIM3_IRQHandler(void)
{
  if (BTIM_GetITStatus(CW_BTIM3, BTIM_IT_UPDATE))
  {
		BTIM_ClearITPendingBit(CW_BTIM3, BTIM_IT_UPDATE);
		
		Motor_SpeedLoopRun();
	}
}
