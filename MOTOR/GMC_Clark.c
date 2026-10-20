/*<PHPHead>
******************************************************************************
*             PHP Electronic Equipments, Co,Ltd                               
*                      (C) All rights reserved                                
******************************************************************************
* $Filename__: GMC_Clark.c
* $Author____: hxt
* $Function__: GMC_Clark.c file 
******************************************************************************
* $Date______:2022-08-04 22:03
* $Variant___:1.00$
* $Revision__:0$
******************************************************************************
* List Of Changes
* $History___:
* 1.00; 2022-08-04 22:03 first edition for GMC_Clark.c -hxt
* 
******************************************************************************
</PHPHead>*/
/* Include Headerfiles  */
#include "GMC_Clark.h"
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


__attribute__((used)) sType_Complex GMCLIB_Clark_F16(sType_PhaseCurr abc)
{
    sType_Complex albe;
    albe.Real = abc.A;
    albe.Imag = ((int32_t)18918 * (abc.B + abc.A + abc.B))>>15;
    return albe;
}
