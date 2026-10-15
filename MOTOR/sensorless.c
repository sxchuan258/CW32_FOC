#include "main.h"
#include "global.h"
#include "sensorlessP.h"
#include "sensorless.h"
#include "motor.h"
#include "init.h"

void ATIM_IRQHandler(void)
{
	if(CW_ATIM->ISR_f.UIF)
	{
		ATIM_ClearITPendingBit(ATIM_IT_UIE);
		
		ADC_GetSqr0Result(&SampleData[0]); //母线电压
		ADC_GetSqr1Result(&SampleData[1]); //U/A相
		ADC_GetSqr2Result(&SampleData[2]); //V/B相
		ADC_GetSqr3Result(&SampleData[3]); //W/C相
	  ADC_GetSqr4Result(&SampleData[4]); //母线电流
		
		Motor_RunMachine();
	}
}
