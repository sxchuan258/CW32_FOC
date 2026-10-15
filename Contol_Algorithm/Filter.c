#include "Fliter.h"

unsigned int LowPass_Fliter(unsigned int FliterValue)
{
	unsigned int 				OutPut_Value;
	static unsigned int FliterValue_Old;
	
	OutPut_Value = FliterValue_Old*0.95 + FliterValue*0.05;
	
	FliterValue_Old = FliterValue;
	
	return OutPut_Value;
}
