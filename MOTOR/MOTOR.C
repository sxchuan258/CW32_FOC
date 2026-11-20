#include "motor.h"
#include "global.h"
#include "sensorless.h"
#include "main.h"
#include "Mat.h"
#include "CurSen.h"
#include "Control.h"
#include "Observer.h"

Motor_Type 				 Motor;
sType_SvmOut 			 gFocSvm = {16384, 16384, 16384};
sType_ActPwms 		 gActPwmsOut;
PI_Type						 RealCurrPI;
PI_Type						 ImagCurrPI;
SMO_Type					 Smo;
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
			Motor.RotCurrRef.Real 		= 250;
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
				
				RealCurrPI.Kp = 0.02   * (1 << 15);
				RealCurrPI.Ki = 0.0008 * (1 << 15);
				RealCurrPI.PIMax = 2000;
				RealCurrPI.PIMin = 0;
				
				ImagCurrPI.Kp = RealCurrPI.Kp;
				ImagCurrPI.Ki = RealCurrPI.Ki;
				ImagCurrPI.PIMax = RealCurrPI.PIMax;
				ImagCurrPI.PIMin = RealCurrPI.PIMin;
				
				Smo_ObserverInit(&Smo);
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
			CurSen_PwmPhaseWalk_Init(&gActPwmsOut);
		break;
		
		case FORCE_RUN_STATE:		//进入VF强拖运行状态
			Motor.Angle = Motor.Angle + Motor.AngleAccStep;
			Smo_Observer(&Motor.StatCurr ,&Motor.StatVolt ,&Smo);

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
			Motor.RotVolt.Real = PID_Control(Motor.RotCurr.Real ,Motor.RotCurrRef.Real ,&RealCurrPI);
			Motor.RotVolt.Imag = PID_Control(Motor.RotCurr.Imag ,Motor.RotCurrRef.Imag ,&ImagCurrPI);
		break;
		
		default:
			/* 根据定子电流误差，执行PI控制器得出定子电压 */
		break;
	}
	
	Motor.StatVolt = GMCLIB_InvPark_F16(Motor.RotVolt, Motor.AngSinCos);
	GMC_ElimDcBusRip(Motor.Vbus, Motor.InvParam.KdcFactorVp, &Motor.Svm.Vs, &Motor.StatVolt);
	Motor.Svm.Sector = GMC_SvmStd(&gFocSvm, &Motor.Svm.Vs);
	
	CurSen_PwmPhaseWalk_Run(&gActPwmsOut ,&gFocSvm);
	Motor_PWMOUT();

	GTIM_SetCompare1(CW_GTIM1, gActPwmsOut.ADC_Trig1Cnt);
	GTIM_SetCompare2(CW_GTIM1, gActPwmsOut.ADC_Trig2Cnt);
}

void Motor_PWMOUT(void)
{
	switch(Motor.Svm.Sector)
	{
		case 1:
			CW_ATIM->CCR6 &= 0x03FFFFFF;
			CW_ATIM->CCR6_f.GC6C2 = 1;
			
			ATIM_SetCompare1(gActPwmsOut.comp60.High);
			ATIM_SetCompare2(gActPwmsOut.comp61.High);
			ATIM_SetCompare3(gActPwmsOut.comp62.High);
			ATIM_SetCompare6(gActPwmsOut.comp61.Low);
		break;
		
		case 2:
			CW_ATIM->CCR6 &= 0x03FFFFFF;
			CW_ATIM->CCR6_f.GC6C1 = 1;
			
			ATIM_SetCompare1(gActPwmsOut.comp60.High);
			ATIM_SetCompare2(gActPwmsOut.comp61.High);
			ATIM_SetCompare3(gActPwmsOut.comp62.High);
			ATIM_SetCompare6(gActPwmsOut.comp60.Low);
		break;
		
		case 3:
			CW_ATIM->CCR6 &= 0x03FFFFFF;
			CW_ATIM->CCR6_f.GC6C3 = 1;
			
			ATIM_SetCompare1(gActPwmsOut.comp60.High);
			ATIM_SetCompare2(gActPwmsOut.comp61.High);
			ATIM_SetCompare3(gActPwmsOut.comp62.High);
			ATIM_SetCompare6(gActPwmsOut.comp62.Low);
		break;
		
		case 4:
			CW_ATIM->CCR6 &= 0x03FFFFFF;
			CW_ATIM->CCR6_f.GC6C2 = 1;
			
			ATIM_SetCompare1(gActPwmsOut.comp60.High);
			ATIM_SetCompare2(gActPwmsOut.comp61.High);
			ATIM_SetCompare3(gActPwmsOut.comp62.High);
			ATIM_SetCompare6(gActPwmsOut.comp61.Low);
		break;
		
		case 5:
			CW_ATIM->CCR6 &= 0x03FFFFFF;
			CW_ATIM->CCR6_f.GC6C1 = 1;
			
			ATIM_SetCompare1(gActPwmsOut.comp60.High);
			ATIM_SetCompare2(gActPwmsOut.comp61.High);
			ATIM_SetCompare3(gActPwmsOut.comp62.High);
			ATIM_SetCompare6(gActPwmsOut.comp60.Low);
		break;
		
		case 6:
			CW_ATIM->CCR6 &= 0x03FFFFFF;
			CW_ATIM->CCR6_f.GC6C3 = 1;
			
			ATIM_SetCompare1(gActPwmsOut.comp60.High);
			ATIM_SetCompare2(gActPwmsOut.comp61.High);
			ATIM_SetCompare3(gActPwmsOut.comp62.High);
			ATIM_SetCompare6(gActPwmsOut.comp62.Low);
		break;
	}
}
