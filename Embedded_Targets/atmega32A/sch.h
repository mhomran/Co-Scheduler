/**
 * @file sch.h
 * @author Mohamed Hassanin 
 * @brief A header file for cooperative scheduler module.
 * @version 0.1
 * @date 2021-03-04
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
