/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: joystick.h
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

#ifndef joystick_h_
#define joystick_h_
#ifndef joystick_COMMON_INCLUDES_
#define joystick_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "MW_SerialRead.h"
#include "MW_SerialWrite.h"
#include "MW_AnalogIn.h"
#endif                                 /* joystick_COMMON_INCLUDES_ */

#include "joystick_types.h"
#include "rt_nonfinite.h"
#include <stddef.h>
#include "MW_target_hardware_resources.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  codertarget_arduinobase_inter_T obj; /* '<Root>/Serial Transmit' */
  codertarget_arduinobase_int_o_T obj_i;/* '<Root>/Y-axis' */
  codertarget_arduinobase_int_o_T obj_n;/* '<Root>/X-axis' */
} DW_joystick_T;

/* Parameters (default storage) */
struct P_joystick_T_ {
  uint8_T BitwiseAND_BitMask;          /* Mask Parameter: BitwiseAND_BitMask
                                        * Referenced by: '<Root>/Bitwise AND'
                                        */
  uint8_T BitwiseAND1_BitMask;         /* Mask Parameter: BitwiseAND1_BitMask
                                        * Referenced by: '<Root>/Bitwise AND1'
                                        */
  real_T Constant1_Value;              /* Expression: 1023
                                        * Referenced by: '<Root>/Constant1'
                                        */
  real_T Constant3_Value;              /* Expression: 1023
                                        * Referenced by: '<Root>/Constant3'
                                        */
  uint8_T Constant_Value;              /* Computed Parameter: Constant_Value
                                        * Referenced by: '<Root>/Constant'
                                        */
  uint8_T Constant2_Value;             /* Computed Parameter: Constant2_Value
                                        * Referenced by: '<Root>/Constant2'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_joystick_T {
  const char_T * volatile errorStatus;
};

/* Block parameters (default storage) */
extern P_joystick_T joystick_P;

/* Block states (default storage) */
extern DW_joystick_T joystick_DW;

/* Model entry point functions */
extern void joystick_initialize(void);
extern void joystick_step(void);
extern void joystick_terminate(void);

/* Real-time Model object */
extern RT_MODEL_joystick_T *const joystick_M;
extern volatile boolean_T stopRequested;
extern volatile boolean_T runModel;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<Root>/Display' : Unused code path elimination
 * Block '<Root>/Display1' : Unused code path elimination
 * Block '<Root>/Display2' : Unused code path elimination
 * Block '<Root>/Display3' : Unused code path elimination
 * Block '<Root>/Display4' : Unused code path elimination
 * Block '<Root>/Display5' : Unused code path elimination
 * Block '<Root>/Display6' : Unused code path elimination
 * Block '<Root>/Display7' : Unused code path elimination
 * Block '<Root>/Display8' : Unused code path elimination
 * Block '<Root>/Scope' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'joystick'
 */
#endif                                 /* joystick_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
