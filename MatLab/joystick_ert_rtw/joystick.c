/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: joystick.c
 *
 * Code generated for Simulink model 'joystick'.
 *
 * Model version                  : 1.12
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Thu Jan 29 02:00:00 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "joystick.h"
#include "joystick_private.h"
#include "rt_nonfinite.h"
#include <math.h>
#include "rtwtypes.h"
#include <stddef.h>

/* Block states (default storage) */
DW_joystick_T joystick_DW;

/* Real-time model */
static RT_MODEL_joystick_T joystick_M_;
RT_MODEL_joystick_T *const joystick_M = &joystick_M_;
real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

/* Model step function */
void joystick_step(void)
{
  real_T tmp;
  uint16_T b_varargout_1;
  uint16_T b_varargout_1_0;
  uint8_T dataIn[2];

  /* MATLABSystem: '<Root>/X-axis' */
  joystick_DW.obj_n.AnalogInDriverObj.MW_ANALOGIN_HANDLE = MW_AnalogIn_GetHandle
    (14UL);
  MW_AnalogInSingle_ReadResult
    (joystick_DW.obj_n.AnalogInDriverObj.MW_ANALOGIN_HANDLE, &b_varargout_1,
     MW_ANALOGIN_UINT16);

  /* MATLABSystem: '<Root>/Y-axis' */
  joystick_DW.obj_i.AnalogInDriverObj.MW_ANALOGIN_HANDLE = MW_AnalogIn_GetHandle
    (15UL);
  MW_AnalogInSingle_ReadResult
    (joystick_DW.obj_i.AnalogInDriverObj.MW_ANALOGIN_HANDLE, &b_varargout_1_0,
     MW_ANALOGIN_UINT16);

  /* DataTypeConversion: '<Root>/Data Type Conversion3' incorporates:
   *  Constant: '<Root>/Constant'
   *  Constant: '<Root>/Constant1'
   *  MATLABSystem: '<Root>/X-axis'
   *  Product: '<Root>/Divide'
   *  Product: '<Root>/Product'
   * */
  tmp = rt_roundd_snf((real_T)((int32_T)b_varargout_1 *
    joystick_P.Constant_Value) / joystick_P.Constant1_Value);
  if (rtIsNaN(tmp) || rtIsInf(tmp)) {
    tmp = 0.0;
  } else {
    tmp = fmod(tmp, 256.0);
  }

  /* MATLABSystem: '<Root>/Serial Transmit' incorporates:
   *  DataTypeConversion: '<Root>/Data Type Conversion3'
   *  S-Function (sfix_bitop): '<Root>/Bitwise AND'
   */
  dataIn[0] = (uint8_T)((uint16_T)(tmp < 0.0 ? (int16_T)(uint8_T)-(int8_T)
    (uint8_T)-tmp : (int16_T)(uint8_T)tmp) & joystick_P.BitwiseAND_BitMask);

  /* DataTypeConversion: '<Root>/Data Type Conversion2' incorporates:
   *  Constant: '<Root>/Constant2'
   *  Constant: '<Root>/Constant3'
   *  MATLABSystem: '<Root>/Y-axis'
   *  Product: '<Root>/Divide1'
   *  Product: '<Root>/Product1'
   * */
  tmp = rt_roundd_snf((real_T)((int32_T)joystick_P.Constant2_Value *
    b_varargout_1_0) / joystick_P.Constant3_Value);
  if (rtIsNaN(tmp) || rtIsInf(tmp)) {
    tmp = 0.0;
  } else {
    tmp = fmod(tmp, 256.0);
  }

  /* MATLABSystem: '<Root>/Serial Transmit' incorporates:
   *  DataTypeConversion: '<Root>/Data Type Conversion2'
   *  S-Function (sfix_bitop): '<Root>/Bitwise AND1'
   */
  dataIn[1] = (uint8_T)((uint16_T)(tmp < 0.0 ? (int16_T)(uint8_T)-(int8_T)
    (uint8_T)-tmp : (int16_T)(uint8_T)tmp) | joystick_P.BitwiseAND1_BitMask);
  MW_Serial_write(joystick_DW.obj.port, &dataIn[0], 2.0,
                  joystick_DW.obj.dataSizeInBytes, joystick_DW.obj.sendModeEnum,
                  joystick_DW.obj.dataType, joystick_DW.obj.sendFormatEnum, 2.0,
                  '\x00', NULL, 0.0, NULL, 0.0);
}

/* Model initialize function */
void joystick_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* Start for MATLABSystem: '<Root>/X-axis' */
  joystick_DW.obj_n.matlabCodegenIsDeleted = false;
  joystick_DW.obj_n.isInitialized = 1L;
  joystick_DW.obj_n.AnalogInDriverObj.MW_ANALOGIN_HANDLE =
    MW_AnalogInSingle_Open(14UL);
  joystick_DW.obj_n.isSetupComplete = true;

  /* Start for MATLABSystem: '<Root>/Y-axis' */
  joystick_DW.obj_i.matlabCodegenIsDeleted = false;
  joystick_DW.obj_i.isInitialized = 1L;
  joystick_DW.obj_i.AnalogInDriverObj.MW_ANALOGIN_HANDLE =
    MW_AnalogInSingle_Open(15UL);
  joystick_DW.obj_i.isSetupComplete = true;

  /* Start for MATLABSystem: '<Root>/Serial Transmit' */
  joystick_DW.obj.matlabCodegenIsDeleted = false;
  joystick_DW.obj.isInitialized = 1L;
  joystick_DW.obj.port = 0.0;
  joystick_DW.obj.dataSizeInBytes = 1.0;
  joystick_DW.obj.dataType = 0.0;
  joystick_DW.obj.sendModeEnum = 0.0;
  joystick_DW.obj.sendFormatEnum = 0.0;
  MW_SCI_Open(0);
  joystick_DW.obj.isSetupComplete = true;
}

/* Model terminate function */
void joystick_terminate(void)
{
  /* Terminate for MATLABSystem: '<Root>/X-axis' */
  if (!joystick_DW.obj_n.matlabCodegenIsDeleted) {
    joystick_DW.obj_n.matlabCodegenIsDeleted = true;
    if ((joystick_DW.obj_n.isInitialized == 1L) &&
        joystick_DW.obj_n.isSetupComplete) {
      joystick_DW.obj_n.AnalogInDriverObj.MW_ANALOGIN_HANDLE =
        MW_AnalogIn_GetHandle(14UL);
      MW_AnalogIn_Close(joystick_DW.obj_n.AnalogInDriverObj.MW_ANALOGIN_HANDLE);
    }
  }

  /* End of Terminate for MATLABSystem: '<Root>/X-axis' */

  /* Terminate for MATLABSystem: '<Root>/Y-axis' */
  if (!joystick_DW.obj_i.matlabCodegenIsDeleted) {
    joystick_DW.obj_i.matlabCodegenIsDeleted = true;
    if ((joystick_DW.obj_i.isInitialized == 1L) &&
        joystick_DW.obj_i.isSetupComplete) {
      joystick_DW.obj_i.AnalogInDriverObj.MW_ANALOGIN_HANDLE =
        MW_AnalogIn_GetHandle(15UL);
      MW_AnalogIn_Close(joystick_DW.obj_i.AnalogInDriverObj.MW_ANALOGIN_HANDLE);
    }
  }

  /* End of Terminate for MATLABSystem: '<Root>/Y-axis' */

  /* Terminate for MATLABSystem: '<Root>/Serial Transmit' */
  if (!joystick_DW.obj.matlabCodegenIsDeleted) {
    joystick_DW.obj.matlabCodegenIsDeleted = true;
  }

  /* End of Terminate for MATLABSystem: '<Root>/Serial Transmit' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
