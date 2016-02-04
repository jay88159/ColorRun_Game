/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-A65
 */

#include <xdc/std.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>
extern const ti_sysbios_family_arm_m3_Hwi_Handle ti_sysbios_family_arm_m3_Hwi0;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle grlibTask1;

#include <ti/sysbios/knl/Queue.h>
extern const ti_sysbios_knl_Queue_Handle LED_Queue;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle QueSem;

#include <ti/sysbios/knl/Queue.h>
extern const ti_sysbios_knl_Queue_Handle Ano_Queue;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle AnoSem;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle TimerTask;

#include <ti/sysbios/family/arm/m3/Hwi.h>
extern const ti_sysbios_family_arm_m3_Hwi_Handle HWI_TIMER2;

#define TI_DRIVERS_WIFI_INCLUDED 0

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

#ifndef ti_sysbios_knl_Task__include
#ifndef __nested__
#define __nested__
#include <ti/sysbios/knl/Task.h>
#undef __nested__
#else
#include <ti/sysbios/knl/Task.h>
#endif
#endif

extern ti_sysbios_knl_Task_Struct TSK_idle;

