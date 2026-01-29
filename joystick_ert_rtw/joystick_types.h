/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: joystick_types.h
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

#ifndef joystick_types_h_
#define joystick_types_h_
#include "rtwtypes.h"
#include "MW_SVD.h"
#ifndef struct_tag_Pbgev5zVqPH5mwobtiKYmC
#define struct_tag_Pbgev5zVqPH5mwobtiKYmC

struct tag_Pbgev5zVqPH5mwobtiKYmC
{
  int16_T __dummy;
};

#endif                                 /* struct_tag_Pbgev5zVqPH5mwobtiKYmC */

#ifndef typedef_c_arduinodriver_ArduinoSerial_T
#define typedef_c_arduinodriver_ArduinoSerial_T

typedef struct tag_Pbgev5zVqPH5mwobtiKYmC c_arduinodriver_ArduinoSerial_T;

#endif                             /* typedef_c_arduinodriver_ArduinoSerial_T */

#ifndef struct_tag_lEfVKSLRxhO8DhHRAG0Rz
#define struct_tag_lEfVKSLRxhO8DhHRAG0Rz

struct tag_lEfVKSLRxhO8DhHRAG0Rz
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  real_T port;
  real_T dataSizeInBytes;
  real_T dataType;
  real_T sendModeEnum;
  real_T sendFormatEnum;
  c_arduinodriver_ArduinoSerial_T SerialDriverObj;
};

#endif                                 /* struct_tag_lEfVKSLRxhO8DhHRAG0Rz */

#ifndef typedef_codertarget_arduinobase_inter_T
#define typedef_codertarget_arduinobase_inter_T

typedef struct tag_lEfVKSLRxhO8DhHRAG0Rz codertarget_arduinobase_inter_T;

#endif                             /* typedef_codertarget_arduinobase_inter_T */

#ifndef struct_tag_pMPPJgn69ckPBhypf3vQzD
#define struct_tag_pMPPJgn69ckPBhypf3vQzD

struct tag_pMPPJgn69ckPBhypf3vQzD
{
  MW_Handle_Type MW_ANALOGIN_HANDLE;
};

#endif                                 /* struct_tag_pMPPJgn69ckPBhypf3vQzD */

#ifndef typedef_e_arduinodriver_ArduinoAnalog_T
#define typedef_e_arduinodriver_ArduinoAnalog_T

typedef struct tag_pMPPJgn69ckPBhypf3vQzD e_arduinodriver_ArduinoAnalog_T;

#endif                             /* typedef_e_arduinodriver_ArduinoAnalog_T */

#ifndef struct_tag_FIY6N64L77TlG9jHBRqBuB
#define struct_tag_FIY6N64L77TlG9jHBRqBuB

struct tag_FIY6N64L77TlG9jHBRqBuB
{
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  e_arduinodriver_ArduinoAnalog_T AnalogInDriverObj;
};

#endif                                 /* struct_tag_FIY6N64L77TlG9jHBRqBuB */

#ifndef typedef_codertarget_arduinobase_int_o_T
#define typedef_codertarget_arduinobase_int_o_T

typedef struct tag_FIY6N64L77TlG9jHBRqBuB codertarget_arduinobase_int_o_T;

#endif                             /* typedef_codertarget_arduinobase_int_o_T */

/* Parameters (default storage) */
typedef struct P_joystick_T_ P_joystick_T;

/* Forward declaration for rtModel */
typedef struct tag_RTM_joystick_T RT_MODEL_joystick_T;

#endif                                 /* joystick_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
