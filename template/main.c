/**
 * @file main.c
 * @author Mohamed Hassanin Mohamed
 * @brief A task scheduled with a cooperative scheduler 
 * @version 0.1
 * @date 2021-03-03
 */
#include "sch.h"

void Task1(void);

int 
main(void)
{
  Sch_Init();

  uint8_t task1Id = Sch_AddTask(Task1, 0, 100);
  
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
	//TODO: put your task here
}
