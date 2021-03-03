/**
 * @file main.c
 * @author Mohamed Hassanin
 * @brief A simple program that blinks a led in every second using 
 * cooperative scheduler.
 * @version 0.1
 * @date 2021-03-04
 */
#include <avr/io.h>
#include "sch.h"

void Task1(void);

int
main()
{
  DDRB |= 1 << PINB0; //PINB0 output

  Sch_Init();

  Sch_AddTask(Task1, 0, 100);

  Sch_Start();

  while(1)
    {
      Sch_Update();
    }
  return 0;
}

/**
 * @brief A task function for Task1
 *
 */
void
Task1(void)
{
  PORTB ^= 1 << PINB0;
}

