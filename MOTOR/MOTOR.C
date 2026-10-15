#include "motor.h"
#include "global.h"
#include "sensorless.h"
#include "main.h"

Motor_Type Motor;
extern System_Type System;
extern uint16_t SampleData[5]; 

void Motor_RunMachine(void)
{
  signed short sTemp16;
  unsigned short Temp16;	
	
	switch(Motor.RunState)
	{
		case STOP_STATE:
			/* 进入初始停止状态 */
			//Motor.RunState = ORIENT_INIT_STATE;
			Motor.Phase		 = 0;
			Motor.Duty		 = DUTY_MIN;
		break;
		
		case ORIENT_INIT_STATE:
			/* 进入预定位初始化状态，定位到AB相 */
			Motor.Phase = Motor_NewPhaseValueGet(Motor.Phase);
			Motor_ChangePhase(Motor.Phase);
			Motor.RunState = ORIENT_END_STATE;
		break;
		
		case ORIENT_END_STATE:	//进入预定位结束状态
			System.Orient_Time++;
		
			if(System.Orient_Time > 500)
			{
				/* 进行强拖参数初始化 */
				Motor.Phase		 							 = 2;
				Motor.ForceState 						 = 1;
				Motor.DemagnetizationTimeCnt = 3;
				Motor.ForceRunTimeBase 			 = TIME_MAX;
				Motor.Duty 									 = DUTY_MIN;
				Motor.TerminalVoltageMax 		 = 0;
        Motor.TerminalVoltageMin 		 = 3708;

				Motor.RunState = FORCE_RUN_STATE;
			}
		break;
		
		case FORCE_RUN_STATE:		//进入VF强拖运行状态
			Motor.ForceRunTimeCnt++;
			switch(Motor.ForceState)
			{
			}
		break;
		
		case SENSORLESS_STATE: //进入无感运行状态
		break;
		
		default:
			//电机停转
		break;
	}
}

void TerminalVoltageGet(void)
{
	switch(Motor.Phase)
	{
		case 1:
			Motor.Slope = 0;
			Motor.TerminalVoltage = SampleData[3];
		break;
		
		case 2:
			Motor.Slope = 1;
			Motor.TerminalVoltage = SampleData[2];
		break;
		
		case 3:
			Motor.Slope = 0;
			Motor.TerminalVoltage = SampleData[1];
		break;
		
		case 4:
			Motor.Slope = 1;
			Motor.TerminalVoltage = SampleData[3];
		break;
		
		case 5:
			Motor.Slope = 0;
			Motor.TerminalVoltage = SampleData[2];
		break;
		
		case 6:
			Motor.Slope = 1;
			Motor.TerminalVoltage = SampleData[1];
		break;
	}
}

unsigned char Motor_NewPhaseValueGet(unsigned char OldPhaseValue)
{
	
	unsigned char NewPhaseValue;
	
	NewPhaseValue = OldPhaseValue+1;
	if(NewPhaseValue > 6)
	{
		NewPhaseValue=1;
	}
	
	return NewPhaseValue;
}

void Motor_ChangePhase(unsigned char NowPhaseValue)
{
	/* 当前相序为逆时针转动 */
	switch(NowPhaseValue)
	{
		case 1:
			//AB
			ATIM_SetCompare1(0);
			ATIM_SetCompare2(0);
			ATIM_SetCompare3(Motor.Duty);
			PWM_AL_OFF;
			PWM_BL_ON;
			PWM_CL_OFF;
		break;
		
		case 2:
			//AC
			ATIM_SetCompare1(0);
			ATIM_SetCompare2(0);
			ATIM_SetCompare3(Motor.Duty);
			PWM_AL_OFF;
			PWM_BL_OFF;
			PWM_CL_ON;
		break;
		
		case 3:
			//BC
			ATIM_SetCompare1(0);
			ATIM_SetCompare2(Motor.Duty);
			ATIM_SetCompare3(0);
			PWM_AL_OFF;
			PWM_BL_OFF;
			PWM_CL_ON;
		break;
		
		case 4:
			//BA
			ATIM_SetCompare1(0);
			ATIM_SetCompare2(Motor.Duty);
			ATIM_SetCompare3(0);
			PWM_AL_ON;
			PWM_BL_OFF;
			PWM_CL_OFF;
		break;
		
		case 5:
			//CA
			ATIM_SetCompare1(Motor.Duty);
			ATIM_SetCompare2(0);
			ATIM_SetCompare3(0);
			PWM_AL_ON;
			PWM_BL_OFF;
			PWM_CL_OFF;
		break;
		
		case 6:
			//CB
			ATIM_SetCompare1(Motor.Duty);
			ATIM_SetCompare2(0);
			ATIM_SetCompare3(0);
			PWM_AL_OFF;
			PWM_BL_ON;
			PWM_CL_OFF;
		break;
		
		default:
			//电机停转
		break;
	}
}

void Motor_SpeedClc(unsigned char NowPhaseValue ,unsigned int RunTime)
{
	switch(NowPhaseValue)
	{
		case 1:
			Motor.SixTyRunTime[0] = RunTime;
		break;
		
		case 2:
			Motor.SixTyRunTime[1] = RunTime;
		break;
		
		case 3:
			Motor.SixTyRunTime[2] = RunTime;
		break;
		
		case 4:
			Motor.SixTyRunTime[3] = RunTime;
		break;
		
		case 5:
			Motor.SixTyRunTime[4] = RunTime;
		break;
		
		case 6:
			Motor.SixTyRunTime[5] = RunTime;
			
			if((Motor.SixTyRunTime[0] != 0) & (Motor.SixTyRunTime[1] != 0) & (Motor.SixTyRunTime[2] != 0) & 
				 (Motor.SixTyRunTime[3] != 0) & (Motor.SixTyRunTime[4] != 0) & (Motor.SixTyRunTime[5] != 0))
			{
				Motor.Speed_u32 			= Motor.SixTyRunTime[0]+Motor.SixTyRunTime[1]+Motor.SixTyRunTime[2]+
																Motor.SixTyRunTime[3]+Motor.SixTyRunTime[4]+Motor.SixTyRunTime[5];
				Motor.Speed_u32_1_6 	= Motor.Speed_u32 / 6;
				
				if(System.SpeedClcDown_FristFlag != 1) System.SpeedClcDown_FristFlag = 1;
			}
		break;
	}
}

void Motor_ConventTimeClc(unsigned char NowPhaseValue ,unsigned int RunTime)
{
	switch(NowPhaseValue)
	{
		case 1:
			Motor.CeventTime = RunTime + Motor.Speed_u32_1_6 - Motor.SixTyRunTime[0];
		break;
		
		case 2:
			Motor.CeventTime = RunTime + Motor.Speed_u32_1_6 - Motor.SixTyRunTime[1];
		break;
		
		case 3:
			Motor.CeventTime = RunTime + Motor.Speed_u32_1_6 - Motor.SixTyRunTime[2];
		break;
		
		case 4:
			Motor.CeventTime = RunTime + Motor.Speed_u32_1_6 - Motor.SixTyRunTime[3];
		break;
		
		case 5:
			Motor.CeventTime = RunTime + Motor.Speed_u32_1_6 - Motor.SixTyRunTime[4];
		break;
		
		case 6:
			Motor.CeventTime = RunTime + Motor.Speed_u32_1_6 - Motor.SixTyRunTime[5];
		break;
	}
}

unsigned char DemagnetizationTimeSet(unsigned int SpeedNow)
{
	unsigned char DemagnetizationTime;
	
	if(SpeedNow < 1000)
	{
		DemagnetizationTime = 3;
	}
	else
	{
		DemagnetizationTime = 3+(SpeedNow-1000)/1000;
	}
	
	return DemagnetizationTime;
}
