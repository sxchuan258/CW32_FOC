void TerminalVoltageGet(void);
void Motor_SpeedLoopRun(void);
void Motor_CurrentLoopRun(void);
void Motor_PWMOUT(void);

void Motor_ChangePhase(unsigned char PhaseValue);
unsigned char Motor_NewPhaseValueGet(unsigned char OldPhaseValue);
unsigned char DemagnetizationTimeSet(unsigned int SpeedNow);
