#include "Control.h"

extern Motor_Type Motor;

int PID_Control(int16_t FeedBack_Value ,int16_t Ref ,PI_Type *PI)
{
	int Error;
	
	Error 	 			= Ref - FeedBack_Value;
	PI->IntegPart = PI->IntegPart + Error;
	PI->Out  			= PI->Kp*Error + PI->Ki*PI->IntegPart;
	PI->Out	 			= PI->Out >> 15;
	
	if(PI->Out < PI->PIMin)
	{
		PI->Out = PI->PIMin;
	}
	else if(PI->Out > PI->PIMax)
	{
		PI->Out = PI->PIMax;
	}
	
	return PI->Out;
}
