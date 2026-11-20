#include "Fliter.h"

int32_t LowPass_Fliter(int32_t FliterValue ,int32_t FliterOldValue)
{
	int32_t 				OutPut_Value;
	
	OutPut_Value = FliterOldValue*7/10 + FliterValue*3/10;
	
	return OutPut_Value;
}
