/**
 * @file sch.c
 * @author Mohamed Hassanin 
 * @brief cooperative scheduler module.
 * @version 0.1
 * @date 2021-03-02
 */

/**********************************************************************
* Module Preprocessor Constants
**********************************************************************/
#define CLOCKID CLOCK_MONOTONIC
/**********************************************************************
* Includes
**********************************************************************/
#include <inttypes.h>
#include "sch.h"
#include "sch_cfg.h"
/**********************************************************************
* Typedefs
**********************************************************************/
/**
* Defines the scheduler configuration table’s elements that are used
* by Sch_Init to configure the Scheduler Module.
*/
typedef struct 
{
  void (*Task)(void); /*< a pointer to the task function */
  uint16_t Delay; /*< Delay in ticks until the function runs */
  uint32_t Period; /*< Interval (ticks) between subsequent runs. */
  uint16_t RunMe; /*< Incremented (by scheduler) when task is due to execute */
} TaskConfig_t;
/**********************************************************************
* Module Variable Definitions
**********************************************************************/
static TaskConfig_t Config[SCH_MAX_TASKS];
/**********************************************************************
* Function Prototypes
**********************************************************************/
static void Sch_GoToSleep(void);
/**********************************************************************
* Function Definitions
**********************************************************************/
/*********************************************************************
* Function : Sch_Init()
*//**
* \b Description:
* This function is used to initialize the scheduler. It should be
* called before any usage of the sheduler functions.
*
* PRE-CONDITION: 1000 > tick value > 0 <br>
* POST-CONDITION: The scheduler module is set up.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* @endcode
*
**********************************************************************/
void 
Sch_Init(void)
{
  //set the task parameters.
  for (uint8_t TaskIndex = 0; TaskIndex < SCH_MAX_TASKS; TaskIndex++)
    {
      Sch_DeleteTask(TaskIndex);
    }

  //TODO: configure and init the timer used for the scheduler and its interrupt.
}
/*********************************************************************
* Function : Sch_DispatchTasks()
*//**
* \b Description:
* Utility function is used to dispatch/call the tasks that're due to run.
*
* PRE-CONDITION: Sch_Init() is called <br>
* PRE-CONDITION: Any task duration must < tick <br>
* POST-CONDITION: If There's a task that's due will run. 
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* @endcode
*
* @see Sch_Init
**********************************************************************/
void Sch_DispatchTasks(void)
{
  uint8_t TaskId;
  
  // Dispatches (runs) the next task (if one is ready)
  for (TaskId = 0; TaskId < SCH_MAX_TASKS; TaskId++)
    {
      if (Config[TaskId].Task != 0x0 && Config[TaskId].RunMe > 0)
        {
          (*Config[TaskId].Task)(); // Run the task
          Config[TaskId].RunMe -= 1; // Reset / reduce RunMe flag
        }
    }
}
/*********************************************************************
* Function : Sch_GoToSleep()
*//**
* \b Description:
* Utility function used to make CPU enter sleep mode. It's invoked inside Sch_DispatchTasks
*
* PRE-CONDITION: Sch_Init() is called <br>
* POST-CONDITION: The CPU enters into sleep mode.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* Sch_DispatchTasks();
* @endcode
*
* @see Sch_Init
* @see Sch_DispatchTasks
*
**********************************************************************/
static void 
Sch_GoToSleep(void)
{
  //TODO: Identifiy the sleep instruction
}

/*********************************************************************
* Function : Sch_AddTask()
*//**
* \b Description:
*
* This function is used to add task to the scheduler. 
*
* PRE-CONDITION: Sch_Init() is called <br>
* POST-CONDITION: The task will be added to the scheduler.
*
* @param Function a function pointer to the task function.
* @param Delay a delay before the function executed for its first time
* @param Period the period of the task
*
* @return uint8_t the id of the task
*
* \b Example:
* @code
* Sch_Init();
* Sch_AddTask(count, 0, 10); // Make a task starting at 0 tick with period 10 ticks. 
* @endcode
*
* @see Sch_Init
*
**********************************************************************/
uint8_t 
Sch_AddTask(void (*Function)(),
      const uint32_t Delay,
      const uint32_t Period)
{
  uint8_t TaskId = 0;

  // First find a gap in the array (if there is one)
  while ((Config[TaskId].Task != 0x0) && (TaskId < SCH_MAX_TASKS))
    {
      TaskId++;
    }

  // If we're here, there is a space in the task array
  Config[TaskId].Task = Function;
  Config[TaskId].Delay = Delay;
  Config[TaskId].Period = Period;
  Config[TaskId].RunMe = 0;

  return TaskId;
}

/*********************************************************************
* Function : Sch_DeleteTask()
*//**
* \b Description:
*
* This function is used to delete a task from the scheduler. 
*
* PRE-CONDITION: Sch_Init() is called <br>
* POST-CONDITION: The task will be deleted.
*
* @param TaskId The id of the task to be deleted.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* uint8_t taskId = Sch_AddTask(count, 0, 10); 
* Sch_DeleteTask(taskId);
* @endcode
*
* @see Sch_Init
* @see Sch_AddTask
*
**********************************************************************/
void 
Sch_DeleteTask(const uint8_t TaskId)
{
  Config[TaskId].Task = 0x0;
  Config[TaskId].Delay = 0;
  Config[TaskId].Period = 0;
  Config[TaskId].RunMe = 0;
}

/*********************************************************************
* Function : Sch_Update()
*//**
* \b Description:
*
* this function used to schedule the tasks at every tick. 
*
* PRE-CONDITION: Sch_Init() is called <br>
* POST-CONDITION: The tasks are scheduled according to their configuration.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* @endcode
*
* @see Sch_Init
*
**********************************************************************/
void 
Sch_Update(void)
{
  uint8_t Index;
  for (Index = 0; Index < SCH_MAX_TASKS; Index++)
    {
      // Check if there is a task at this location
      if (Config[Index].Task != 0x0)
        {
          if (Config[Index].Delay == 0)
            {
              // The task is due to run
              Config[Index].RunMe += 1; 
              // Schedule periodic tasks to run again
              Config[Index].Delay = Config[Index].Period - 1;
            }
          else
            {
              // Not yet ready to run: just decrement the delay
              Config[Index].Delay -= 1;
            }
        }
    }
  
  Sch_DispatchTasks();

  // The scheduler enters idle mode at this point
  Sch_GoToSleep();
}

/*********************************************************************
* Function : Sch_Start()
*//**
* \b Description:
*
* This function is used to start the schedule module. 
*
* PRE-CONDITION: Sch_Init() is called <br>
* POST-CONDITION: The scheduler starts.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* Sch_AddTask(count, 0, 10);
* Sch_Start();
* @endcode
*
* @see Sch_Init
*
**********************************************************************/
void 
Sch_Start(void)
{ 
  //TODO: Start the timer */
}

/*********************************************************************
* Function : TimerHandler()
*//**
* \b Description:
*
* Utility function: a handler for the realtime Timer overrunning.
*
* PRE-CONDITION: Sch_Init() is called <br>
* POST-CONDITION: A scheduler tick happens.<br>
*
* @return void
*
* @see Sch_Init
**********************************************************************/
static void
TimerHandler(void)
{
  //Do nothing, just wake up the CPU
	//TODO: clear the timer interrupt flag
}

/**
 * @brief Deinitialize the scheduler module.
 */
/************************* END OF FILE ********************************/
void 
Sch_Deinit(void) {
  //TODO: Stop the timer
}
/************************* END OF FILE ********************************/
