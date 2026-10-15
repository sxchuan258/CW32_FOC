#include "main.h"

void Flash_Init(void);
void Flash_ReadOpration(uint32_t ReadAddr);
void Flash_UnlockOpration(uint8_t Page_BaseNumber ,uint8_t Page_LongNumber);
void Flash_EraseOpration(uint8_t Page_BaseNumber ,uint8_t Page_LongNumber);
void Flash_WriteOneByte(uint32_t WriteAddr);
