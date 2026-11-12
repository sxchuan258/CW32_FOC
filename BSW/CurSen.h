#include "global.h"
#include "GMC_SvmStd.h"

sType_PhaseCurr CurSen_CurrentReconstruction(sType_PhaseCurr abc);
void CurSen_PwmPhaseWalk_Init(sType_ActPwms *PwmOut);
void CurSen_PwmPhaseWalk_Run(sType_ActPwms *PwmOut ,sType_SvmOut *Pwm);
