/*<PHPHead>
******************************************************************************
*             PHP Electronic Equipments, Co,Ltd                               
*                      (C) All rights reserved                                
******************************************************************************
* $Filename__: GMC_ParkInv.c
* $Author____: hxt
* $Function__: GMC_ParkInv.c file 
******************************************************************************
* $Date______:2022-08-04 22:12
* $Variant___:1.00$
* $Revision__:0$
******************************************************************************
* List Of Changes
* $History___:
* 1.00; 2022-08-04 22:12 first edition for GMC_ParkInv.c -hxt
* 
******************************************************************************
</PHPHead>*/
/* Include Headerfiles  */
#include "GMC_ParkInv.h"
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

__attribute__((used)) sType_Complex GMCLIB_InvPark_F16(sType_Complex rot,
sType_Complex Angle)
{
    sType_Complex stat;
    stat.Real = (((int32_t)rot.Real*Angle.Imag) - ((int32_t)rot.Imag*Angle.Real))>>15u;
    stat.Imag = (((int32_t)rot.Real*Angle.Real) + ((int32_t)rot.Imag*Angle.Imag))>>15u;
    return stat;
}


