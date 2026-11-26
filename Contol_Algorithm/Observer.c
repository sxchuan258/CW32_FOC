#include "Observer.h"
#include "Atan.h"

static int32_t sign(uint16_t x ,uint16_t Layer)
{
	if (x > Layer) 	return 65535;
	if (x < -Layer) return -65535;
	
	/* 原代码为：return x/Layer*65535，但是包含除法，影响运算时间。故代入Layer实际值，把65535/Layer化简为410 */
	return x*410;
}

void Smo_ObserverInit(SMO_Type *Smo)
{
	Smo->Uk_Q13 							= 8192 / 4096;
	Smo->Ik_Q13 					 		= 8192 * 5.0 / 16384 / 0.001 / IbusSampleOpa;
	Smo->Ls_PreProcess_Q8  		= (256 / MotorLs) / PWM_FREQUENCY;
	Smo->Rs_PreProcess_Q13 		= MotorRs * Smo->Ik_Q13;
	Smo->Layer_PreProcess_Q13 = 0.1 * Smo->Ik_Q13;
}

uint16_t Smo_Observer(sType_Complex *StatCurr ,sType_Complex *StatVolt ,SMO_Type *Smo)
{
	int32_t E_AlphaOld_z;
	int32_t E_BetaOld_z;
	
	E_AlphaOld_z = Smo->E_Alpha_z;
	E_BetaOld_z	 = Smo->E_Beta_z;
	
	Smo->I_Alpha = StatCurr->Real;
	Smo->I_Beta	 = StatCurr->Imag;
	Smo->V_Alpha = StatVolt->Real;
	Smo->V_Beta  = StatVolt->Imag;

	Smo->I_Alpha_z_Old = Smo->I_Alpha_z>>21;
	Smo->I_Beta_z_Old	 = Smo->I_Beta_z>>21;
	
	Smo->I_Alpha_Err = Smo->I_Alpha - Smo->I_Alpha_z_Old;
	Smo->I_Beta_Err	 = Smo->I_Beta	- Smo->I_Beta_z_Old;
	
	Smo->I_Alpha_z = Smo->I_Alpha_z + Smo->Ls_PreProcess_Q8 * ((Smo->V_Alpha*Smo->Uk_Q13 - Smo->Rs_PreProcess_Q13*Smo->I_Alpha_z_Old)
									 + SMO_K_SLIDE*sign(Smo->I_Alpha_Err ,Smo->Layer_PreProcess_Q13));
	Smo->I_Beta_z	 = Smo->I_Beta_z + Smo->Ls_PreProcess_Q8 * ((Smo->V_Beta*Smo->Uk_Q13 - Smo->Rs_PreProcess_Q13*Smo->I_Beta_z_Old)
									 + SMO_K_SLIDE*sign(Smo->I_Beta_Err ,Smo->Layer_PreProcess_Q13));

	Smo->E_Alpha_z = SMO_K_SLIDE * sign(Smo->I_Alpha_Err ,Smo->Layer_PreProcess_Q13);
	Smo->E_Beta_z  = SMO_K_SLIDE * sign(Smo->I_Beta_Err ,Smo->Layer_PreProcess_Q13);
	
	Smo->E_Alpha_z = LowPass_Fliter(Smo->E_Alpha_z ,E_AlphaOld_z);
	Smo->E_Beta_z  = LowPass_Fliter(Smo->E_Beta_z ,E_BetaOld_z);
	
	Smo->Angle_Est = GFLIB_ATanYX(Smo->E_Beta_z, Smo->E_Alpha_z);
	
	return Smo->Angle_Est;
}
