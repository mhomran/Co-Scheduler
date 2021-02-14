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

#include <stdio.h>
#include "sch.h"

void count1(void);
void count2(void);

int main(void)
{
  Sch_Init();

  uint8_t task1Id = Sch_AddTask(count1, 0, 100);
  uint8_t task2Id = Sch_AddTask(count2, 1, 50);
  
  Sch_Start();
  
  while(1)
    {
      Sch_DispatchTasks();
    }
  
  return 0;
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
