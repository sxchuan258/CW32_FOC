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
	ADC_Configuration();
	
	SENSORLESS_TIM_Config();
	GTIM_Init();
	BLDC_Configuration();

	NVIC_SetPriority(ADC_IRQn ,0);
	NVIC_SetPriority(ATIM_IRQn ,1);
	NVIC_SetPriority(BTIM3_IRQn ,2);
	
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

