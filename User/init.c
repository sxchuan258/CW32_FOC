#include "init.h"
#include "global.h" 
#include "main.h"

void RCC_Configuration(void)
{
	CW_SYSCTRL->AHBEN_f.FLASH = 1;
  CW_FLASH->CR2 = 0x5A5A0000 | 0x00000001;
	SYSCTRL_HSI_Enable(SYSCTRL_HSIOSC_DIV1); //HSIOSC = 48MHz; HSI = 48MHz;
	SYSCTRL_SYSCLKSRC_Config(SYSCTRL_SYSCLKSRC_HSI);//配置时钟源，高速内部时钟
	SYSCTRL_HCLKPRS_Config(SYSCTRL_HCLK_DIV1);//配置SysClk 到 HCLK的分频系数
  SYSCTRL_PCLKPRS_Config(SYSCTRL_PCLK_DIV1);//配置HCLK 到 PCLK的分频系数
	//实际系统频率48MHz
}

void DPWMConfiguration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__SYSCTRL_GPIOA_CLK_ENABLE(); 
	
	GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Pins = LED_GPIO_PIN|GPIO_PIN_0;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
   
	GPIO_WritePin(CW_GPIOA,GPIO_PIN_0,GPIO_Pin_RESET);
}


void RST_KEYConfiguration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__SYSCTRL_GPIOB_CLK_ENABLE(); 
	GPIO_RST2GPIO();

	GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIO_InitStruct.Pins = DR_GPIO_PIN;
	GPIO_Init(DR_GPIO_PORT, &GPIO_InitStruct);  
}

void ADC_Configuration(void)
{
	__SYSCTRL_GPIOA_CLK_ENABLE();
  __SYSCTRL_GPIOB_CLK_ENABLE();
	
	PA02_ANALOG_ENABLE();   //PA02,AIN2 母线电压
	PB02_ANALOG_ENABLE();   //PB02,AIN9 电流采样
	
	PB04_ANALOG_ENABLE();   //PB04,AIN11 W相反电动势
	PB05_ANALOG_ENABLE();   //PB05,AIN12 V相反电动势
	PB06_ANALOG_ENABLE();   //PB06,AIN13 U相反电动势
	
	__SYSCTRL_ADC_CLK_ENABLE();
  ADC_InitTypeDef ADC_InitStructure = {0};

	ADC_InitStructure.ADC_ClkDiv = ADC_Clk_Div2;
  ADC_InitStructure.ADC_ConvertMode = ADC_ConvertMode_Once;    // 单次多通道采样
  ADC_InitStructure.ADC_SQREns = ADC_SqrEns0to1;               // 6通道
  ADC_InitStructure.ADC_IN0.ADC_InputChannel = ADC_InputCH2;
  ADC_InitStructure.ADC_IN0.ADC_SampTime = ADC_SampTime9Clk;
	ADC_InitStructure.ADC_IN1.ADC_InputChannel = ADC_InputCH9;
  ADC_InitStructure.ADC_IN1.ADC_SampTime = ADC_SampTime9Clk;
	ADC_Init(&ADC_InitStructure);
	
	ADC_ClearITPendingAll();
  ADC_ITConfig(ADC_IT_EOS, ENABLE);
  NVIC_EnableIRQ(ADC_IRQn);
	ADC_ExtTrigCfg(ADC_TRIG_ATIMCC4 | ADC_TRIG_GTIM1CC1, ENABLE);

  //ADC使能
  ADC_Enable();
}

void BLDC_Configuration(void)
{
  __SYSCTRL_ATIM_CLK_ENABLE();
	__SYSCTRL_GPIOA_CLK_ENABLE();
  __SYSCTRL_GPIOB_CLK_ENABLE();
	
	__disable_irq();
  NVIC_EnableIRQ(ATIM_IRQn);
  __enable_irq();
	
  /* 上桥引脚设置，根据原理图，上桥三个引脚分别为：CH(PB0)、BH(PB1)、AH(PA3)；下桥三个引脚为：
		 CL(PA4)、BH(PA5)、AH(PA6)；并且A、B、C分别对应ATIM的CH3、CH2、CH1。*/
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	//完成PB0、PB1引脚配置
  GPIO_InitStruct.Pins = PWM_CP_PIN | PWM_BP_PIN; 
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
	//完成PA3、PA4、PA5、PA6引脚配置
  GPIO_InitStruct.Pins = PWM_AP_PIN | PWM_AN_PIN  | PWM_BN_PIN | PWM_CN_PIN;
  GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
	
	/* 对六个引脚进行复用设置，以作为定时器ATIM的PWM输出引脚 */
	PB00_AFx_ATIMCH1();
	PB01_AFx_ATIMCH2();
	PA03_AFx_ATIMCH3();
	PA04_AFx_ATIMCH1N();
	PA05_AFx_ATIMCH2N();
	PA06_AFx_ATIMCH3N();
	
	ATIM_InitTypeDef ATIM_InitStruct;
  ATIM_OCInitTypeDef ATIM_OCInitStruct = {0};
  ATIM_InitStruct.BufferState 			 = ENABLE;               						 //使能缓存寄存器    
  ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_ALIGN_MODE_CENTER_UP;  //边沿对齐
  ATIM_InitStruct.CounterDirection 	 = ATIM_COUNTING_UP;        				 //向上计数；
  ATIM_InitStruct.CounterOPMode 		 = ATIM_OP_MODE_REPETITIVE;    			 //连续运行模式       
  ATIM_InitStruct.Prescaler 				 = 1 - 1;                          	 //计算时钟 48 MHz
  ATIM_InitStruct.ReloadValue 			 = (PWM_PERIOD >> 1) - 1;             //PWM_TS = 2999(16kHz)
  ATIM_InitStruct.RepetitionCounter  = 0;
  ATIM_Init(&ATIM_InitStruct);
	
	ATIM_OCInitStruct.BufferState = ENABLE;
  ATIM_OCInitStruct.OCComplement = ENABLE;
  ATIM_OCInitStruct.OCFastMode = DISABLE;
  ATIM_OCInitStruct.OCInterruptState = DISABLE;
  ATIM_OCInitStruct.OCMode = ATIM_OCMODE_PWM2;    
  ATIM_OCInitStruct.OCPolarity = ATIM_OCPOLARITY_INVERT;
  ATIM_OC1Init(&ATIM_OCInitStruct);
	ATIM_OC2Init(&ATIM_OCInitStruct);
	ATIM_OC3Init(&ATIM_OCInitStruct);
	
	ATIM_OCInitStruct.OCComplement = DISABLE;
	ATIM_OC4Init(&ATIM_OCInitStruct);
	ATIM_OC5Init(&ATIM_OCInitStruct);
	
//	ATIM_SetCompare1(950);
//  ATIM_SetCompare2(1100);
//	ATIM_SetCompare3(1250);
//	//ATIM_SetCompare4(1140); 
	
	ATIM_CH1Config(ENABLE);
	ATIM_CH2Config(ENABLE);
	ATIM_CH3Config(ENABLE);
	ATIM_CH4Config(ENABLE);
	ATIM_CH5Config(ENABLE);
  ATIM_CtrlPWMOutputs(ENABLE);
	
	ATIM_ClearITPendingBit(ATIM_IT_UIE);
	ATIM_ITConfig(ATIM_IT_UIE ,ENABLE);
	
  ATIM_Cmd(ENABLE);    
}

void SENSORLESS_TIM_Config(void)
{
	/***************************初始化BTIM1、BTIM2和BTIM3************************/
  __SYSCTRL_BTIM123_CLK_ENABLE();
	BTIM_TimeBaseInitTypeDef BTIM_InitStruct = {0};
	__disable_irq(); 
  NVIC_EnableIRQ(BTIM3_IRQn); 
  __enable_irq();
	
	BTIM_InitStruct.BTIM_Mode = BTIM_MODE_TIMER;
	BTIM_InitStruct.BTIM_CountMode = BTIM_COUNT_MODE_REPETITIVE;
  BTIM_InitStruct.BTIM_Period = 65530;
  BTIM_InitStruct.BTIM_Prescaler = 128 - 1;   //8MHz
	
	/* BTIM1用来触发第一次ADC采样 */
	BTIM_TimeBaseInit(CW_BTIM1, &BTIM_InitStruct);
	/* BTIM2用来触发第二次ADC采样*/
  BTIM_TimeBaseInit(CW_BTIM2, &BTIM_InitStruct);
	/* BTIM2用来计算30°电角度周期 */
  BTIM_TimeBaseInit(CW_BTIM3, &BTIM_InitStruct);
	
	BTIM_Cmd(CW_BTIM1, ENABLE);
  BTIM_Cmd(CW_BTIM2, ENABLE);
	BTIM_Cmd(CW_BTIM3, DISABLE);
  BTIM_ITConfig(CW_BTIM3, BTIM_IT_UPDATE, ENABLE);
}

void GTIM_Init(void)
{
	__SYSCTRL_GTIM1_CLK_ENABLE();
	
	__disable_irq();
  NVIC_EnableIRQ(GTIM1_IRQn);
  __enable_irq();
	
	GTIM_InitTypeDef GTIM_InitStruct = {0};
	GTIM_OCModeCfgTypeDef GTIM_OCModeCfg;
	
	GTIM_InitStruct.ARRBuffState = GTIM_ARR_BUFF_EN;
	GTIM_InitStruct.Direction = GTIM_DIRECTION_UP;
	GTIM_InitStruct.PulseMode = GTIM_PULSE_MODE_DIS;
	GTIM_InitStruct.EventOption = GTIM_EVENT_NORMAL;
	GTIM_InitStruct.UpdateOption = GTIM_UPDATE_EN;
	GTIM_InitStruct.Prescaler = 1 - 1;
	GTIM_InitStruct.ReloadValue = 48000;
	GTIM_TimeBaseInit(CW_GTIM1, &GTIM_InitStruct);
	
	GTIM_OCModeCfg.FastMode = DISABLE;
	GTIM_OCModeCfg.OCMode		= GTIM_OC_MODE_PWM1;
	GTIM_OCModeCfg.OCPolarity = GTIM_OC_POLAR_NONINVERT;
	GTIM_OCModeCfg.PreloadState = ENABLE;
	GTIM_OC1ModeCfg(CW_GTIM1, &GTIM_OCModeCfg);
	
	GTIM_OC1Cmd(CW_GTIM1, ENABLE);
	
	GTIM_ClearITPendingBit(CW_GTIM1, GTIM_IT_UPDATA);
	GTIM_ITConfig(CW_GTIM1, GTIM_IT_UPDATA, ENABLE);
	GTIM_Cmd(CW_GTIM1, ENABLE);
}
