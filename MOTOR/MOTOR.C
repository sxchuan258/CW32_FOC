#include "motor.h"
#include "global.h"
#include "sensorless.h"
#include "main.h"
#include "Mat.h"

Motor_Type 				 Motor;
sType_SvmOut 			 gFocSvm = {16384, 16384, 16384};
sType_ActPwms 		 gActPwmsOut;
extern System_Type System;
extern uint16_t 	 SampleData[5]; 

unsigned int Speed_Remap(unsigned int Aim ,unsigned int Now_Value ,unsigned char Step_Value)
{
	if(Now_Value < Aim)
	{
		Now_Value = Now_Value + Step_Value;
	}
	else if(Now_Value > Aim)
	{
		Now_Value = Now_Value - Step_Value;
	}
	else
	{
		
	}
	return Now_Value;
}

void Motor_SpeedLoopRun(void)
{
	switch(Motor.CurrentLoopRunState)
	{
		case STOP_STATE:
			/* 进入初始停止状态 */
			Motor.Phase		 						= 0;
			Motor.RotCurrRef.Real 		= 0;
			Motor.RotCurrRef.Imag 		= 0;
			Motor.CurrentLoopRunState = ORIENT_INIT_STATE;
		break;
		
	  case ORIENT_INIT_STATE:
			/* 进入预定位初始化状态 */
			Motor.RotCurrRef.Real 		= 100;
			Motor.RotCurrRef.Imag 		= 0;
			Motor.CurrentLoopRunState = ORIENT_END_STATE;
		break;
		
		case ORIENT_END_STATE:	//进入预定位结束状态
			System.Orient_Time++;
		
			if(System.Orient_Time > 500)
			{
				/* 进行参数初始化 */
				Motor.SpeedRef						 = 0;
				Motor.SpdToAngAcc 				 = 32768.0 * 32768.0 / 30.0 / (1.0*PWM_FREQUENCY) * MotorPolePair;
				Motor.CurrentLoopRunState  = FORCE_RUN_STATE;
				Motor.SpeedLoopRUNState		 = VF_MODE;
				Motor.InvParam.KdcFactorVp = ((1L<<16)/1.732050807568877); // 1/根号3的16位标幺值
			}
		break;
			
		case FORCE_RUN_STATE:
			Motor.SpeedRef = Speed_Remap(500 ,Motor.SpeedRef ,1);
			if(Motor.SpeedRef >= 500)
			{
				//Motor.CurrentLoopRunState = SENSORLESS_STATE;
				//Motor.SpeedLoopRUNState		= SPEED_MODE;
			}
			else
			{
				/* SpdToAngAcc作为Q31格式与转速实际值相乘并右移15位，得到当前转速下，一个内环周期内转动的电角度AngleAccStep(Q16格式) */
				Motor.AngleAccStep = __SSAT(Mat_FixMulScale(Motor.SpeedRef, Motor.SpdToAngAcc, 0), MAT_FIX_SAT);
			}
		break;
		
		case SENSORLESS_STATE:
			/* 执行外环PI控制器得出Id_ref、Iq_ref */
			switch(Motor.SpeedLoopRUNState)
			{
				case SPEED_MODE:
					/* 速度环控制，根据速度误差执行外环PI，得出Id、Iq */
				break;
				
				case POWER_MODE:
					/* 功率环控制，根据功率误差执行外环PI，得出Id、Iq */
				break;
			}
		break;
	}
}

void Motor_CurrentLoopRun(void)
{
	switch(Motor.CurrentLoopRunState)
	{
		case ORIENT_INIT_STATE:
			/* 初始化状态内环参数，例如观测器锁相环等 */
		break;
		
		case FORCE_RUN_STATE:		//进入VF强拖运行状态
			//Motor.Angle = Motor.Angle + Motor.AngleAccStep;
			Motor.Angle = 7281;
		break;
		
		case SENSORLESS_STATE: //进入无感运行状态
			/* 通过观测器计算角度 */
		break;
		
		default:
			//电机停转
		break;
	}
	
	Motor.Vbus 			= SampleData[0] << 3; //Q15格式
	
	Motor.AngSinCos = GFLIB_SinCos_F16(Motor.Angle);
	Motor.StatCurr 	= GMCLIB_Clark_F16(Motor.PhaseCurr);
	Motor.RotCurr 	= GMCLIB_Park_F16(Motor.StatCurr, Motor.AngSinCos);
	
	switch(Motor.SpeedLoopRUNState)
	{
		case VF_MODE:
			Motor.RotVolt.Real = 1500;
			Motor.RotVolt.Imag = 0;
		break;
		
		default:
			/* 根据定子电流误差，执行PI控制器得出定子电压 */
		break;
	}
	
	Motor.StatVolt = GMCLIB_InvPark_F16(Motor.RotVolt, Motor.AngSinCos);
	GMC_ElimDcBusRip(Motor.Vbus, Motor.InvParam.KdcFactorVp, &Motor.Svm.Vs, &Motor.StatVolt);
	Motor.Svm.Sector = GMC_SvmStd(&gFocSvm, &Motor.Svm.Vs);
	
//	gActPwmsOut.comp60.High = gFocSvm.Ta * (PWM_PERIOD >> 1) >> 15;
//	gActPwmsOut.comp61.High = gFocSvm.Tb * (PWM_PERIOD >> 1) >> 15;
//	gActPwmsOut.comp62.High = gFocSvm.Tc * (PWM_PERIOD >> 1) >> 15;
	gActPwmsOut.comp60.High = 500;
	gActPwmsOut.comp61.High = 701;
	gActPwmsOut.comp62.High = 820;
	gActPwmsOut.comp60.Low	= gActPwmsOut.comp60.High;
	gActPwmsOut.comp61.Low	= gActPwmsOut.comp61.High;
	gActPwmsOut.comp62.Low	= gActPwmsOut.comp62.High;
	ATIM_SetCompare1(gActPwmsOut.comp60.High);
  ATIM_SetCompare2(gActPwmsOut.comp61.High);
	ATIM_SetCompare3(gActPwmsOut.comp62.High);
	
	ATIM_SetCompare4(gActPwmsOut.comp60.High+20);
	//ATIM_SetCompare5(gActPwmsOut.comp61.High+3);
	GTIM_SetCompare1(CW_GTIM1, 3000-gActPwmsOut.comp62.High+20);
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
