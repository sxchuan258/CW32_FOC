#include "Fliter.h"

int32_t LowPass_Fliter(int32_t FliterValue ,int32_t FliterOldValue)
{
	int32_t 				OutPut_Value;
	
	/* 由于浮点计算和整数除法都会严重增加处理时间，故使用右移代替除法，右移4位等价于除以16，故FliterOldValue
		 和FliterValue系数的分子加起来要等于16 */
	OutPut_Value = (FliterOldValue*8>>4) + (FliterValue*8>>4);
	
	return OutPut_Value;
}
