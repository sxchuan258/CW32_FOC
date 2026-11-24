/*<PHPHead>
******************************************************************************
*             PHP Electronic Equipments, Co,Ltd                               
*                      (C) All rights reserved                                
******************************************************************************
* $Filename__: GMC_ElimDcBusRip.c
* $Author____: hxt
* $Function__: GMC_ElimDcBusRip.c file 
******************************************************************************
* $Date______:2022-08-07 17:31
* $Variant___:1.00$
* $Revision__:0$
******************************************************************************
* List Of Changes
* $History___:
* 1.00; 2022-08-07 17:31 first edition for GMC_ElimDcBusRip.c -hxt
* 
******************************************************************************
</PHPHead>*/
/* Include Headerfiles  */
#include "GMC_ElimDcBusRip.h"

/* ===================                                                      */
/* Macros Local To This Module                                              */

/* ===========================                                              */
/* Local Module RAM-Definitions (attribute static)                          */

/* ===========================================                              */
/* Definition of variables only local to this module. That is, not to   */
/* be exported to other modules.                                        */

/* Exported Variables Definitions */
/* ============================================================             */

/****************************************************************************/
/*                 Static Function Definitions                              */
/****************************************************************************/

/****************************************************************************/
/*                 Global Definitions                                       */
/****************************************************************************/
const int8_t Table_Vdh[40] = {
	108, 106, 105, 103, 102,
	100, 99,  98,  96,  95,
	94,  93,  91,  90,  89,
	88,  87,  86,  85,  84,
	83,  82,  81,  80,  79,
	78,  77,  77,  76,  75,
	74,  73,  73,  72,  71,
	71,  70,  69,  68,  68,
};

//__attribute__((used,section(".__RW_BASE"))) void GMC_ElimDcBusRip(int16_T DCVolRaw,
void GMC_ElimDcBusRip(int16_t DCVolRaw, uint16_t KdcFactorVp, sType_Complex * pOut_Vs, sType_Complex * pIn_StatVolt)
{
	int8_t i16BaseVp;

	/* 原代码为i16BaseVp = ((int32_t)DCVolRaw  * (int32_t)KdcFactorVp)>>16u
						 pOut_Vs->Real = (((int32_t)pIn_StatVolt->Real)<<15)/i16BaseVp
						 pOut_Vs->Imag = (((int32_t)pIn_StatVolt->Imag)<<15)/i16BaseVp，由于该代码包含除法，严重影响执行时间
		 故改为查表法，DCVolRaw作为输入，查询i16BaseVp的倒数并<<19，新的倒数同样命名i16BaseVp，这样就可以直接使用定子
		 电压乘以新的倒数在>>4就可以得到和原来一样的值。 */
	i16BaseVp = Table_Vdh[(DCVolRaw - 8400)>>7];
	pOut_Vs->Real = ((int32_t)pIn_StatVolt->Real)*i16BaseVp>>4;
	pOut_Vs->Imag = ((int32_t)pIn_StatVolt->Imag)*i16BaseVp>>4;
}

