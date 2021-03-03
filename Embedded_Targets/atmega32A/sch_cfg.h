/**
 * @file sch_cfg.h
 * @author Mohamed Hassanin 
 * @brief A configuration header file for cooperative scheduler module.
 * @version 0.1
 * @date 2021-03-04
 */

#ifndef SCH_CFG_H
#define SCH_CFG_H
/**********************************************************************
* Preprocessor Constants
**********************************************************************/
#define SYSTEM_FREQ (12000000ul) /**< the system clock frequency in Hz */
#define SCHED_FREQ 100 /**< (1/TICK) in Hz */

/*< The maximum number of tasks in the project */
#define SCH_MAX_TASKS (1)

#endif /* end SCH_CFG_H */
/************************* END OF FILE ********************************/
