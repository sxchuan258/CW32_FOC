#include "main.h"
#include "global.h"
#include "Flash_Opration.h"
#include "cw32l010_flash.h"

FLASH_Type Flash;

void Flash_Init(void)
{
	/* 开启Flash时钟 */
	__SYSCTRL_FLASH_CLK_ENABLE();
	
	/* 设置读取等待时间 设置为2*HCLK*/
	CW_FLASH->CR2 = 0x5A5A0000 | 0x00000001;
	
	/* 设置Flash的工作模式 */
	Flash.RunMode = Read_Mode;
	
	/* 对所用的页进行解锁，以便于后续的擦除和写入数据 */
	Flash_UnlockOpration(SpeedWrite_PgaeBase ,SpeedWrite_PgaeLong);
	
	/* 对所用的页进行擦除 */
	Flash_EraseOpration(SpeedWrite_PgaeBase ,SpeedWrite_PgaeLong);
	
	/* 设置存储数据初始地址 */
	Flash.SpeedWrite_AddrNow = SpeedWrite_PgaeBase*512;
}

void Flash_ReadOpration(uint32_t ReadAddr)
{
	Flash.Flash_Read[0] = *((uint8_t *)ReadAddr);
	Flash.Flash_Read[1] = *((uint8_t *)ReadAddr+1);
	Flash.Flash_Read[2] = *((uint8_t *)ReadAddr+2);
	Flash.Flash_Read[3] = *((uint8_t *)ReadAddr+3);
}

void Flash_UnlockOpration(uint8_t Page_BaseNumber ,uint8_t Page_LongNumber)
{
	unsigned char i;
	unsigned char Unlock_Flag;
	
	for(i=Page_BaseNumber; i<(Page_BaseNumber+Page_LongNumber); i++)
	{
		Unlock_Flag = FLASH_UnlockPage(i);
		while(Unlock_Flag);
	}
}

void Flash_EraseOpration(uint8_t Page_BaseNumber ,uint8_t Page_LongNumber)
{
	unsigned char i;
	unsigned char Erase_Flag;
	
	for(i=Page_BaseNumber; i<(Page_BaseNumber+Page_LongNumber); i++)
	{
		Erase_Flag = FLASH_ErasePage(i);
		while(Erase_Flag);
	}
}

void Flash_WriteOneByte(uint32_t WriteAddr)
{
	/* 等待进入空闲状态 */
	while( CW_FLASH->ISR_f.BUSY);
	
	/* 清除中断标志位 */
	CW_FLASH->ICR = 0x00;
	
	/* 设置Flash进入写模式 */
	CW_FLASH->CR1 = 0x5A5A0001;
	
	/* 写入数据 */
	*((uint8_t *)WriteAddr) = 0x55;
	
	/* 等待进入空闲状态 */
	while( CW_FLASH->ISR_f.BUSY);
}
