#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "main.h"

//LED灯
#define LED_GPIO_PIN        GPIO_PIN_1                  
#define LED_GPIO_PORT      	CW_GPIOA            
#define LEDON	  GPIO_WritePin(LED_GPIO_PORT,LED_GPIO_PIN,GPIO_Pin_SET)
#define LEDOFF  GPIO_WritePin(LED_GPIO_PORT,LED_GPIO_PIN,GPIO_Pin_RESET)

//方向控制端口定义
#define DR_GPIO_PIN        	GPIO_PIN_7                  
#define DR_GPIO_PORT      	CW_GPIOB               
            
#define PWM_AP_PORT									(CW_GPIOA)
#define PWM_AP_PIN									(GPIO_PIN_3)
#define PWM_AN_PORT									(CW_GPIOA)
#define PWM_AN_PIN									(GPIO_PIN_6)

#define PWM_BP_PORT									(CW_GPIOB)
#define PWM_BP_PIN									(GPIO_PIN_1)
#define PWM_BN_PORT									(CW_GPIOA)
#define PWM_BN_PIN									(GPIO_PIN_5)

#define PWM_CP_PORT									(CW_GPIOB)
#define PWM_CP_PIN									(GPIO_PIN_0)
#define PWM_CN_PORT									(CW_GPIOA)
#define PWM_CN_PIN									(GPIO_PIN_4)


#define PWM_AH_OFF GPIO_WritePin(PWM_AP_PORT,PWM_AP_PIN,GPIO_Pin_RESET) 
#define PWM_BH_OFF GPIO_WritePin(PWM_BP_PORT,PWM_BP_PIN,GPIO_Pin_RESET)
#define PWM_CH_OFF GPIO_WritePin(PWM_CP_PORT,PWM_CP_PIN,GPIO_Pin_RESET)

#define PWM_AH_ON GPIO_WritePin(PWM_AP_PORT,PWM_AP_PIN,GPIO_Pin_SET) 
#define PWM_BH_ON GPIO_WritePin(PWM_BP_PORT,PWM_BP_PIN,GPIO_Pin_SET) 
#define PWM_CH_ON GPIO_WritePin(PWM_CP_PORT,PWM_CP_PIN,GPIO_Pin_SET) 

//上管调制，下管开关控制, 上高电平开关管导通
#define PWM_AL_OFF GPIO_WritePin(PWM_AN_PORT,PWM_AN_PIN,GPIO_Pin_RESET) 
#define PWM_BL_OFF GPIO_WritePin(PWM_BN_PORT,PWM_BN_PIN,GPIO_Pin_RESET) 
#define PWM_CL_OFF GPIO_WritePin(PWM_CN_PORT,PWM_CN_PIN,GPIO_Pin_RESET) 

#define PWM_AL_ON GPIO_WritePin(PWM_AN_PORT,PWM_AN_PIN,GPIO_Pin_SET)
#define PWM_BL_ON GPIO_WritePin(PWM_BN_PORT,PWM_BN_PIN,GPIO_Pin_SET)
#define PWM_CL_ON GPIO_WritePin(PWM_CN_PORT,PWM_CN_PIN,GPIO_Pin_SET)
              

//载波频率20k
#define PWM_PERIOD 1500
// (48000000 / 20000 )-1; 

#define OnepercentPWM (PWM_PERIOD+1)/100

#define STATESTARTCHECK 0
#define STATESTARTPID 1
#define STATERUNPID 2
#define STATESTOP 3
#define STATEBREAK 4
#define STATEERROR 5
#define STATEERROROVER 6
#define STATWAITSTART 7
#define STATESTARTDELAY 8
#define STATESTARTOPEN 9
#define STATERUNOPEN 10

#define STOP_STATE 				(0)
#define ORIENT_INIT_STATE (1)
#define ORIENT_END_STATE	(2)
#define FORCE_RUN_STATE		(3)
#define SENSORLESS_STATE	(4)

#define DUTY_MIN (450)
#define DUTY_MAX (500)
#define TIME_MIN (75)
#define TIME_MAX (200)

#define PHASE_N_FACTOR      (5242) //升频升压阶段 换相系数N  Vbus*N%
#define BEMF_ZERO_FACTOR    (30000) //理论值为50% 实际情况可以灵活调整

extern uint16_t SampleData[5];

typedef struct
{
	unsigned char RunState;
	unsigned char Phase;
	unsigned char DemagnetizationTimeCnt;
	unsigned char ForceState;
	unsigned char SensorlessState;
	unsigned char Slope;
	unsigned int 	DetectBEMF_Times;
	unsigned int  ForceRunTimeCnt;
	unsigned int  ForceRunTimeBase;
	unsigned int  Duty;
	unsigned int  TerminalVoltage;
	unsigned int  TerminalVoltageMax; //端电压最大阈值
  unsigned int  TerminalVoltageMin; //端电压最小阈值
  unsigned int  VbusZcp;//1/2Vbus反电动势过零比较值
	unsigned int  Vbus;
	unsigned int  CeventTime;
	unsigned int  SixTyRunTime[6]; //每个相序的运行时间
	unsigned int  Speed_u32;
	unsigned int  Speed_u32_1_6;
	unsigned int  Speed;
//	double error;
//	double errorsum;
}Motor_Type;

typedef struct
{
	unsigned char OverTurn_Flag;
	unsigned char SpeedClcDown_FristFlag;
	unsigned int  Orient_Time;
	unsigned char OpenLopp_RunTimes;
}System_Type;

#define Read_Mode 		 0
#define Program_Mode 	 1
#define PageErase_Mpde 2
#define ChipErase_Mpde 3

#define SpeedWrite_PgaeBase 120
#define SpeedWrite_PgaeLong 8

typedef struct
{
	unsigned char RunMode;
	unsigned char Flash_Read[4];
	unsigned int 	SpeedWrite_AddrNow;
}FLASH_Type;

#endif
