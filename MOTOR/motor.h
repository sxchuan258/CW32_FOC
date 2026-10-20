void TerminalVoltageGet(void);
void Motor_SpeedLoopRun(void);
void Motor_CurrentLoopRun(void);
void Motor_ChangePhase(unsigned char PhaseValue);
unsigned char Motor_NewPhaseValueGet(unsigned char OldPhaseValue);
void Motor_SpeedClc(unsigned char NowPhaseValue ,unsigned int RunTime);
void Motor_SpeedClc(unsigned char NowPhaseValue ,unsigned int RunTime);
unsigned char DemagnetizationTimeSet(unsigned int SpeedNow);
