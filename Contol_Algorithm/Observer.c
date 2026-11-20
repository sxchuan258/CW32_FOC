#include "Observer.h"

static int32_t sign(uint16_t x ,uint16_t Layer)
{
	if (x > Layer) 	return 65536;
	if (x < -Layer) return -65536;
	return x*65536 / Layer;
}

void Smo_ObserverInit(SMO_Type *Smo)
{
	Smo->Uk_Q16 = 65536 * 3.2 / 4096;
	Smo->Ik_Q16 = Smo->Uk_Q16 / 0.001 / 50;
	Smo->Ls_PreProcess_Q10 = (1024 / MotorLs) / PWM_FREQUENCY;
	Smo->Rs_PreProcess_Q16 = MotorRs * Smo->Ik_Q16;
	Smo->Layer_PreProcess_Q16 = 0.1 * Smo->Ik_Q16;
}

void Smo_Observer(sType_Complex *StatCurr ,sType_Complex *StatVolt ,SMO_Type *Smo)
{
	int16_t I_Alpha_Err;
	int16_t I_Beta_Err;
	int32_t E_AlphaOld_z;
	int32_t E_BetaOld_z;
	
	E_AlphaOld_z = Smo->E_Alpha_z;
	E_BetaOld_z	 = Smo->E_Beta_z;
	
	Smo->I_Alpha = StatCurr->Real;
	Smo->I_Beta	 = StatCurr->Imag;
	Smo->V_Alpha = StatVolt->Real;
	Smo->V_Beta  = StatVolt->Imag;

	I_Alpha_Err  = Smo->I_Alpha - Smo->I_Alpha_z;
	I_Beta_Err	 = Smo->I_Beta	- Smo->I_Beta_z;
	
	Smo->I_Alpha_z += Smo->Ls_PreProcess_Q10 * (Smo->V_Alpha*Smo->Uk_Q16 - Smo->Rs_PreProcess_Q16*Smo->I_Alpha_z)
										+ SMO_K_SLIDE*sign(I_Alpha_Err ,Smo->Layer_PreProcess_Q16);
	Smo->I_Beta_z	 += Smo->Ls_PreProcess_Q10 * (Smo->V_Beta*Smo->Uk_Q16 - Smo->Rs_PreProcess_Q16*Smo->I_Beta_z)
										+ SMO_K_SLIDE*sign(I_Beta_Err ,Smo->Layer_PreProcess_Q16);
	
	Smo->E_Alpha_z = SMO_K_SLIDE * sign(I_Alpha_Err ,Smo->Layer_PreProcess_Q16);
	Smo->E_Beta_z  = SMO_K_SLIDE * sign(I_Beta_Err ,Smo->Layer_PreProcess_Q16);
	
	Smo->E_Alpha_z = LowPass_Fliter(Smo->E_Alpha_z ,E_AlphaOld_z);
	Smo->E_Beta_z  = LowPass_Fliter(Smo->E_Beta_z ,E_BetaOld_z);
}
