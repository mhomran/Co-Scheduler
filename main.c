/**
 * @file main.c
 * @author Mohamed Hassanin Mohamed
 * @brief two tasks scheduled with a cooperative scheduler based on
 *  POSIX.4 signals and POSIX.1 timers.
 * @version 0.1
 * @date 2021-02-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "sch.h"

static void ClearResources(int sig, siginfo_t* si, void* uc);
void count1(void);
void count2(void);

sig_atomic_t exit_program_flag;

int main(void)
{
  /* Establish sigint handler to clear the timer
  The timer signal should be masked to prevent undesired
  behaviours */
  exit_program_flag = 0;
  struct sigaction sa;

  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = ClearResources;
  if(sigemptyset(&sa.sa_mask) != 0) 
    {
      perror("sigemptyset");
      exit(EXIT_FAILURE);
    }
  if(sigaddset(&sa.sa_mask, TIMER_SIG))
    {
      perror("sigaddset");
      exit(EXIT_FAILURE);
    }
  if (sigaction(SIGINT, &sa, NULL) == -1)
    {
      perror("sigaction");
      exit(EXIT_FAILURE);
    }

  Sch_Init();

  uint8_t task1Id = Sch_AddTask(count1, 0, 100);
  uint8_t task2Id = Sch_AddTask(count2, 1, 50);
  
  Sch_Start();
  
  while(1)
    {
      if(exit_program_flag == 1) 
        {
          Sch_Deinit();
          exit(EXIT_SUCCESS);
        }
      Sch_Update();
    }
  
  return EXIT_SUCCESS;
}

/**
 * @brief A task function for Task1
 * 
 */
void count1(void)
{
  static int counter = 0;
  printf("TASK1: %d\n", counter);
  counter++;
}

/**
 * @brief A task function for Task2
 * 
 */
void count2(void)
{
  static int counter = 0;
  printf("TASK2: \t\t%d\n", counter);
  counter++;
}

/*********************************************************************
* Function : ClearResources()
*//**
* \b Description:
*
* Utility function: a handler for the SIGINT signal.
*
* PRE-CONDITION: SIGINT is binded to this handler
* PRE-CONDITION: User press CTRL+C
* PRE-CONDITION: the timer signal is masked
* POST-CONDITION: The timer resource is freed, then the program terminates.
*
* @param sig SIGINT signal number
* @param si information from the signal sender
* @param uc the context of signal
* 
* @return void
**********************************************************************/
static void
ClearResources(int sig, siginfo_t* si, void* uc)
{
  exit_program_flag = 1;
}
