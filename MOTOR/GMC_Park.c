/*<PHPHead>
******************************************************************************
*             PHP Electronic Equipments, Co,Ltd                               
*                      (C) All rights reserved                                
******************************************************************************
* $Filename__: GMC_Park.c
* $Author____: hxt
* $Function__: GMC_Park.c file 
******************************************************************************
* $Date______:2022-08-04 22:08
* $Variant___:1.00$
* $Revision__:0$
******************************************************************************
* List Of Changes
* $History___:
* 1.00; 2022-08-04 22:08 first edition for GMC_Park.c -hxt
* 
******************************************************************************
</PHPHead>*/
/* Include Headerfiles  */
#include "GMC_Park.h"
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

__attribute__((used)) sType_Complex GMCLIB_Park_F16(sType_Complex stat, sType_Complex Angle)
{
    sType_Complex rot;
    rot.Real = (((int32_t)stat.Real*Angle.Imag) + ((int32_t)stat.Imag*Angle.Real))>>15u;
    rot.Imag = (((int32_t)stat.Imag*Angle.Imag) - ((int32_t)stat.Real*Angle.Real))>>15u;
    return rot;
}


