/**
 * @file sch.h
 * @author Mohamed Hassanin
 * @brief Header file for a cooperative scheduler based on POSIX.4 signals and POSIX.1 timers.
 * <b>NOTE</b>: it uses SIGRTMIN signal 
 * @version 0.1
 * @date 2021-02-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef SCH_H
#define SCH_H
/**********************************************************************
* Includes
**********************************************************************/
#include <inttypes.h>
/**********************************************************************
* Module Preprocessor Constants
**********************************************************************/
#define TIMER_SIG SIGRTMIN
/**********************************************************************
* Function Prototypes
**********************************************************************/
void Sch_Init(void);
void Sch_Deinit(void);
uint8_t Sch_AddTask(void (*Task) (void), const uint32_t Delay, const uint32_t Interval);
void Sch_DeleteTask(const uint8_t TaskId);
void Sch_Start(void);
void Sch_Update(void);

#endif /* end SCH_H */
/************************* END OF FILE ********************************/
