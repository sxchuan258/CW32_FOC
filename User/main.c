#include "main.h"
#include "init.h"
#include "global.h"
#include "motor.h"
#include "Fliter.h"
#include "Control.h"
#include "sensorless.h"
#include "Flash_Opration.h"

System_Type System;
extern Motor_Type Motor;
extern FLASH_Type Flash;

void PowerOnDelay(void)
{
	unsigned int i;
		
	for(i=0;i<60000;i++);
	for(i=0;i<60000;i++);
	for(i=0;i<60000;i++);
	for(i=0;i<60000;i++);
	for(i=0;i<60000;i++);
	for(i=0;i<60000;i++);
}

int main(void)
{			
	PowerOnDelay(); //延时，等系统电源稳定后，获取电流偏置
  RCC_Configuration();
	DPWMConfiguration();	
	RST_KEYConfiguration();
	BLDC_Configuration();
	SENSORLESS_TIM_Config(); //使能BTIM2,禁止BTIM3
	//GTIM_Init();
	ADC_Configuration();
	NVIC_SetPriority(BTIM3_IRQn ,0);
	NVIC_SetPriority(ADC_IRQn ,1);
	NVIC_SetPriority(GTIM1_IRQn ,2);
	
	if(*((uint8_t *)(SpeedWrite_PgaeBase*512)) == 0xff)
	{
		Flash_Init();
	}
	
  while (1)
	{
//		Flash_ReadOpration(Flash.SpeedWrite_AddrNow);
//		Flash_WriteOneByte(Flash.SpeedWrite_AddrNow);
	}
}

//void GTIM1_IRQHandler(void)
//{
//	if(GTIM_GetITStatus(CW_GTIM1, GTIM_IT_UPDATA))
//	{
//				
//		GTIM_ClearITPendingBit(CW_GTIM1, GTIM_IT_UPDATA);
//		
//		if(Motor.RunState == 4)
//		{
//			/* Motor.Speed_u32记录了一个电转速的运行时间，具体时间为BTIM1->CNT/BTIM1->Prescaler。
//				 所以，实际转速Motor.Speed = 60/Motor.Speed_u32/2 (RPM) */
//			Motor.Speed = 60*375000/Motor.Speed_u32/2;
//		
//			Motor.Speed = LowPass_Fliter(Motor.Speed);
//			
//			if(System.OpenLopp_RunTimes<100)
//			{
//				System.OpenLopp_RunTimes++;
//			}
//			else
//			{
//				Motor.Duty = PID_Control(Motor.Speed);
//			}
//		}
//	}
//}

void BTIM3_IRQHandler(void)
{
  if (BTIM_GetITStatus(CW_BTIM3, BTIM_IT_UPDATE))
  {
		BTIM_ClearITPendingBit(CW_BTIM3, BTIM_IT_UPDATE);
		
		Motor_SpeedClc(Motor.Phase ,CW_BTIM1->CNT);
		
		Motor.Phase = Motor_NewPhaseValueGet(Motor.Phase);
		Motor_ChangePhase(Motor.Phase);
		Motor.SensorlessState = 1;
		
		CW_BTIM1->CNT = 0;
		CW_BTIM3->CNT = 0;
		BTIM_Cmd(CW_BTIM3, DISABLE);
	}
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

