#include "MKL25Z4.h"
#include "cmsis_os.h"

#include "Board_LED.h"

void  multiplex_Thread (void const *argument);

osThreadDef(multiplex_Thread, osPriorityNormal, 3, 0); //Thread with 3 possible instances

osThreadId T_mux1;																			
osThreadId T_mux2;
osThreadId T_mux3;


osSemaphoreId semMultiplex;						//declare the Semaphore
osSemaphoreDef(semMultiplex);
osSemaphoreId semSignal;
osSemaphoreDef(semSignal);

/*******************************************************************
Create a thread and multiplex the threads with semaphore
********************************************************************/

void multiplex_Thread (void const *argument) 
{
	for (;;) 
	{
    osSemaphoreWait(semMultiplex,osWaitForever);
		LED_On((uint32_t)argument);
		osDelay(100);
		LED_Off((uint32_t)argument);
		osDelay(100);
		osSemaphoreRelease(semMultiplex);
	}
}


int main (void) 
{
	int k = 2;														// NUMBER OF SIMULTANEOUSLY RUNNING THREADS
																				
	osKernelInitialize ();                    
	LED_Initialize();
	
	semMultiplex = osSemaphoreCreate(osSemaphore(semMultiplex), k);	
	
	T_mux1 = osThreadCreate(osThread(multiplex_Thread),(void *)1UL);
	T_mux2 = osThreadCreate(osThread(multiplex_Thread),(void *)2UL);                  
	T_mux3 = osThreadCreate(osThread(multiplex_Thread),(void *)3UL);
 	
	
	osKernelStart ();                         
}

