/**
 * @file sch_cfg.h
 * @author Mohamed Hassanin
 * @brief Configuration layer for a cooperative scheduler 
 * based on POSIX.4 signals and POSIX.1 timers.
 * @version 0.1
 * @date 2021-02-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef CFG_H
#define CFG_H
/**********************************************************************
* Preprocessor Constants
**********************************************************************/
/*< the tick value in milliseconds. */
#define TICK 10

/*< The maximum number of tasks in the project */
#define SCH_MAX_TASKS (2)

#endif /* end CFG_H */
/************************* END OF FILE ********************************/
