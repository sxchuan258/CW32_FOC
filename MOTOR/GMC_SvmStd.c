/*<PHPHead>
******************************************************************************
*             PHP Electronic Equipments, Co,Ltd                               
*                      (C) All rights reserved                                
******************************************************************************
* $Filename__: GMC_SvmStd.c
* $Author____: hxt
* $Function__: GMC_SvmStd.c file 
******************************************************************************
* $Date______:2022-08-04 18:44
* $Variant___:1.00$
* $Revision__:0$
******************************************************************************
* List Of Changes
* $History___:
* 1.00; 2022-08-04 18:44 first edition for GMC_SvmStd.c -hxt
* 
******************************************************************************
</PHPHead>*/
/* Include Headerfiles  */
#include "GMC_SvmStd.h"
/* ===================                                                      */
/* Macros Local To This Module                                              */

/* ===========================                                              */
/* Local Module RAM-Definitions (attribute static)                          */
static uint8_t SectorBuf[2][2][2] = {{{0,2},{6,1}},{{4,3},{5,0}}};

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
__attribute__((__used__)) uint8_t GMC_SvmStd(sType_SvmOut *pSvm, sType_Complex *Vs)
{
    uint8_t sector;
    int16_t U1,U2,U3;
    
    U1 = Vs->Imag;
    /* Vd*sqrt(3)/2 - Vq/2*/
    U2 = (((int32_t)Vs->Real * 28377)>>15) - (Vs->Imag>>1);
    /* -Vd*sqrt(3)/2 - Vq/2*/
    U3 = -(((int32_t)Vs->Real * 28377)>>15) - (Vs->Imag>>1);
    sector = SectorBuf[(U3>0)][(U2>0)][(U1>0)];

    switch(sector)
    {
        case 1u:
        {
            pSvm->Ta = (32767-U1-U2)>>1;
            pSvm->Tb = pSvm->Ta + U2;
            pSvm->Tc = pSvm->Tb + U1;
            break;
        }
        
        case 2u:
        {
            pSvm->Tb = (32767+U2+U3)>>1;
            pSvm->Ta = pSvm->Tb - U2;
            pSvm->Tc = pSvm->Ta - U3;
            break;
        }

        case 3u:
        {
            pSvm->Tb = (32767-U1-U3)>>1;
            pSvm->Tc = pSvm->Tb + U1;
            pSvm->Ta = pSvm->Tc + U3;
            break;
        }

        case 4u:
        {
            pSvm->Tc = (32767+U1+U2)>>1;
            pSvm->Tb = pSvm->Tc - U1;
            pSvm->Ta = pSvm->Tb - U2;
            break;
        }

        case 5u:
        {
            pSvm->Tc = (32767-U2-U3)>>1;
            pSvm->Ta = pSvm->Tc + U3;
            pSvm->Tb = pSvm->Ta + U2;
            break;
        }

        case 6u:
        {
            pSvm->Ta = (32767+U1+U3)>>1;
            pSvm->Tc = pSvm->Ta - U3;
            pSvm->Tb = pSvm->Tc - U1;
            break;
        }
        
        default:
        {
            sector = 0;
            pSvm->Ta = 16384;
            pSvm->Tc = 16384;
            pSvm->Tb = 16384;
            break;
        }
    }
    return sector;
}



