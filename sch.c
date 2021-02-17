/**
 * @file sch.h
 * @author Mohamed Hassanin
 * @brief A cooperative scheduler based on POSIX.4 signals and POSIX.1 timers.
 * @version 0.1
 * @date 2021-02-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/**********************************************************************
* Module Preprocessor Constants
**********************************************************************/
#define CLOCKID CLOCK_MONOTONIC
#define SIG SIGRTMIN
/**********************************************************************
* Includes
**********************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
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
  uint32_t Delay; /*< Delay (ticks) until the function will (next) be run */
  uint32_t Period; /*< Interval (ticks) between subsequent runs. */
  int8_t RunMe; /*< Incremented (by scheduler) when task is due to execute */
} TaskConfig_t;
/**********************************************************************
* Module Variable Definitions
**********************************************************************/
static TaskConfig_t Config[SCH_MAX_TASKS];
static timer_t timerid;
/**********************************************************************
* Function Prototypes
**********************************************************************/
static void Sch_GoToSleep(void);
static void Sch_Update(void);
static void TimerHandler(int, siginfo_t*, void*);
static void SigintHandler(int, siginfo_t*, void*);
/**********************************************************************
* Function Definitions
**********************************************************************/
/*********************************************************************
* Function : Sch_Init()
*//**
* \b Description:
*
* This function is used to initialize the scheduler. It should be
* called before any usage of the sheduler functions.
*
* PRE-CONDITION: 1000 > tick value > 0 <br>
*
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
void Sch_Init(void)
{
  uint8_t TaskIndex;
  struct sigevent sev;
  struct sigaction sa;

  //set the task parameters.
  for (TaskIndex = 0; TaskIndex < SCH_MAX_TASKS; TaskIndex++)
    {
      Sch_DeleteTask(TaskIndex);
    }

  //init the timer used for the scheduler.

  /* Establish handler for timer signal */
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = TimerHandler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIG, &sa, NULL) == -1)
    {
      perror("sigaction");
      exit(EXIT_FAILURE);
    }

  /* Establish handler to clear the timer */
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = SigintHandler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGINT, &sa, NULL) == -1)
    {
      perror("sigaction");
      exit(EXIT_FAILURE);
    }

  /* Create the timer */
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_ptr = &timerid;
  if (timer_create(CLOCKID, &sev, &timerid) == -1)
    {
      perror("timer_create");
      exit(EXIT_FAILURE);
    }
}

/*********************************************************************
* Function : Sch_DispatchTasks()
*//**
* \b Description:
*
* This function is used to dispatch/call the tasks that're due to run.
*
* PRE-CONDITION: Sch_Init() is called <br>
* PRE-CONDITION: Any task duration must < tick <br>
* 
* POST-CONDITION: If There's a task that's due will run. Then the CPU will go to sleep.
* The CPU will wakeup in the next tick.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* 
* Sch_DispatchTasks();
* @endcode
*
* @see Sch_Init
*
**********************************************************************/
void Sch_DispatchTasks(void)
{
  uint8_t TaskId;
  
  // Dispatches (runs) the next task (if one is ready)
  for (TaskId = 0; TaskId < SCH_MAX_TASKS; TaskId++)
    {
      if (Config[TaskId].Task != NULL && Config[TaskId].RunMe > 0)
        {
          (*Config[TaskId].Task)(); // Run the task
          Config[TaskId].RunMe -= 1; // Reset / reduce RunMe flag
        }
    }
  
  // The scheduler enters idle mode at this point
  Sch_GoToSleep();
}
/*********************************************************************
* Function : Sch_GoToSleep()
*//**
* \b Description:
*
* Utility function used to make CPU enter sleep mode. It's invoked inside Sch_DispatchTasks
*
* PRE-CONDITION: Sch_Init() is called <br>
* 
* POST-CONDITION: The CPU enters into sleep mode.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* 
* Sch_DispatchTasks();
* @endcode
*
* @see Sch_Init
* @see Sch_DispatchTasks
*
**********************************************************************/
static void Sch_GoToSleep(void)
{
  pause();
}

/*********************************************************************
* Function : Sch_AddTask()
*//**
* \b Description:
*
* This function is used to add task to the scheduler. 
*
* PRE-CONDITION: Sch_Init() is called <br>
*
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
* 
* Sch_AddTask(count, 0, 10); // Make a task starting at 0 tick with period 10 ticks. 
* @endcode
*
* @see Sch_Init
*
**********************************************************************/
uint8_t Sch_AddTask(void (*Function)(),
      const uint32_t Delay,
      const uint32_t Period)
{
  uint8_t TaskId = 0;

  // First find a gap in the array (if there is one)
  while ((Config[TaskId].Task != NULL) && (TaskId < SCH_MAX_TASKS))
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
*
* POST-CONDITION: The task will be deleted.
*
* @param TaskId The id of the task to be deleted.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
* 
* uint8_t taskId = Sch_AddTask(count, 0, 10); 
* 
* Sch_DeleteTask(taskId);
* @endcode
*
* @see Sch_Init
* @see Sch_AddTask
*
**********************************************************************/
void Sch_DeleteTask(const uint8_t TaskId)
{
  Config[TaskId].Task = NULL;
  Config[TaskId].Delay = 0;
  Config[TaskId].Period = 0;
  Config[TaskId].RunMe = 0;
}

/*********************************************************************
* Function : Sch_Update()
*//**
* \b Description:
*
* Utility function used to schedule the tasks at every tick. 
*
* PRE-CONDITION: Sch_Init() is called <br>
*
* POST-CONDITION: The tasks are scheduled according to their configuration.
*
* @return void
*
* \b Example:
* @code
* Sch_Init();
*
* TimerHandler {
*   Sch_Update();
* }
* @endcode
*
* @see Sch_Init
*
**********************************************************************/
static void Sch_Update(void)
{
  uint8_t Index;
  for (Index = 0; Index < SCH_MAX_TASKS; Index++)
    {
      // Check if there is a task at this location
      if (Config[Index].Task != NULL)
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
}

/*********************************************************************
* Function : Sch_Start()
*//**
* \b Description:
*
* This function is used to start the schedule module. 
*
* PRE-CONDITION: Sch_Init() is called <br>
*
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
void Sch_Start(void)
{ 
  struct itimerspec its;    
  /* Start the timer */
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = TICK * 1000000;
  its.it_interval.tv_sec = its.it_value.tv_sec;
  its.it_interval.tv_nsec = its.it_value.tv_nsec;

  if (timer_settime(timerid, 0, &its, NULL) == -1)
    {
      perror("timer_settime");
      exit(EXIT_FAILURE);
    }
}

/*********************************************************************
* Function : TimerHandler()
*//**
* \b Description:
*
* Utility function: a handler for the realtime Timer overrunning. It uses SIGRTMIN signal.
*
* PRE-CONDITION: Sch_Init() is called <br>
* PRE-CONDITION: SIGRTMIN is binded to this handler
*
* POST-CONDITION: A scheduler tick happens.
*
* @param sig SIGRTMIN signal number
* @param si information from the signal sender
* @param uc the context of signal
* @return void
*
* @see Sch_Init
*
**********************************************************************/
static void
TimerHandler(int sig, siginfo_t *si, void *uc)
{
  Sch_Update();
}

/**
 * @brief A handler for SIGINT handler to be invoked when user
 * presses CTRL+C
 * @param SIGINT signal number
 * @param si information from the signal sender
 * @param uc the context of signal
 */
/*********************************************************************
* Function : SigintHandler()
*//**
* \b Description:
*
* Utility function: a handler for the realtime Timer overrunning. It uses SIGINT signal.
*
* PRE-CONDITION: Sch_Init() is called <br>
* PRE-CONDITION: SIGINT is binded to this handler
* PRE-CONDITION: User press CTRL+C
*
* POST-CONDITION: The timer resource is freed, then the program terminates.
*
* @param sig SIGINT signal number
* @param si information from the signal sender
* @param uc the context of signal
* 
* @return void
*
* @see Sch_Init
*
**********************************************************************/
static void
SigintHandler(int sig, siginfo_t* si, void* uc)
{
  timer_delete(timerid);
  exit(EXIT_SUCCESS);
}
/************************* END OF FILE ********************************/
