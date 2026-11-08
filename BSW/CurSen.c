#include "CurSen.h"

extern Motor_Type Motor;
extern signed short Ibus_AdcSample1;
extern signed short Ibus_AdcSample2;

sType_PhaseCurr CurSem_CurrentReconstruction(sType_PhaseCurr abc)
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
