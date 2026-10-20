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


//__attribute__((used,section(".__RW_BASE"))) void GMC_ElimDcBusRip(int16_T DCVolRaw,
void GMC_ElimDcBusRip(int16_t DCVolRaw, uint16_t KdcFactorVp, sType_Complex * pOut_Vs, sType_Complex * pIn_StatVolt)
{
    int16_t i16BaseVp;
    i16BaseVp = ((int32_t)DCVolRaw  * (int32_t)KdcFactorVp)>>16u;
    pOut_Vs->Real = (((int32_t)pIn_StatVolt->Real)<<15)/i16BaseVp;
    pOut_Vs->Imag = (((int32_t)pIn_StatVolt->Imag)<<15)/i16BaseVp;
}

