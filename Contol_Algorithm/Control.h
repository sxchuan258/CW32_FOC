#ifndef __CONTROL_H
#define __CONTROL_H

#include "global.h"

int PID_Control(int16_t FeedBack_Value ,int16_t Ref ,PI_Type *PI);

#endif
