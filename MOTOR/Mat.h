/**
 * @cond
 ***********************************************************************************************************************
 *
 * Copyright (c) 2015, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES;LOSS OF USE, DATA, OR PROFITS;OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************************************************************/

/*******************************************************************************
**                      Revision Control History                              **
*******************************************************************************/
/* See Mat.c */

#ifndef MAT_H
#define MAT_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "GF_SinCos.h"
#include "GMC_Clark.h"
#include "GMC_Park.h"
#include "GMC_ParkInv.h"
#include "GMC_ElimDcBusRip.h"
#include "GMC_SvmStd.h"

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
/* Function-like macro to multiply two fixed-point factors to get a standard fixed-point product */
#define Mat_FixMul(Factor1, Factor2) ((((int32_t)(Factor1)) * ((int32_t)(Factor2))) >> MAT_FIX_SHIFT)

/* Function-like macro to multiply two fixed-point factors to get a fixed-point product with scaling
 * e.g. scale = 1: shift result left by Scale */
#define Mat_FixMulScale(Factor1, Factor2, Scale) ((((int32_t)(Factor1)) * ((int32_t)(Factor2))) >> ((uint32_t)((int32_t)MAT_FIX_SHIFT - (Scale))))

/* Shift value for fixed-point format */
#define MAT_FIX_SHIFT (15u)

/* Saturation bit for fixed-point format */
#define MAT_FIX_SAT (16u)

/* (1 / sqrt(3)) in fixed-point format */
#define MAT_ONE_OVER_SQRT_3 (18919u)


#define _abs(x) (((x)<0)?(-(x)):(x))
#define _max(a,b) (((a) > (b)) ? (a) : (b))
#define _min(a,b) (((a) < (b)) ? (a) : (b))

/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/


/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/


/* Inline functions ***********************************************************/


/*******************************************************************************
**                      Global Inline Function Definitions                    **
*******************************************************************************/


#endif /* MAT.H */

