#include "CurSen.h"

extern Motor_Type Motor;
extern signed short Ibus_AdcSample1;
extern signed short Ibus_AdcSample2;

sType_PhaseCurr CurSen_CurrentReconstruction(sType_PhaseCurr abc)
{
	switch(Motor.Svm.SectorOld)
	{
		case 1:
			abc.A = Ibus_AdcSample1;
			abc.C = -Ibus_AdcSample2;
			abc.B = -abc.A-abc.C;
		break;
		
		case 2:
			abc.B = Ibus_AdcSample1;
			abc.C = -Ibus_AdcSample2;
			abc.A = -abc.B-abc.C;
		break;
		
		case 3:
			abc.B = Ibus_AdcSample1;
			abc.A = -Ibus_AdcSample2;
			abc.C = -abc.B-abc.A;
		break;
		
		case 4:
			abc.C = Ibus_AdcSample1;
			abc.A = -Ibus_AdcSample2;
			abc.B = -abc.C-abc.A;
		break;
		
		case 5:
			abc.C = Ibus_AdcSample1;
			abc.B = -Ibus_AdcSample2;
			abc.A = -abc.C-abc.B;
		break;
		
		case 6:
			abc.A = Ibus_AdcSample1;
			abc.B = -Ibus_AdcSample2;
			abc.C = -abc.A-abc.B;
		break;
	}
	Motor.Svm.SectorOld = Motor.Svm.Sector;
	
	return abc;
}

void CurSen_PwmPhaseWalk_Init(sType_ActPwms *PwmOut)
{
	PwmOut->ShiftCntLimMin 		= 60;
	PwmOut->ShiftCntLimMin_x2 = PwmOut->ShiftCntLimMin * 2;
	PwmOut->ADC_TrigDelayCnt	= 30;
}

void CurSen_PwmPhaseWalk_Run(sType_ActPwms *PwmOut ,sType_SvmOut *Pwm)
{
	uint16_t T1;
	uint16_t T2;
	uint16_t Delta;
	
	PwmOut->comp60.High = Pwm->Ta * (PWM_PERIOD >> 1) >> 15;
	PwmOut->comp61.High = Pwm->Tb * (PWM_PERIOD >> 1) >> 15;
	PwmOut->comp62.High = Pwm->Tc * (PWM_PERIOD >> 1) >> 15;
	
	PwmOut->comp60.Low	= PwmOut->comp60.High;
	PwmOut->comp61.Low	= PwmOut->comp61.High;
	PwmOut->comp62.Low	= PwmOut->comp62.High;
	
	switch(Motor.Svm.Sector)
	{
		case 1:
			T1 = PwmOut->comp61.High - PwmOut->comp60.High;
			T2 = PwmOut->comp62.High - PwmOut->comp61.High;
			
			if((T1 + T2) < PwmOut->ShiftCntLimMin_x2)
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp60.High = PwmOut->comp60.High - Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	+ Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp62.High = PwmOut->comp62.High + Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	- Delta;
				}
			}
			else
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp61.High = PwmOut->comp61.High + Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	- Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp61.High = PwmOut->comp61.High - Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	+ Delta;
				}
			}
			
			PwmOut->ADC_Trig1Cnt = PwmOut->comp60.High + PwmOut->ADC_TrigDelayCnt;
			PwmOut->ADC_Trig2Cnt = PWM_PERIOD - PwmOut->comp62.Low + PwmOut->ADC_TrigDelayCnt;
		break;
		
		case 2:
			T1 = PwmOut->comp60.High - PwmOut->comp61.High;
			T2 = PwmOut->comp62.High - PwmOut->comp60.High;
			
			if((T1 + T2) < PwmOut->ShiftCntLimMin_x2)
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp61.High = PwmOut->comp61.High - Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	+ Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp62.High = PwmOut->comp62.High + Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	- Delta;
				}
			}
			else
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp60.High = PwmOut->comp60.High + Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	- Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp60.High = PwmOut->comp60.High - Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	+ Delta;
				}
			}
			
			PwmOut->ADC_Trig1Cnt = PwmOut->comp61.High + PwmOut->ADC_TrigDelayCnt;
			PwmOut->ADC_Trig2Cnt = PWM_PERIOD - PwmOut->comp62.Low + PwmOut->ADC_TrigDelayCnt;
		break;
		
		case 3:
			T1 = PwmOut->comp62.High - PwmOut->comp61.High;
			T2 = PwmOut->comp60.High - PwmOut->comp62.High;
			
			if((T1 + T2) < PwmOut->ShiftCntLimMin_x2)
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp61.High = PwmOut->comp61.High - Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	+ Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp60.High = PwmOut->comp60.High + Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	- Delta;
				}
			}
			else
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp62.High = PwmOut->comp62.High + Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	- Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp62.High = PwmOut->comp62.High - Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	+ Delta;
				}
			}
			
			PwmOut->ADC_Trig1Cnt = PwmOut->comp61.High + PwmOut->ADC_TrigDelayCnt;
			PwmOut->ADC_Trig2Cnt = PWM_PERIOD - PwmOut->comp60.Low + PwmOut->ADC_TrigDelayCnt;
		break;
				
		case 4:
			T1 = PwmOut->comp61.High - PwmOut->comp62.High;
			T2 = PwmOut->comp60.High - PwmOut->comp61.High;
			
			if((T1 + T2) < PwmOut->ShiftCntLimMin_x2)
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp62.High = PwmOut->comp62.High - Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	+ Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp60.High = PwmOut->comp60.High + Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	- Delta;
				}
			}
			else
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp61.High = PwmOut->comp61.High + Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	- Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp61.High = PwmOut->comp61.High - Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	+ Delta;
				}
			}
			
			PwmOut->ADC_Trig1Cnt = PwmOut->comp62.High + PwmOut->ADC_TrigDelayCnt;
			PwmOut->ADC_Trig2Cnt = PWM_PERIOD - PwmOut->comp60.Low + PwmOut->ADC_TrigDelayCnt;
		break;
						
		case 5:
			T1 = PwmOut->comp60.High - PwmOut->comp62.High;
			T2 = PwmOut->comp61.High - PwmOut->comp60.High;
			
			if((T1 + T2) < PwmOut->ShiftCntLimMin_x2)
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp62.High = PwmOut->comp62.High - Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	+ Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp61.High = PwmOut->comp61.High + Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	- Delta;
				}
			}
			else
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp60.High = PwmOut->comp60.High + Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	- Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp60.High = PwmOut->comp60.High - Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	+ Delta;
				}
			}
			
			PwmOut->ADC_Trig1Cnt = PwmOut->comp62.High + PwmOut->ADC_TrigDelayCnt;
			PwmOut->ADC_Trig2Cnt = PWM_PERIOD - PwmOut->comp61.Low + PwmOut->ADC_TrigDelayCnt;
		break;
								
		case 6:
			T1 = PwmOut->comp62.High - PwmOut->comp60.High;
			T2 = PwmOut->comp61.High - PwmOut->comp62.High;
			
			if((T1 + T2) < PwmOut->ShiftCntLimMin_x2)
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp60.High = PwmOut->comp60.High - Delta;
					PwmOut->comp60.Low	= PwmOut->comp60.Low	+ Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp61.High = PwmOut->comp61.High + Delta;
					PwmOut->comp61.Low	= PwmOut->comp61.Low	- Delta;
				}
			}
			else
			{
				if(T1 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T1;
					PwmOut->comp62.High = PwmOut->comp62.High + Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	- Delta;
				}
				
				if(T2 < PwmOut->ShiftCntLimMin)
				{
					Delta 							= PwmOut->ShiftCntLimMin - T2;
					PwmOut->comp62.High = PwmOut->comp62.High - Delta;
					PwmOut->comp62.Low	= PwmOut->comp62.Low	+ Delta;
				}
			}
			
			PwmOut->ADC_Trig1Cnt = PwmOut->comp60.High + PwmOut->ADC_TrigDelayCnt;
			PwmOut->ADC_Trig2Cnt = PWM_PERIOD - PwmOut->comp61.Low + PwmOut->ADC_TrigDelayCnt;
		break;
	}
}